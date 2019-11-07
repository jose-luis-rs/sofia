// ------------------------------------------------------------
// -----                R3BSofFragmentTracker             -----
// -----    Created 06/11/19  by J.L. Rodriguez-Sanchez   -----
// -----         Tracking of fragments inside GLAD        -----
// ------------------------------------------------------------

#ifndef R3BSofFragmentTracker_H
#define R3BSofFragmentTracker_H

#include "FairTask.h"

#include <vector>
#include <string>
#include "TVector3.h"

class TClonesArray;
class R3BFieldPar;
class R3BTPropagator;
class R3BTrackingDetector;
class R3BTrackingParticle;
class R3BTrackingSetup;
class R3BFragmentFitterGeneric;

class TH1F;

class R3BSofFragmentTracker : public FairTask
{
  public:
    R3BSofFragmentTracker(const char* name, Bool_t vis = kFALSE, Int_t verbose = 1);
    virtual ~R3BSofFragmentTracker();

    virtual InitStatus Init();
    virtual InitStatus ReInit();

    virtual void SetParContainers();

    virtual void Exec(const Option_t* = "");

    virtual void Finish();
    
    void SetFragmentFitter(R3BFragmentFitterGeneric* fitter) { fFitter = fitter; }
    void SetEnergyLoss(Bool_t energyLoss) { fEnergyLoss = energyLoss; }

  private:
    Bool_t InitPropagator();

    R3BFieldPar* fFieldPar;
    R3BTPropagator* fPropagator;
    TClonesArray* fArrayMCTracks; // simulation output??? To compare?
    TClonesArray* fhitmwp3;
    TClonesArray* fhittof;
    TClonesArray* fhittwim;
    R3BTrackingSetup* fDetectors; // array of R3BTrackingDetector
    std::vector<R3BTrackingParticle*> fFragments;
    TClonesArray* fArrayFragments;
    Int_t fNEvents;
    Bool_t fVis;
    
    R3BFragmentFitterGeneric* fFitter;
    Bool_t fEnergyLoss;
    
    Double_t fAfterGladResolution;

    TH1F* fh_mult_psp;
    TH1F* fh_mult_fi4;
    TH1F* fh_mult_fi5;
    TH1F* fh_mult_tofd;
    TH1F* fh_eloss_psp_mc;
    TH1F* fh_eloss_psp;
    TH1F* fh_eloss_fi4_mc;
    TH1F* fh_eloss_fi4;
    TH1F* fh_ncand;
    TH1F* fh_x_res[6];
    TH1F* fh_x_pull[6];
    TH1F* fh_A_reco1;
    TH1F* fh_A_reco2;
    TH1F* fh_mom_res;
    TH1F* fh_mass_res;
    TH1F* fh_chi2;
    TH1F* fh_vz_res;

    ClassDef(R3BSofFragmentTracker, 1)
};

#endif
