// ----------------------------------------------------------------------
// -----                                                            -----
// -----                     R3BSofFrsAnalysis                      -----
// -----             Created 23/01/20  by J.L. Rodriguez-Sanchez    -----
// ----------------------------------------------------------------------

// ROOT headers
#include "TClonesArray.h"
#include "TF1.h"
#include "TMath.h"
#include "TRandom.h"

// Fair headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include <iomanip>

// R3B headers
#include "R3BMusicHitData.h"

// SOFIA headers
#include "R3BSofFrsAnaPar.h"
#include "R3BSofFrsAnalysis.h"
#include "R3BSofFrsData.h"
#include "R3BSofMwpcHitData.h"
#include "R3BSofSciSingleTcalData.h"

Double_t const c = 299792458.0;     // Light velocity
Double_t const e = 1.60217662e-19;  // Electron charge
Double_t const u = 1.660538921e-27; // Atomic mass unit

// R3BSofFrsAnalysis: Default Constructor --------------------------
R3BSofFrsAnalysis::R3BSofFrsAnalysis()
    : FairTask("R3B-Sof Analysis for FRS", 1)
    , fOffsetAq(0)
    , fOffsetZ(0)
    , fFrs_Par(NULL)
    , fMwpcHitDataCA(NULL)
    , fSciHitDataCA(NULL)
    , fMusicHitDataCA(NULL)
    , fFrsDataCA(NULL)
    , fOnline(kFALSE)
{
}

// R3BSofFrsAnalysisPar: Standard Constructor --------------------------
R3BSofFrsAnalysis::R3BSofFrsAnalysis(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fOffsetAq(0)
    , fOffsetZ(0)
    , fFrs_Par(NULL)
    , fMwpcHitDataCA(NULL)
    , fSciHitDataCA(NULL)
    , fMusicHitDataCA(NULL)
    , fFrsDataCA(NULL)
    , fOnline(kFALSE)
{
}

// Virtual R3BSofFrsAnalysis: Destructor
R3BSofFrsAnalysis::~R3BSofFrsAnalysis()
{
    LOG(INFO) << "R3BSofFrsAnalysis: Delete instance";
    if (fMwpcHitDataCA)
    {
        delete fMwpcHitDataCA;
    }
    if (fSciHitDataCA)
    {
        delete fSciHitDataCA;
    }
    if (fMusicHitDataCA)
    {
        delete fMusicHitDataCA;
    }
    if (fFrsDataCA)
    {
        delete fFrsDataCA;
    }
}

void R3BSofFrsAnalysis::SetParContainers()
{
    // Parameter Container
    // Reading soffrsAnaPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(ERROR) << "FairRuntimeDb not opened!";
    }

    fFrs_Par = (R3BSofFrsAnaPar*)rtdb->getContainer("soffrsAnaPar");
    if (!fFrs_Par)
    {
        LOG(ERROR) << "R3BSofFrsAnalysisPar:: Couldn't get handle on soffrsAnaPar container";
    }
    else
    {
        LOG(INFO) << "R3BSofFrsAnalysisPar:: soffrsAnaPar container open";
    }
}

void R3BSofFrsAnalysis::SetParameter()
{
    //--- Parameter Container ---
    fMagS2Cave = fFrs_Par->GetMagnificationS2Cave();
    fDispS2Cave = fFrs_Par->GetDispersionS2Cave();
    fPathS2Cave = fFrs_Par->GetPathS2Cave();
    fTOFS2Cave = fFrs_Par->GetTOFS2Cave();
    frho_S0_S2 = fFrs_Par->GetRhoS0S2();
    frho_S2_Cave = fFrs_Par->GetRhoS2Cave();
    fBfield_S0_S2 = fFrs_Par->GetBfiedS0S2();
    fBfield_S2_Cave = fFrs_Par->GetBfiedS2Cave();
    fPosFocalS2 = fFrs_Par->GetPosFocalS2();
    fPosFocalCave = fFrs_Par->GetPosFocalCave();
    LOG(INFO) << "R3BSofFrsAnalysis: Rho (S0-S2): " << frho_S0_S2;
    LOG(INFO) << "R3BSofFrsAnalysis: B (S0-S2): " << fBfield_S0_S2;
    LOG(INFO) << "R3BSofFrsAnalysis: Rho (S2-Cave-C): " << frho_S2_Cave;
    LOG(INFO) << "R3BSofFrsAnalysis: B (S2-Cave-C): " << fBfield_S2_Cave;
    LOG(INFO) << "R3BSofFrsAnalysis: Pos for focal plane at S2[mm]: " << fPosFocalS2;
    LOG(INFO) << "R3BSofFrsAnalysis: Pos for focal plane at Cave-C[mm]: " << fPosFocalCave;
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BSofFrsAnalysis::Init()
{
    LOG(INFO) << "R3BSofFrsAnalysis: Init FRS analysis from S2 to Cave-C";

    // INPUT DATA
    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        return kFATAL;
    }

    fMwpcHitDataCA = (TClonesArray*)rootManager->GetObject("Mwpc0HitData");
    if (!fMwpcHitDataCA)
    {
        return kFATAL;
    }

    fMusicHitDataCA = (TClonesArray*)rootManager->GetObject("MusicHitData");
    if (!fMusicHitDataCA)
    {
        return kFATAL;
    }

    fSciHitDataCA = (TClonesArray*)rootManager->GetObject("SofSciSingleTcalData");
    if (!fSciHitDataCA)
    {
        return kFATAL;
    }

    // OUTPUT DATA
    fFrsDataCA = new TClonesArray("R3BSofFrsData", 5);
    if (!fOnline)
    {
        rootManager->Register("SofFrsData", "Analysis FRS", fFrsDataCA, kTRUE);
    }
    else
    {
        rootManager->Register("SofFrsData", "Analysis FRS", fFrsDataCA, kFALSE);
    }
    SetParameter();
    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BSofFrsAnalysis::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BSofFrsAnalysis::Exec(Option_t* option)
{

    // if(++nEvents % 10000 == 0)
    // LOG(INFO) << nEvents ;

    // Reset entries in output arrays, local arrays
    Reset();

    Double_t fZ = 0., fAq = 0.;

    Int_t nHitMwpc = fMwpcHitDataCA->GetEntries();
    Int_t nHitSci = fSciHitDataCA->GetEntries();
    Int_t nHitMusic = fMusicHitDataCA->GetEntries();
    // LOG(INFO) << nHitMusic << " " << nHitSci << " " << nHitMwpc ;
    if (nHitMwpc < 1 || nHitSci < 1 || nHitMusic < 1)
        return; // FIXME:include here warning!

    R3BSofSciSingleTcalData** HitSci = new R3BSofSciSingleTcalData*[nHitSci];
    R3BMusicHitData** HitMusic = new R3BMusicHitData*[nHitMusic];
    R3BSofMwpcHitData** HitMwpc = new R3BSofMwpcHitData*[nHitMwpc];

    // Z from r3b-music ------------------------------------
    Double_t countz = 0;
    for (Int_t i = 0; i < nHitMusic; i++)
    {
        HitMusic[i] = (R3BMusicHitData*)(fMusicHitDataCA->At(i));
        if (HitMusic[i]->GetZcharge() > 1)
        {
            fZ = fZ + HitMusic[i]->GetZcharge();
            countz++;
        }
    }
    if (countz > 0)
    {
        fZ = fZ / countz;
    }
    else
    {
        fZ = 0.;
    }

    // Correction for dependence on angle
    // Add more....

    // Fill the data
    AddData(fZ + fOffsetZ, fAq + fOffsetAq);

    if (HitSci)
        delete HitSci;
    if (HitMwpc)
        delete HitMwpc;
    if (HitMusic)
        delete HitMusic;
    return;
}

// -----   Protected method Finish   --------------------------------------------
void R3BSofFrsAnalysis::Finish() {}

// -----   Public method Reset   ------------------------------------------------
void R3BSofFrsAnalysis::Reset()
{
    LOG(DEBUG) << "Clearing FrsData Structure";
    if (fFrsDataCA)
        fFrsDataCA->Clear();
}

// -----   Private method AddData  --------------------------------------------
R3BSofFrsData* R3BSofFrsAnalysis::AddData(Double_t z, Double_t aq)
{
    // It fills the R3BSofFrsData
    TClonesArray& clref = *fFrsDataCA;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BSofFrsData(z, aq);
}
