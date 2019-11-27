// *** *************************************************************** *** //
// ***                  R3BSofSciTcal2SingleHit                        *** //
// *** ---> whatever multiplicity per PMT, select the propoer hit      *** //
// *** *************************************************************** *** //

#ifndef R3BSOFSCI_TCAL2SINGLETCAL
#define R3BSOFSCI_TCAL2SINGLETCAL

#include "FairTask.h"

#include "R3BSofTcalPar.h"
#include "R3BSofSciTcalData.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"
class TRandom3;

class R3BSofSciTcal2SingleTcal : public FairTask
{

 public:
  // --- Default constructor --- //
  R3BSofSciTcal2SingleTcal();
  
  // --- Standard constructor --- //
  R3BSofSciTcal2SingleTcal(const char* name, Int_t iVerbose=1);

  // --- Destructor --- // 
  virtual ~R3BSofSciTcal2SingleTcal();

  virtual InitStatus Init();         
  virtual void SetParContainers();
  virtual InitStatus ReInit();
  virtual void Exec(Option_t* option);
  virtual void FinishEvent();
  virtual void FinishTask();
  Double_t HitFinder(UShort_t det, UShort_t pmtl, UShort_t pmtr, UInt_t tlns, UInt_t trns);

 private:
  TClonesArray*  fTcalPmt;            
  R3BSofTcalPar* fPosPar;          
  TClonesArray*  fSingleTcal;         
  
  UInt_t fNumTcalItems;               // number of Tcal items per event

  UInt_t fNevent;

  TRandom rand;

 public:
  ClassDef(R3BSofSciTcal2SingleTcal, 1)

};

#endif  // R3BSOFSCI_TCAL2SINGLETCAL
