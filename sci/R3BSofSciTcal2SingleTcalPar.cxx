#include "R3BSofSciTcal2SingleTcalPar.h"

#include "R3BSofSciTcalData.h"
#include "R3BSofSingleTcalPar.h"

#include "R3BEventHeader.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "TGeoManager.h"

#include "TClonesArray.h"
#include "TGeoMatrix.h"
#include "TH1F.h"
#include "TF1.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TRandom.h"
#include "TVector3.h"

#include <iostream>
#include <stdlib.h>

// for the engineering run (fNumDetectors(1) instead of fNumDetectors(2)
#define NUMBER_OF_DETECTORS 1 
#define NUMBER_OF_CHANNELS 3 // 2PMT + Tref


// *** ******************************************* *** //
// *** SofSci at S2, Pmt Right (Tcal Data)         *** //
// ***     * det=1                                 *** //
// ***     * channel=1                             *** //
// *** ******************************************* *** //
// *** SofSci at S2, Pmt Left (Tcal Data)          *** //
// ***     * det=1                                 *** //
// ***     * channel=2                             *** //
// *** ******************************************* *** //
// *** SofSci at S2, Common Ref (Tcal Data)        *** //
// ***     * det=1                                 *** //
// ***     * channel=3                             *** //
// *** ******************************************* *** //
// *** SofSci at Cave C, Pmt Right (Tcal Data)     *** //
// ***     * det=2                                 *** //
// ***     * channel=1                             *** //
// *** ******************************************* *** //
// *** SofSci at Cave C, Pmt Left (Tcal Data)      *** //
// ***     * det=2                                 *** //
// ***     * channel=2                             *** //
// *** ******************************************* *** //
// *** SofSci at Cave C, Common Ref (Tcal Data)    *** //
// ***     * det=2                                 *** //
// ***     * channel=3                             *** //
// *** ******************************************* *** //


//R3BSofSciTcal2SingleTcalPar: Default Constructor --------------------------
R3BSofSciTcal2SingleTcalPar::R3BSofSciTcal2SingleTcalPar() 
  : FairTask("R3BSofSciTcal2SingleTcalPar",1)
  , fNumDetectors(NUMBER_OF_DETECTORS)
  , fNumSingleTcalParsPerSignal(2)
  , fMinStatistics(0)
  , fTcal(NULL)
  , fSingleTcalPar(NULL)
  , fOutputFile(NULL) 
{
  fNumSignals = fNumDetectors ;
}

//R3BSofSciTcal2SingleTcalPar: Standard Constructor --------------------------
R3BSofSciTcal2SingleTcalPar::R3BSofSciTcal2SingleTcalPar(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose)
  , fNumDetectors(NUMBER_OF_DETECTORS)
  , fNumSingleTcalParsPerSignal(2)
  , fMinStatistics(0)
  , fTcal(NULL)
  , fSingleTcalPar(NULL)
  , fOutputFile(NULL) 

{
  fNumSignals = fNumDetectors ;
}

//R3BSofSciTcal2SingleTcalPar: Destructor ----------------------------------------
R3BSofSciTcal2SingleTcalPar::~R3BSofSciTcal2SingleTcalPar() 
{
  if(fSingleTcalPar) delete fSingleTcalPar;
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BSofSciTcal2SingleTcalPar::Init() {

  LOG(INFO) << "R3BSofSciTcal2SingleTcalPar: Init";

  FairRootManager* rm = FairRootManager::Instance();
  if (!rm) { return kFATAL;}
  
  // --- --------------- --- //
  // --- INPUT TCAL DATA --- //
  // --- ---------------- --- //

  // scintillator at S2 and cave C
  fTcal = (TClonesArray*)rm->GetObject("SofSciTcalData");      
  if (!fTcal){
    LOG(ERROR)<<"R3BSofSciTcal2SingleTcalPar::Init() Couldn't get handle on SofSciTcalData container";
    return kFATAL;
  }


  // --- ---------------------------------------- --- //
  // --- SOF SCI SINGLE TCAL PARAMETERS CONTAINER --- //
  // --- ---------------------------------------- --- //

  FairRuntimeDb* rtdb = FairRuntimeDb::instance();
  if (!rtdb) { return kFATAL;}  

  fSingleTcalPar=(R3BSofSingleTcalPar*)rtdb->getContainer("SofSciSingleTcalPar");
  if (!fSingleTcalPar) {
    LOG(ERROR)<<"R3BSofSciTcal2SingleTcalPar::Init() Couldn't get handle on SofSciSingleTcalPar container";
    return kFATAL;
  }

  // --- ---------------------- --- //
  // --- HISTOGRAMS DECLARATION --- //
  // --- ---------------------- --- //

  char name[100];  
  fh_RawPosMult1 = new TH1D*[fNumSignals];
  for(Int_t det=0; det<fNumDetectors; det++){
    sprintf(name,"PosRaw_Sci%i",det+1);
    fh_RawPosMult1[det] = new TH1F(name,name,100000,-5000,5000);
  }
  
  return kSUCCESS;
}

// -----   Public method ReInit   --------------------------------------------
InitStatus R3BSofSciTcal2SingleTcalPar::ReInit() {
  
  
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void R3BSofSciTcal2SingleTcalPar::Exec(Option_t* opt) {

  // --- ------------------------------ --- //
  // --- LOOP OVER TCAL HITS FOR SofSci --- //
  // --- ------------------------------ --- //
  
  // nHitsSci = number of hits per event
  UInt_t nHitsSci = fTcal->GetEntries();    // can be very high especially for S2 detector
  UShort_t mult[NUMBER_OF_DETECTORS*NUMBER_OF_CHANNELS];
  Double_t iRawTimeNs[NUMBER_OF_DETECTORS*NUMBER_OF_CHANNELS];
  UShort_t iDet; // 0 based Det number
  UShort_t iPmt; // 0 based Pmt number

  for(UShort_t d=0; d<NUMBER_OF_DETECTORS; d++){
    iLeftRawTimeNs[d] = 0;
    iRightRawTimeNs[d] = 0;
    for(UShort_t ch=0; ch<NUMBER_OF_CHANNELS; ch++){
      mult[d*NUMBER_OF_CHANNELS+ch] = 0;
    }
  }

  // CALCULATE THE MULTIPLICITY FOR EACH SIGNAL
  for (UInt_t ihit=0; ihit<nHitsSci; ihit++){
    R3BSofSciTcalData* hitSci = (R3BSofSciTcalData*)fTcal->At(ihit);
    if (!hitSci){
      LOG(WARNING) << "R3BSofSciTcal2SingleTcalPar::Exec() : could not get hitSci";
      continue; // should not happen
    }           
    iDet = hitSci->GetDetector()-1; // get the 0 based Det number
    iPmt = hitSci->GetPmt()-1;      // get the 0 based Pmt number
    iRawTimeNs[iDet*NUMBER_OF_CHANNELS+iPmt] = hitSci->GetTimeRawNs();
    mult[iDet*NUMBER_OF_CHANNELS+iPmt]++;
  }// end of for(ihit) 

  // FILL THE HISTOGRAM ONLY FOR MULT=1 IN RIGHT AND MULT=1 IN LEFT
  for(UShort_t d=0; d<NUMBER_OF_DETECTORS; d++){
    // check if mult=1 at RIGHT PMT [0,3] and mult=1 at LEFT PMT [1,4]
    if((mult[d*NUMBER_OF_CHANNELS]==1)&&(mult[d*NUMBER_OF_CHANNELS+1]==1)){
       fh_RawPosMult1[d]->Fill(iRawTimeNs[d*NUMBER_OF_CHANNELS+1]-iRawTimeNs[d*NUMBER_OF_CHANNELS]);
    }
  }
}

// ---- Public method Reset   --------------------------------------------------
void R3BSofSciTcal2SingleTcalPar::Reset() 
{
}

void R3BSofSciTcal2SingleTcalPar::FinishEvent() 
{
}

// ---- Public method Finish   --------------------------------------------------
void R3BSofSciTcal2SingleTcalPar::FinishTask() 
{  
  CalculateRawPosSingleTcalParams();
  fSingleTcalPar->printParams();
}


//------------------
void R3BSofSciTcal2SingleTcalPar::CalculateRawPosSingleTcalParams()
{
  LOG(INFO) << "R3BSofSciTcal2SingleTcalPar: CalculateVftxSingleTcalParams()";
  
  fSingleTcalPar->SetNumDetectors(fNumDetectors);
  fSingleTcalPar->SetNumSignals(fNumDetectors);
  fSingleTcalPar->SetNumSingleTcalParsPerSignal(fNumSingleTcalParsPerSignal);

  UInt_t IntegralTot;
  UInt_t IntegralPartial;
  Double_t Bin2Ns[fNumSingleTcalParsPerSignal];

  for(Int_t sig=0; sig<fNumSignals; sig++){
    if(fh_RawPosMult1[sig]->GetEntries()>fMinStatistics){
      IntegralTot = fh_TimeFineBin[sig]->Integral();
      IntegralPartial = 0;
      for(Int_t bin=0; bin<fNumSingleTcalParsPerSignal; bin++){
	IntegralPartial+=fh_TimeFineBin[sig]->GetBinContent(bin+1);
	Bin2Ns[bin] = 5.*((Double_t)IntegralPartial) / (Double_t)IntegralTot;
	fh_TimeFineNs[sig]->SetBinContent(bin+1,Bin2Ns[bin]);
	fSingleTcalPar->SetSignalSingleTcalParams(Bin2Ns[bin],sig*fNumSingleTcalParsPerSignal+bin);
      }
    }
    fh_TimeFineNs[sig]->Write(); // empty histo if stat <fMinStatistics
    fh_TimeFineBin[sig]->Write();
  }

  Double_t iMax;
  Int_t bin, binLimit; 
  Double_t RawPosLimit[fNumSingleTcalParsPerSignal];
  for(Int_t sig=0; sig<fNumSignals; sig++){
    if(fh_RawPosMult1[sig]->GetEntries()>fMinStatistics){
      iMax = fh_RawPosMult1[sig]->GetMaximum();
      //LOWER LIMIT
      bin=1;
      binLimit=1;
      while ((bin<=fh_RawPosMult1[sig]->GetNbinsX())||(binLimit>1)){
	if(fh_RawPosMult1[sig]->GetBinContent(bin)>iMax/10000.) binLimit=bin;
	bin++;
      }
      fSingleTcalPar->SetSignalSingleTcalParams(fh_RawPosMult1[0]->GetBinLowEdge(binLimit));
      //HIGHER LIMIT
      bin=fh_RawPosMult1[sig]->GetNbinsX();
      binLimit=fh_RawPosMult1[sig]->GetNbinsX();
      while ((bin>=1)||(binLimit<fh_RawPosMult1[sig]->GetNbinsX())){
	if(fh_RawPosMult1[sig]->GetBinContent(bin)>iMax/10000.) binLimit=bin;
	bin--;
      }
      fSingleTcalPar->SetSignalSingleTcalParams(fh_RawPosMult1[1]->GetBinLowEdge(binLimit));
    }
    fh_RawPosMult1[sig]->Write();
  }

  fSingleTcalPar->setChanged();
  return;
  
}

ClassImp(R3BSofSciTcal2SingleTcalPar)
  
