// ----------------------------------------------------------------
// -----          R3BSofTofWDigitizer source file             -----
// -----         Created 03/11/19  by JL Rodriguez            -----
// ----------------------------------------------------------------

#include "R3BSofTofWDigitizer.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"

// includes for modeling
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"

#include "TMath.h"
#include "TRandom.h"
#include "TVector3.h"
#include <iostream>
#include <string>

#include "R3BSofToFWPoint.h"
#include "R3BMCTrack.h"

// R3BSofTofWDigitizer: Default Constructor --------------------------
R3BSofTofWDigitizer::R3BSofTofWDigitizer()
    : FairTask("R3BSof Tof Digitization scheme", 1)
    , fMCTrack(NULL)
    , fTofPoints(NULL)
    , fTofHits(NULL)
    , fsigma_y(0.1)   // sigma=1mm
    , fsigma_t(0.017) // sigma=17ps
    , fPosX(0.)
    , fPosZ(0.)
    , fangle(0.)
    , fsigma_ELoss(0.)
{
}

// R3BSofTofWDigitizer: Standard Constructor --------------------------
R3BSofTofWDigitizer::R3BSofTofWDigitizer(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMCTrack(NULL)
    , fTofPoints(NULL)
    , fTofHits(NULL)
    , fsigma_y(0.1)
    , fsigma_t(0.017)
    , fPosX(0.)
    , fPosZ(0.)
    , fangle(0.)
    , fsigma_ELoss(0.)
{
}

// Virtual R3BSofTofWDigitizer: Destructor ----------------------------
R3BSofTofWDigitizer::~R3BSofTofWDigitizer()
{
    LOG(INFO) << "R3BSofTofWDigitizer: Delete instance";
    if (fTofPoints)
        delete fTofPoints;
    if (fTofHits)
        delete fTofHits;
}

// ----   Public method Init  -----------------------------------------
InitStatus R3BSofTofWDigitizer::Init()
{

    LOG(INFO) << "R3BSofTofWDigitizer: Init";

    // Get input array
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        LOG(fatal) << "Init: No FairRootManager";

    fMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
    fTofPoints = (TClonesArray*)ioman->GetObject("SofTofWallPoint");

    // Register output array fTofHits
    //fTofHits = new TClonesArray("R3BSofTofWHitData", 10);
    //ioman->Register("TofWHit", "Digital response in TofW", fTofHits, kTRUE);
    fTofHits = new TClonesArray("R3BHit", 10);
    ioman->Register("TofWHit", "Digital response in TofW", fTofHits, kTRUE);

    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BSofTofWDigitizer::Exec(Option_t* opt)
{
    Reset();
    // Reading the Input -- Point Data --
    Int_t nHits = fTofPoints->GetEntries();
    if (!nHits)
        return;
    // Data from Point level
    R3BSofToFWPoint** pointData;
    pointData = new R3BSofToFWPoint*[nHits];
    UChar_t paddle = 0;
    Int_t TrackId = 0, PID = 0, mother=-1;
    Double_t x = 0., y = 0., z = 0., time = 0.;
    for (Int_t i = 0; i < nHits; i++)
    {
        pointData[i] = (R3BSofToFWPoint*)(fTofPoints->At(i));
        TrackId = pointData[i]->GetTrackID();

        R3BMCTrack *Track = (R3BMCTrack*) fMCTrack->At(TrackId);   
        PID = Track->GetPdgCode();
        mother = Track->GetMotherId();

        if (PID > 1000080160 && mother<0)//Z=8 and A=16
        {
            Double_t fX_in = pointData[i]->GetXIn();
            Double_t fY_in = pointData[i]->GetYIn();
            Double_t fZ_in = pointData[i]->GetZIn();
            Double_t fX_out = pointData[i]->GetXOut();
            Double_t fY_out = pointData[i]->GetYOut();
            Double_t fZ_out = pointData[i]->GetZOut();
            paddle = pointData[i]->GetDetCopyID();

            x = ((fX_in + fX_out) / 2.);
            y = ((fY_in + fY_out) / 2.) + gRandom->Gaus(0., fsigma_y);
            z = ((fZ_in + fZ_out) / 2.);

            x = (((x - fPosX) * cos(fangle * TMath::DegToRad())) - ((z - fPosZ) * sin(fangle * TMath::DegToRad())));

            time = pointData[i]->GetTime() + gRandom->Gaus(0., fsigma_t);
            //AddHitData(paddle, x, y, time);
            AddR3BHitData(7, x, y, 0., time);
        }
    }
    if (pointData)
        delete pointData;
    return;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BSofTofWDigitizer::ReInit() { return kSUCCESS; }

// -----   Public method Reset   -----------------------------------------------
void R3BSofTofWDigitizer::Reset()
{
    LOG(DEBUG) << "Clearing R3BSofTofWDigitizer Structure";
    if (fTofHits)
        fTofHits->Clear();
}

// -----   Private method AddHitData  -------------------------------------------
R3BSofTofWHitData* R3BSofTofWDigitizer::AddHitData(UChar_t paddle, Double_t x, Double_t y, Double_t time)
{
    // It fills the R3BSofTofWHitData
    TClonesArray& clref = *fTofHits;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BSofTofWHitData(paddle, x, y, time);
}

// -----   Private method AddR3BHitData  -------------------------------------------
R3BHit* R3BSofTofWDigitizer::AddR3BHitData(Int_t detId, Double_t x, Double_t y, Double_t eloss, Double_t time)
{
    // It fills the R3BHit
    TClonesArray& clref = *fTofHits;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BHit(detId, x, y, eloss, time);
}

// -----   Public method Finish  ------------------------------------------------
void R3BSofTofWDigitizer::Finish() {}

ClassImp(R3BSofTofWDigitizer)
