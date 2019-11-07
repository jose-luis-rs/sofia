// ------------------------------------------------------------
// -----                R3BSofFragmentTracker             -----
// -----    Created 06/11/19  by J.L. Rodriguez-Sanchez   -----
// -----         Tracking of fragments inside GLAD        -----
// ------------------------------------------------------------

#include "R3BSofFragmentTracker.h"
#include "R3BGladFieldMap.h"
#include "R3BMCTrack.h"
#include "R3BTPropagator.h"
#include "R3BTrackingDetector.h"
#include "R3BTrackingSetup.h"
#include "R3BTGeoPar.h"
#include "R3BTrackingParticle.h"
#include "R3BHit.h"
#include "R3BFragmentFitterGeneric.h"

#include "FairRootManager.h"
#include "FairLogger.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TArc.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLine.h"



TVector3 v1;
double c=29.9792458;

using namespace std;

#define SPEED_OF_LIGHT 29.9792458 // cm/ns

/* For the tracking we use a user-defined list of TrackingDetectors,
 * stored in a TClonesArrays. The TrackingDetectors will provide
 * detector properties as well as hit coordinates to which we want to
 * fit the track.
 *
 * The tracker also needs a (measured) time-of-flight and a charge.
 * The charge is part of the Twim hit level.
 * The time-of-flight can be constructed from the hits of
 * start and stop detector (ToF-Wall).
 *
 * Unfortunately, the HIT level data is not universal, hence, we cannot use the
 * same generic code for all detectors. We *could* have X,Y,C,T as basic structure
 * for ALL hit data. But not all detectors provide all variables.
 * We could inherit getters from a common base class and return NAN in the
 * overloaded members. Can fix this later. For now we can manually construct
 * a list of detectors with X,Y and additionaly fetch TOF and charge.
 *
 */

R3BSofFragmentTracker::R3BSofFragmentTracker(const char* name, Bool_t vis, Int_t verbose)
    : FairTask(name, verbose)
    , fFieldPar(NULL)
    , fPropagator(NULL)
    , fArrayMCTracks(NULL)
    , fhitmwp3(NULL)
    , fhittof(NULL)
    , fhittwim(NULL)
    , fDetectors(new R3BTrackingSetup())
    , fArrayFragments(new TClonesArray("R3BTrackingParticle"))
    , fNEvents(0)
    , fVis(vis)
    , fFitter(nullptr)
    , fEnergyLoss(kTRUE)
{
    // this is the list of detectors (active areas) we use for tracking
    fDetectors->AddDetector("target", kTarget, "TargetGeoPar");
    fDetectors->AddDetector("mwpc1", kTargetGlad, "mwpc1GeoPar", "Mwpc1Hit");
    fDetectors->AddDetector("mwpc2", kTargetGlad, "mwpc2GeoPar", "Mwpc2Hit");
    fDetectors->AddDetector("mwpc3", kAfterGlad, "mwpc3GeoPar", "Mwpc3Hit");
    fDetectors->AddDetector("tofw", kTof, "tofwGeoPar", "TofWHit");
}

R3BSofFragmentTracker::~R3BSofFragmentTracker() {}

InitStatus R3BSofFragmentTracker::Init()
{
    FairRootManager* man = FairRootManager::Instance();

    fArrayMCTracks = (TClonesArray*)man->GetObject("MCTrack");
    if (NULL == fArrayMCTracks)
    {
        LOG(ERROR) << "No MC Track array found in input file.";
        return kERROR;
    }

    fhitmwp3 = (TClonesArray*)man->GetObject("Mwpc3Hit");
    fhittof = (TClonesArray*)man->GetObject("TofWHit");
    fhittwim = (TClonesArray*)man->GetObject("TwimHit");

   // man->Register("TrackingParticle", "Tracking", fArrayFragments, kTRUE);

    if (!InitPropagator())
    {
        return kERROR;
    }

    fDetectors->Init();


    fh_A_reco2 = new TH1F("h_A_reco", "Reconstructed mass", 400., 30., 150.);

 
    return kSUCCESS;
}

InitStatus R3BSofFragmentTracker::ReInit()
{
    SetParContainers();

    if (!InitPropagator())
    {
        return kERROR;
    }

    return kSUCCESS;
}

void R3BSofFragmentTracker::SetParContainers()
{
    fFieldPar = (R3BFieldPar*)FairRuntimeDb::instance()->getContainer("R3BFieldPar");

    fDetectors->SetParContainers();
}

void R3BSofFragmentTracker::Exec(const Option_t*)
{
    /* this part needs to be adopted to each experiment / setup
     *
     * First thing is to tell each detector which hit we want to use
     * for the track fitting. This means: manually copy the hit we want to
     * use for tracking from the TClonesArray into the single hit structure.
     *
     * Then call fit_fragment() to optimize the track and calculate
     * particle properties.
     */
    fDetectors->CopyHits();

    R3BTrackingDetector* target = fDetectors->GetByName("target");
    R3BTrackingDetector* mwpc1 = fDetectors->GetByName("mwpc1");
    R3BTrackingDetector* mwpc2 = fDetectors->GetByName("mwpc2");
    R3BTrackingDetector* mwpc3 = fDetectors->GetByName("mwpc3");
    R3BTrackingDetector* tofw = fDetectors->GetByName("tofw");

    // remember: in this test, target hast no data
    // if (target->hits->GetEntriesFast()==0) return; // no error, can always happen
    if (mwpc1->hits.size() == 0)
        return; // no error, can always happen
    if (mwpc2->hits.size() == 0)
        return; // no error, can always happen
    if (mwpc3->hits.size() == 0)
        return; // no error, can always happen
    if (tofw->hits.size() == 0)
        return; // no error, can always happen

    // fetch start pos, default momentum and charge from the simulation
    // (just for this test!)
    R3BMCTrack* ion = (R3BMCTrack*)fArrayMCTracks->At(0);
    // Check if primary
    if (ion->GetMotherId() != -1)
    {
        // error(); !!!
        // exit(0);
        return;
    }

    Int_t nHits = fhitmwp3->GetEntries();
    Int_t nHitstof = fhittof->GetEntries();

    float zf=0.;
    Int_t nHitstwim = fhittwim->GetEntries();
    for(int i=0;i<nHitstwim;i++){
    R3BHit* twim = (R3BHit*)fhittwim->At(i);
    if(twim->GetTime()>10.)zf=twim->GetTime();
    }


    for(int i=0;i<nHits;i++){
    R3BHit* mw3 = (R3BHit*)fhitmwp3->At(i);
    R3BHit* tof = (R3BHit*)fhittof->At(i);

    

    if(mw3&&tof&&(nHits==nHitstof)){
    

    double xx=243.-mw3->GetX()*cos(29.*TMath::DegToRad());
    double zz =689.+mw3->GetX()*sin(29.*TMath::DegToRad())-163.4;
    v1.SetXYZ(xx,0.,zz);

    double rho=94.6/(2.*sin(v1.Theta()/2.)*cos(14.*TMath::DegToRad()-v1.Theta()/2.));
    double brho=4.0*0.8*rho*0.01;
    double len=65.5+163.4+sqrt(xx*xx+zz*zz)+25.23;
    double vel=len/tof->GetTime();
    double beta=vel/c;
    double gamma=1./sqrt(1.-beta*beta);
    double aa=brho*zf/(3.10716*beta*gamma);

    fh_A_reco2->Fill(aa+0.4);

    std::cout << rho<<" [cm] "<< brho<<" [Tm] "<< len*0.01<<" [m] "<< vel<<" [cm/ns] "<< aa << std::endl;
   
    TLine* l1 = new TLine(0, 163.4, 243.-mw3->GetX()*cos(29.*TMath::DegToRad()),689.+mw3->GetX()*sin(29.*TMath::DegToRad()));
    l1->Draw();
    }

    }
 

    if (0 == (fNEvents % 10))
    {
        LOG(INFO) << "Event: " << fNEvents;
    }

    fNEvents += 1;
    
}

void R3BSofFragmentTracker::Finish()
{

    fh_A_reco2->Write();

 if (fVis)
    {
        for (auto const& det : fDetectors->GetArray())
        {
            det->Draw();
        }
    }

            TLine* l1 = new TLine(0, -65.5, 0, 163.4);
            l1->Draw();


        new TCanvas("c6", "", 500, 300, 500, 500);
        fh_A_reco2->Draw();

}

Bool_t R3BSofFragmentTracker::InitPropagator()
{
    FairField* fairField = FairRunAna::Instance()->GetField();
    R3BGladFieldMap* gladField = NULL;
    fFieldPar->printParams();
    if (2 == fFieldPar->GetType())
    {
        gladField = (R3BGladFieldMap*)fairField;

        if (fPropagator)
        {
            delete fPropagator;
        }
        fPropagator = new R3BTPropagator(gladField, fVis);
    }
    else
    {
        LOG(ERROR) << "Unsupported type of field.";
        return kFALSE;
    }
    return kTRUE;
}

ClassImp(R3BSofFragmentTracker)
