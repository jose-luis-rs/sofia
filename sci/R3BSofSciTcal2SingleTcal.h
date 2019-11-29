// *** *************************************************************** *** //
// ***                  R3BSofSciTcal2SingleTcal                        *** //
// *** ---> whatever multiplicity per PMT, select the propoer hit      *** //
// *** *************************************************************** *** //

#ifndef R3BSOFSCI_TCAL2SINGLETCAL
#define R3BSOFSCI_TCAL2SINGLETCAL

#include "FairTask.h"

#include "R3BSofSingleTcalPar.h"
#include "R3BSofSciSingleTcalData.h"

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
  virtual void ExecRawPos();
  //virtual void ExecRawTof(); // need to be done when SofSci at S2
  virtual void FinishEvent();
  virtual void FinishTask();
  Double_t CalculateRawPosNs(UInt_t tlns, UInt_t trns);
  Double_t CalculateRawTimeNs(UInt_t tlns, UInt_t trns);

 private:
  TClonesArray*  fTcal;            
  R3BSofTcalPar* fRawPosPar;          
  TClonesArray*  fSingleTcal;         
  
  UInt_t fNumSingleTcal;     

  UInt_t fNevent;

  TRandom rand;

 public:
  ClassDef(R3BSofSciTcal2SingleTcal, 1)

};

#endif  // R3BSOFSCI_TCAL2SINGLETCAL
