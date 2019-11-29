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
  if(fSingleTcal){
    delete fSingleTcal;
  }
}


InitStatus R3BSofSciTcal2SingleTcal::Init()
{

  LOG(INFO) << "R3BSofSciTcal2SingleTcal: Init";

  FairRootManager* rm = FairRootManager::Instance();
  if (!rm) { 
    LOG(ERROR) << "R3BSofSciTcal2SingleTcal::Init() Couldn't instance the FairRootManager";
    return kFATAL;
  }

  // --- ----------------- --- //
  // --- INPUT MAPPED DATA --- //
  // --- ----------------- --- //

  // scintillator at S2 and cave C
  fTcal = (TClonesArray*)rm->GetObject("SofSciTcalData");  
  if (!fTcal){
    LOG(ERROR)<<"R3BSofSciTcal2SingleTcal::Init() Couldn't get handle on SofSciTcalData container";
    return kFATAL;
  }
  else
    LOG(INFO) << " R3BSofSciTcal2SingleTcal::Init() SofSciTcalData items found";
  

  // --- -------------------------- --- //
  // --- CHECK THE TCALPAR VALIDITY --- //
  // --- -------------------------- --- //
  if(fSingleTcalPar->GetNumSignals()==0){
    LOG(ERROR) << " There are no Tcal parameters for SofSci";
    return kFATAL;
  }
  else
    LOG(INFO) << " Number of signals for SofSci with defined tcal parameters : " << fSingleTcalPar->GetNumSignals();
  

  // --- ---------------- --- //
  // --- OUTPUT TCAL DATA --- //
  // --- ---------------- --- //
  
  // Register output array in tree
  rm->Register("SofSciTcalData","SofSci", fSingleTcal, kTRUE);

  LOG(INFO) << "R3BSofSciTcal2SingleTcal: Init DONE !";

  return kSUCCESS;
}


void R3BSofSciTcal2SingleTcal::SetParContainers()
{
  fRawPosPar = (R3BSofSingleTcalPar*)FairRuntimeDb::instance()->getContainer("SofSingleSciTcalPar");
  if (!fRawPosPar){
    LOG(ERROR) << "R3BSofSciTcal2SingleTcal::SetParContainers() : Could not get access to SofSciTcalPar-Container.";
    return;
  }
  else
    LOG(INFO) << "R3BSofSciTcal2SingleTcal::SetParContainers() : SofSciTcalPar-Container found with " << fSingleTcalPar->GetNumSignals() << " signals";
}


InitStatus R3BSofSciTcal2SingleTcal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}


void R3BSofSciTcal2SingleTcal::Exec(Option_t* option)
{
  ExecRawPos();
}
  
void R3BSofSciTcal2SingleTcal::ExecRawPos()
  UShort_t iDet; // 0-based
  UShort_t iCh;  // 0-based
  Double_t iTraw[fTcal->GetNumDetectors][10];
  UShort_t mult[fTcal->GetNumDetectors*2]; // 2 pmt per plastic
  UShort_t mult_max=0;

  Int_t nHitsPerEvent_SofSci = fTcal->GetEntries();
  for(int ihit=0; ihit<nHitsPerEvent_SofSci; ihit++){
    R3BSofSciTcalData* hit = (R3BSofSciTcalData*)fTcal->At(ihit);
    if(!hit)             continue;
    if(hit->GetPmt()==2) continue; // no interest HERE for the Common reference
    if(mult_max)>10      continue; // events with a multiplicity in a Pmt higher than 10 are discarded

    iDet  = hit->GetDetector()-1;
    iCh   = hit->GetPmt()-1;
    iTraw[iDet*2+iCh][mult[iDet*2+iCh]] = hit->GetTimeRawNs();
    mult[iDet*2+iCh]++;
    if (mult[iDet*2+iCh]>mult_max) mult_max=mult[iDet*2+iCh];
  }// end of loop over the TClonesArray of Tcal data
  
  // TO DO : LOOP OVER THE ENTRIES TO GET ALL THE POSSIBLE COMBINATION AND TO FIND THE GOOD ONE WITHOUT DOUBLE COUNTS

  ++fNevent;

}  

Double_t R3BSofSciTcal2SingleTcal::CalculateRawPos(UInt_t tlns, UInt_t trns)
{
  return(tlns-trns);
}

Double_t R3BSofSciTcal2SingleTcal::CalculateRawTime(UInt_t tlns, UInt_t trns)
{
  return(0.5*(tlns+trns));
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
