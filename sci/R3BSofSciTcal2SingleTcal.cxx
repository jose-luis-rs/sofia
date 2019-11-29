#include "R3BSofSciTcal2SingleTcal.h"
#include "R3BSofSciMappedData.h"

#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"


R3BSofSciTcal2SingleTcal::R3BSofSciTcal2SingleTcal()
  : FairTask("R3BSofSciTcal2SingleTcal",1)
  , fTcal(NULL)
  , fRawPosPar(NULL)
  , fSingleTcal(new TClonesArray("R3BSofSciTcalData"))
  , fNumSingleTcal(0)
  , fNevent(0)
{
}


R3BSofSciTcal2SingleTcal::~R3BSofSciTcal2SingleTcal()
{
  if(fTcal){
    delete fTcal;
  }
  if(fSingleTcal){
    delete fSingleTcal;
  }
}


void R3BSofSciTcal2SingleTcal::SetParContainers()
{
  fRawPosPar = (R3BSofSciSingleTcalPar*)FairRuntimeDb::instance()->getContainer("SofSciSingleTcalPar");
  if (!fRawPosPar){
    LOG(ERROR) << "R3BSofSciTcal2SingleTcal::SetParContainers() : Could not get access to SofSciSingleTcalPar-Container.";
    return;
  }
  else
    LOG(INFO) << "R3BSofSciTcal2SingleTcal::SetParContainers() : SofSciTcalPar-Container found with " << fRawPosPar->GetNumSignals() << " signals";
}

InitStatus R3BSofSciTcal2SingleTcal::Init()
{

  LOG(INFO) << "R3BSofSciTcal2SingleTcal: Init";

  FairRootManager* rm = FairRootManager::Instance();
  if (!rm) { 
    LOG(ERROR) << "R3BSofSciTcal2SingleTcal::Init() Couldn't instance the FairRootManager";
    return kFATAL;
  }

  // --- --------------- --- //
  // --- INPUT TCAL DATA --- //
  // --- --------------- --- //

  // scintillator at S2 and cave C
  fTcal = (TClonesArray*)rm->GetObject("SofSciTcalData");  
  if (!fTcal){
    LOG(ERROR)<<"R3BSofSciTcal2SingleTcal::Init() Couldn't get handle on SofSciTcalData container";
    return kFATAL;
  }
  else
    LOG(INFO) << " R3BSofSciTcal2SingleTcal::Init() SofSciTcalData items found";


  // --- ----------------------- --- //
  // --- OUTPUT SINGLE TCAL DATA --- //
  // --- ----------------------- --- //
  
  // Register output array in tree
  fSingleTcal = new TClonesArray("R3BSofSciSingleTcalData");
  rm->Register("SofSciTcalData","SofSci", fSingleTcal, kTRUE);
  LOG(INFO) << "R3BSofSciMapped2Tcal::Init() R3BSofSciSingleTcalData items created";

  // --- -------------------------- --- //
  // --- CHECK THE TCALPAR VALIDITY --- //
  // --- -------------------------- --- //
  if(fRawPosPar->GetNumSignals()==0){
    LOG(ERROR) << " There are no Tcal parameters for SofSci";
    return kFATAL;
  }
  else
    LOG(INFO) << " Number of signals for SofSci with defined tcal parameters : " << fRawPosPar->GetNumSignals();

  LOG(INFO) << "R3BSofSciTcal2SingleTcal: Init DONE !";

  return kSUCCESS;
}



InitStatus R3BSofSciTcal2SingleTcal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}


void R3BSofSciTcal2SingleTcal::Exec(Option_t* option)
{
  UShort_t iDet; // 0-based
  UShort_t iCh;  // 0-based
  Double_t iTraw[fRawPosPar->GetNumDetectors][16];
  UShort_t mult[fRawPosPar->GetNumDetectors*3];  // 3 channels per SofSci 
  UShort_t mult_max=0;
  
  Int_t nHitsPerEvent_SofSci = fTcal->GetEntries();
  for(int ihit=0; ihit<nHitsPerEvent_SofSci; ihit++){
    R3BSofSciTcalData* hit = (R3BSofSciTcalData*)fTcal->At(ihit);
    if(!hit)             continue;
    if(hit->GetPmt()==2) continue; // no interest HERE for the Common reference
    if(mult_max)>10      continue; // events with a multiplicity in a Pmt higher than 10 are discarded

    iDet  = hit->GetDetector()-1;
    iCh   = hit->GetPmt()-1;
    iTraw[iDet*3+iCh][mult[iDet*3+iCh]] = hit->GetTimeRawNs();
    mult[iDet*3+iCh]++;
    if (mult[iDet*3+iCh]>mult_max) mult_max=mult[iDet*2+iCh];
  }// end of loop over the TClonesArray of Tcal data
  
  // LOOP OVER THE ENTRIES TO GET ALL THE POSSIBLE COMBINATION AND TO FIND THE GOOD ONE WITHOUT DOUBLE COUNTS
  UShort_t maskR; // if mult_max>16, change into UInt_t
  UShort_t maskL; // if mult_max>16, change into UInt_t
  Double_t iRawPos[fRawPosPar->GetNumDetectors()];
  Double_t iRawTime[fRawPosPar->GetNumDetectors()];
  for(UShort_t d=0; d<fRawPosPar->GetNumDetectors();d++) {
    maskR= 0x0;
    maskL= 0x0;
    iRawPos[d] = -1000000.;
    iRawTime[d] = -1000000.;
    for(UShort_t multr=0; multr<mult[d*2]; multr++){
      for(UShort_t multl=0; multl<mult[d*2+1],multl++){
	iRawPos[d] = iTraw[multl]-iTraw[multr]; // Raw position = Tleft - Tright
	if((fRawPosPar->GetSignalTcalParams(0)<=iRawPos)&&(iRawPos<=fRawPosPar->GetSignalTcalParams(1))){
	  // data used, do not use it later
	  // calculate the iRawTime
	  iRawTime = 0.5*(iTraw[multl]+iTraw[multr]);
	}
      }
    }

  }


  ++fNevent;

}  




void R3BSofSciTcal2SingleTcal::FinishEvent()
{
  fSingleTcal->Clear();
  fNumSingleTcal = 0;
}


void R3BSofSciTcal2SingleTcal::FinishTask()
{

}


ClassImp(R3BSofSciTcal2SingleTcal)
