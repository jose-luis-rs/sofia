// ------------------------------------------------------------
// -----                  R3BSofTwimOnlineSpectra           -----
// -----    Created 29/09/19  by J.L. Rodriguez-Sanchez   -----
// -----           Fill SOFIA online histograms           -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with SOFIA online data  
 */

#include "R3BSofTwimOnlineSpectra.h"
#include "R3BSofTwimMappedData.h"
#include "R3BEventHeader.h"
#include "THttpServer.h"

#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TFolder.h"
#include "TVector3.h"

#include "TClonesArray.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <array>
#include "TRandom.h"
#include "TMath.h"
#include "TLegend.h"
#include "TLegendEntry.h"

using namespace std;

R3BSofTwimOnlineSpectra::R3BSofTwimOnlineSpectra()
  : FairTask("SofTwimOnlineSpectra", 1)
  , fMappedItemsTwim(NULL)
  , fNEvents(0) 
{
}

R3BSofTwimOnlineSpectra::R3BSofTwimOnlineSpectra(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose)
  , fMappedItemsTwim(NULL)
  , fNEvents(0)
{
}

R3BSofTwimOnlineSpectra::~R3BSofTwimOnlineSpectra() {
  LOG(INFO) << "R3BSofTwimOnlineSpectra::Delete instance";
  if(fMappedItemsTwim) delete fMappedItemsTwim;
}

InitStatus R3BSofTwimOnlineSpectra::Init() {

  LOG(INFO) << "R3BSofTwimOnlineSpectra::Init ";

  // try to get a handle on the EventHeader. EventHeader may not be 
  // present though and hence may be null. Take care when using.

  FairRootManager* mgr = FairRootManager::Instance();
  if (NULL == mgr)
     LOG(FATAL)<<"R3BSofTwimOnlineSpectra::Init FairRootManager not found";
  //header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

  FairRunOnline *run = FairRunOnline::Instance();
  run->GetHttpServer()->Register("",this);

  //get access to mapped data of the TWIM
  fMappedItemsTwim = (TClonesArray*)mgr->GetObject("TwimMappedData");
  if (!fMappedItemsTwim) { return kFATAL;}

  //Create histograms for detectors 
  char Name1[255];
  char Name2[255];

  //TWIM: Map data
  for(Int_t i=0;i<NbSections;i++){
   sprintf(Name1, "Twim_Emap_Sec_%d", i+1);
   sprintf(Name2, "Section %d", i+1);
   cTwimMap_E[i] = new TCanvas(Name1, Name2, 10, 10, 800, 700);
   cTwimMap_E[i]->Divide(4,4);
  }

  for(Int_t i=0;i<NbSections;i++){
   for(Int_t j=0;j<NbAnodes;j++){
    sprintf(Name1, "fh1_twim_Emap_sec%d_a%d", i+1, j+1);
    sprintf(Name2, "Sec %d:Anode %d", i+1, j+1);
    fh1_twimmap_E[i][j] = new TH1F(Name1,Name2,8192,0,8192);
    fh1_twimmap_E[i][j]->GetXaxis()->SetTitle("Energy [channels]");
    fh1_twimmap_E[i][j]->GetYaxis()->SetTitle("Counts");
    fh1_twimmap_E[i][j]->GetYaxis()->SetTitleOffset(1.1);
    fh1_twimmap_E[i][j]->GetXaxis()->CenterTitle(true);
    fh1_twimmap_E[i][j]->GetYaxis()->CenterTitle(true);
    fh1_twimmap_E[i][j]->GetXaxis()->SetLabelSize(0.045);
    fh1_twimmap_E[i][j]->GetXaxis()->SetTitleSize(0.045);
    fh1_twimmap_E[i][j]->GetYaxis()->SetLabelSize(0.045);
    fh1_twimmap_E[i][j]->GetYaxis()->SetTitleSize(0.045);
    cTwimMap_E[i]->cd(j+1);
    fh1_twimmap_E[i][j]->Draw("");
   }
  }


  //TWIM: Map data
  for(Int_t i=0;i<NbSections;i++){
   sprintf(Name1, "Twim_EvsDT_Sec_%d", i+1);
   sprintf(Name2, "Section %d", i+1);
   cTwimMap_EvsDT[i] = new TCanvas(Name1, Name2, 10, 10, 800, 700);
   cTwimMap_EvsDT[i]->Divide(4,4);
  }

  for(Int_t i=0;i<NbSections;i++){
   for(Int_t j=0;j<NbAnodes;j++){
    sprintf(Name1, "fh1_twim_EvsDT_sec%d_a%d", i+1, j+1);
    sprintf(Name2, "Sec %d:Anode %d", i+1, j+1);
    fh2_twim_EneRawVsDriftTime[i][j] = new TH2F(Name1,Name2,1000,0,8192,1000,-15000,15000);
    fh2_twim_EneRawVsDriftTime[i][j]->GetXaxis()->SetTitle("Energy [channels]");
    fh2_twim_EneRawVsDriftTime[i][j]->GetYaxis()->SetTitle("Drift time [channels]");
    fh2_twim_EneRawVsDriftTime[i][j]->GetYaxis()->SetTitleOffset(1.1);
    fh2_twim_EneRawVsDriftTime[i][j]->GetXaxis()->CenterTitle(true);
    fh2_twim_EneRawVsDriftTime[i][j]->GetYaxis()->CenterTitle(true);
    fh2_twim_EneRawVsDriftTime[i][j]->GetXaxis()->SetLabelSize(0.045);
    fh2_twim_EneRawVsDriftTime[i][j]->GetXaxis()->SetTitleSize(0.045);
    fh2_twim_EneRawVsDriftTime[i][j]->GetYaxis()->SetLabelSize(0.045);
    fh2_twim_EneRawVsDriftTime[i][j]->GetYaxis()->SetTitleSize(0.045);
    cTwimMap_EvsDT[i]->cd(j+1);
    fh2_twim_EneRawVsDriftTime[i][j]->Draw("col");
   }
  }



  for(Int_t i=0;i<NbSections;i++){
   sprintf(Name1, "Twim_DTmap_Sec_%d", i+1);
   sprintf(Name2, "Section %d", i+1);
   cTwimMap_DT[i] = new TCanvas(Name1, Name2, 10, 10, 800, 700);
   cTwimMap_DT[i]->Divide(4,4);
  }

  for(Int_t i=0;i<NbSections;i++){
   for(Int_t j=0;j<NbAnodes;j++){
    sprintf(Name1, "fh1_twim_DTmap_sec%d_a%d", i+1, j+1);
    sprintf(Name2, "Sec %d:Anode %d", i+1, j+1);
    fh1_twimmap_DT[i][j] = new TH1F(Name1,Name2,6000,-15000,15000);
    fh1_twimmap_DT[i][j]->GetXaxis()->SetTitle("Drift time [channels]");
    fh1_twimmap_DT[i][j]->GetYaxis()->SetTitle("Counts");
    fh1_twimmap_DT[i][j]->GetYaxis()->SetTitleOffset(1.1);
    fh1_twimmap_DT[i][j]->GetXaxis()->CenterTitle(true);
    fh1_twimmap_DT[i][j]->GetYaxis()->CenterTitle(true);
    fh1_twimmap_DT[i][j]->GetXaxis()->SetLabelSize(0.045);
    fh1_twimmap_DT[i][j]->GetXaxis()->SetTitleSize(0.045);
    fh1_twimmap_DT[i][j]->GetYaxis()->SetLabelSize(0.045);
    fh1_twimmap_DT[i][j]->GetYaxis()->SetTitleSize(0.045);
    cTwimMap_DT[i]->cd(j+1);
    fh1_twimmap_DT[i][j]->Draw("");
   }
  }

  cTwimMap_ESum = new TCanvas("twim_ESum_1,2", "twim_ESum_1,2", 10, 10, 800, 700);
  cTwimMap_ESum->Divide(1,2);
  cTwimMap_ESum->cd(1);
  fh1_twim_ESum[0] = new TH1F("fh1_twim_ESum1", "twim:ESum:first",8192,0,8192);
  fh1_twim_ESum[0]->GetXaxis()->SetTitle("Energy [channels]");
  fh1_twim_ESum[0]->GetYaxis()->SetTitle("Counts");
  fh1_twim_ESum[0]->GetYaxis()->SetTitleOffset(1.1);
  fh1_twim_ESum[0]->GetXaxis()->CenterTitle(true);
  fh1_twim_ESum[0]->GetYaxis()->CenterTitle(true);
  fh1_twim_ESum[0]->GetXaxis()->SetLabelSize(0.045);
  fh1_twim_ESum[0]->GetXaxis()->SetTitleSize(0.045);
  fh1_twim_ESum[0]->GetYaxis()->SetLabelSize(0.045);
  fh1_twim_ESum[0]->GetYaxis()->SetTitleSize(0.045);
  fh1_twim_ESum[0]->Draw("");
  cTwimMap_ESum->cd(2);
  fh1_twim_ESum[1] = new TH1F("fh1_twim_ESum2", "twim:ESum:Second",8192,0,8192);
  fh1_twim_ESum[1]->GetXaxis()->SetTitle("Energy [channels]");
  fh1_twim_ESum[1]->GetYaxis()->SetTitle("Counts");
  fh1_twim_ESum[1]->GetYaxis()->SetTitleOffset(1.1);
  fh1_twim_ESum[1]->GetXaxis()->CenterTitle(true);
  fh1_twim_ESum[1]->GetYaxis()->CenterTitle(true);
  fh1_twim_ESum[1]->GetXaxis()->SetLabelSize(0.045);
  fh1_twim_ESum[1]->GetXaxis()->SetTitleSize(0.045);
  fh1_twim_ESum[1]->GetYaxis()->SetLabelSize(0.045);
  fh1_twim_ESum[1]->GetYaxis()->SetTitleSize(0.045);
  fh1_twim_ESum[1]->Draw("");

  cTwimMap_ESum1 = new TCanvas("twim_ESum", "twim_ESum", 10, 10, 800, 700);
  fh1_twim_ESum[2] = new TH1F("fh1_twim_ESum", "twim:ESum",8192,0,8192);
  fh1_twim_ESum[2]->GetXaxis()->SetTitle("Energy [channels]");
  fh1_twim_ESum[2]->GetYaxis()->SetTitle("Counts");
  fh1_twim_ESum[2]->GetYaxis()->SetTitleOffset(1.1);
  fh1_twim_ESum[2]->GetXaxis()->CenterTitle(true);
  fh1_twim_ESum[2]->GetYaxis()->CenterTitle(true);
  fh1_twim_ESum[2]->GetXaxis()->SetLabelSize(0.045);
  fh1_twim_ESum[2]->GetXaxis()->SetTitleSize(0.045);
  fh1_twim_ESum[2]->GetYaxis()->SetLabelSize(0.045);
  fh1_twim_ESum[2]->GetYaxis()->SetTitleSize(0.045);
  fh1_twim_ESum[2]->Draw("");


  cTwimMap_ESum2 = new TCanvas("twim_E1vsE2", "twim_E1vsE2", 10, 10, 800, 700);
  fh2_twim_ESum = new TH2F("fh2_twim_ESum", "twim: ESum1 vs Esum2",2192,0,8192,2192,0,8192);
  fh2_twim_ESum->GetXaxis()->SetTitle("Energy2 [channels]");
  fh2_twim_ESum->GetYaxis()->SetTitle("Energy1 [channels]");
  fh2_twim_ESum->GetYaxis()->SetTitleOffset(1.1);
  fh2_twim_ESum->GetXaxis()->CenterTitle(true);
  fh2_twim_ESum->GetYaxis()->CenterTitle(true);
  fh2_twim_ESum->GetXaxis()->SetLabelSize(0.045);
  fh2_twim_ESum->GetXaxis()->SetTitleSize(0.045);
  fh2_twim_ESum->GetYaxis()->SetLabelSize(0.045);
  fh2_twim_ESum->GetYaxis()->SetTitleSize(0.045);
  fh2_twim_ESum->Draw("col");


  //MAIN FOLDER-Twim
  TFolder* mainfolTwim = new TFolder("TWIM","TWIM info");  
  for(Int_t i=0;i<NbSections;i++)
  mainfolTwim->Add(cTwimMap_E[i]);
  for(Int_t i=0;i<NbSections;i++)
  mainfolTwim->Add(cTwimMap_DT[i]);
  mainfolTwim->Add(cTwimMap_ESum);
  mainfolTwim->Add(cTwimMap_ESum1);
  mainfolTwim->Add(cTwimMap_ESum2);
  for(Int_t i=0;i<NbSections;i++)
  mainfolTwim->Add(cTwimMap_EvsDT[i]);
  run->AddObject(mainfolTwim);

  //Register command to reset histograms
  run->GetHttpServer()->RegisterCommand("Reset_TWIM_HIST", Form("/Objects/%s/->Reset_Histo()", GetName()));

  return kSUCCESS;
}

void R3BSofTwimOnlineSpectra::Reset_Histo()
{
  LOG(INFO) << "R3BSofTwimOnlineSpectra::Reset_Histo";
  //Map data
  for(Int_t i=0;i<NbSections;i++)
        for(Int_t j=0;j<NbAnodes;j++){
              fh1_twimmap_E[i][j]->Reset();
              fh1_twimmap_DT[i][j]->Reset();
              fh2_twim_EneRawVsDriftTime[i][j]->Reset();
        }
  fh1_twim_ESum[0]->Reset();
  fh1_twim_ESum[1]->Reset();
  fh1_twim_ESum[2]->Reset();
  fh2_twim_ESum->Reset();
}

void R3BSofTwimOnlineSpectra::Exec(Option_t* option) {
  FairRootManager* mgr = FairRootManager::Instance();
  if (NULL == mgr)
    LOG(FATAL) << "R3BSofTwimOnlineSpectra::Exec FairRootManager not found";

  //Fill mapped data
  if(fMappedItemsTwim && fMappedItemsTwim->GetEntriesFast()){
    double e1=0., e2=0., E[NbAnodes];
    for(int i=0;i<NbAnodes;i++)E[i]=0.;//mult=1 !!!
    double DT[NbAnodes+NbSections];
    for(int i=0;i<NbAnodes+NbSections;i++)DT[i]=0.;//mult=1 !!!
    Int_t nHits = fMappedItemsTwim->GetEntriesFast();
    for (Int_t ihit = 0; ihit < nHits; ihit++){
      R3BSofTwimMappedData* hit = 
	(R3BSofTwimMappedData*)fMappedItemsTwim->At(ihit);
      if (!hit) continue;
        fh1_twimmap_E[hit->GetSecID()][hit->GetAnodeID()]->Fill(hit->GetEnergy());
        if(E[hit->GetAnodeID()]==0)E[hit->GetAnodeID()]=hit->GetEnergy();//mult=1 !!!
        if(DT[hit->GetAnodeID()]==0)DT[hit->GetAnodeID()]=hit->GetTime();//mult=1 !!!

       if(hit->GetAnodeID()<8)e1=e1+hit->GetEnergy();
       else e2=e2+hit->GetEnergy();
    }
//   for(int j=0;j<NbSections;j++)
      for(int i=0;i<NbAnodes;i++){
       fh1_twimmap_DT[0][i]->Fill(DT[i]-DT[16]);
       fh2_twim_EneRawVsDriftTime[0][i]->Fill(E[i],DT[i]-DT[16]);
      }
   fh1_twim_ESum[0]->Fill(e1/8.);
   fh1_twim_ESum[1]->Fill(e2/8.);
   fh1_twim_ESum[2]->Fill((e1+e2)/16.);
   fh2_twim_ESum->Fill(e1/8.,e2/8.);
  }

  fNEvents += 1;
}


void R3BSofTwimOnlineSpectra::FinishEvent() {

    if (fMappedItemsTwim)
    {
        fMappedItemsTwim->Clear();
    }

}


void R3BSofTwimOnlineSpectra::FinishTask() {

    if(fMappedItemsTwim){
        for(Int_t i=0;i<NbSections;i++){
                cTwimMap_E[i]->Write();
                cTwimMap_DT[i]->Write();
                cTwimMap_EvsDT[i]->Write();
        }
                fh1_twim_ESum[0]->Write();
                fh1_twim_ESum[1]->Write();
                fh1_twim_ESum[2]->Write();
                fh2_twim_ESum->Write();
    }

}

ClassImp(R3BSofTwimOnlineSpectra)
