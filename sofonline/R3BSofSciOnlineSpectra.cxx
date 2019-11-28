// ------------------------------------------------------------
// -----                  R3BSofSciOnlineSpectra           -----
// -----           Fill SOFIA online histograms           -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with SOFIA online data
 */

#include "R3BSofSciOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BSofSciMappedData.h"
#include "THttpServer.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TCanvas.h"
#include "TFolder.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TVector3.h"

#include "TClonesArray.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TMath.h"
#include "TRandom.h"
#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>


R3BSofSciOnlineSpectra::R3BSofSciOnlineSpectra()
    : FairTask("SofSciOnlineSpectra", 1)
    , fMappedItemsSci(NULL)
    , fNEvents(0)
{
}

R3BSofSciOnlineSpectra::R3BSofSciOnlineSpectra(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItemsSci(NULL)
    , fNEvents(0)
{
}

R3BSofSciOnlineSpectra::~R3BSofSciOnlineSpectra()
{
    LOG(INFO) << "R3BSofSciOnlineSpectra::Delete instance";
    if (fMappedItemsSci)
        delete fMappedItemsSci;
}

InitStatus R3BSofSciOnlineSpectra::Init()
{

    LOG(INFO) << "R3BSofSciOnlineSpectra::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(FATAL) << "R3BSofSciOnlineSpectra::Init FairRootManager not found";
    // header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // get access to mapped data of the TWIM
    fMappedItemsSci = (TClonesArray*)mgr->GetObject("SofSciMappedData");
    if (!fMappedItemsSci)
    {
        return kFATAL;
    }

    // Create histograms for detectors
    char Name1[255];
    char Name2[255];

    for (Int_t i = 0; i < NbDetectors; i++)    {
      sprintf(Name1, "SofSci%i_MultAndFt",i+1);
      cSciMult[i] = new TCanvas(Name1, Name1, 10, 10, 800, 700);
      cSciMult[i]->Divide(4, 4);
      for (Int_t j = 0; j < NbChannels; j++)    {
	sprintf(Name1, "SofSci%i_FineTimeCh%i",i+1,j+1);
	fh1_finetime[i*NbChannels+j] = new TH1I(Name1,Name1,1000,0,1000);
	cSciMult[i]->cd(j+1);
	fh1_finetime[i*NbChannels+j]->Draw("");
      }
      sprintf(Name1,"SofSci%i_MultPerChannel",i+1);
      fh2_mult[i] = new TH2I(Name1,Name1,NbChannels+1,0,NbChannels+1,20,0,20);
    }

    // MAIN FOLDER-Sci
    TFolder* mainfolSci = new TFolder("SOFSCI", "SOFSCI info");
    for (Int_t i = 0; i < NbDetectors; i++)
        mainfolSci->Add(cSciMult[i]);
    run->AddObject(mainfolSci);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_SOFSCI_HIST", Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

void R3BSofSciOnlineSpectra::Reset_Histo()
{
    LOG(INFO) << "R3BSofSciOnlineSpectra::Reset_Histo";
    // Mapped data
    for (Int_t i = 0; i < NbDetectors; i++)
    {
        fh2_mult[i]->Reset();
        for (Int_t j = 0; j < NbChannels; j++)
        {
            fh1_finetime[i*NbChannels+j]->Reset();
        }
    }
}

void R3BSofSciOnlineSpectra::Exec(Option_t* option)
{
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(FATAL) << "R3BSofSciOnlineSpectra::Exec FairRootManager not found";

    // Fill mapped data
    if (fMappedItemsSci && fMappedItemsSci->GetEntriesFast()){
      Int_t nHits = fMappedItemsSci->GetEntriesFast();
      UShort_t iDet; // 0-bsed
      UShort_t iCh;  // 0-based
      UShort_t mult[NbDetectors*NbChannels];
      for(UShort_t i=0; i<NbDetectors; i++){
	for(UShort_t j=0; j<NbChannels; j++){
	  mult[i*NbChannels+j]=0;
	}
      }
      for (Int_t ihit = 0; ihit < nHits; ihit++){
	R3BSofSciMappedData* hit = (R3BSofSciMappedData*)fMappedItemsSci->At(ihit);
	if (!hit) continue;
	iDet = hit->GetDetector()-1;
	iCh  = hit->GetPmt()-1;
	mult[iDet*NbChannels+iCh]++;
	fh1_finetime[iDet*NbChannels+iCh]->Fill(hit->GetTimeFine());
      }
      for(UShort_t i=0; i<NbDetectors; i++){
	for(UShort_t j=0; j<NbChannels; j++){
	  fh2_mult[i]->Fill(j+1,mult[i*NbChannels+j]);
	}
      }
    }
    fNEvents += 1;
}

void R3BSofSciOnlineSpectra::FinishEvent()
{
    if (fMappedItemsSci){
      fMappedItemsSci->Clear();
    }
}


void R3BSofSciOnlineSpectra::FinishTask()
{

  if (fMappedItemsSci){
    for (UShort_t i = 0; i < NbDetectors; i++){
      fh2_mult[i]->Write();
      cSciMult[i]->Write();
      for(UShort_t j=0; j<NbChannels; j++){
	fh1_finetime[i*NbChannels+j]->Write();
      }
    }
  }
}

 ClassImp(R3BSofSciOnlineSpectra)
