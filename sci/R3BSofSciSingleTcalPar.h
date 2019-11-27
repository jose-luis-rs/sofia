#ifndef __R3BSOFSCISINGLETCALPAR_H__
#define __R3BSOFSCISINGLETCALPAR_H__

#include "FairParGenericSet.h" // for FairParGenericSet

#include "TArrayF.h"
#include "TObjArray.h"
#include "TObject.h"
#include <TObjString.h>

class FairParamList;

class R3BSofSciSingleTcalPar : public FairParGenericSet
{
  
 public:
  /** Standard constructor **/
  R3BSofSciSingleTcalPar(const char* name = "SofSciSingleTcalPar",
			 const char* title = "Sof Sci Parameters",
			 const char* context = "SofSciParContext");
  
  /** Destructor **/
  virtual ~R3BSofSciSingleTcalPar();
  
  /** Method to reset all parameters **/
  virtual void clear();
  
  /** Method to store all parameters using FairRuntimeDB **/
  virtual void putParams(FairParamList* list);
  
  /** Method to retrieve all parameters using FairRuntimeDB**/
  Bool_t getParams(FairParamList* list);
  
  /** Method to print values of parameters to the standard output **/
  void printParams();
  
  /** Accessor functions **/
  const Double_t GetNumDetectors() { return fNumDetectors; }
  const Double_t GetNumSignals() { return fNumSignals; }
  const Double_t GetNumParsPerSignal() { return fNumParsPerSignal; }
  TArrayF* GetAllSignalsTcalParams() { return fAllSignalsSingleTcalParams; }
  Double_t GetSignalTcalParams(UInt_t rank) { return (Double_t)fAllSignalsSingleTcalParams->GetAt(rank); }
  
  void SetNumDetectors(Int_t NumberOfDetectors) { fNumDetectors = NumberOfDetectors; }
  void SetNumSignals(Int_t NumberOfSignals)     { fNumSignals = NumberOfSignals ;  }
  void SetNumParsPerSignal(Int_t NumberOfParsPerSignal) { fNumParsPerSignal = NumberOfParsPerSignal; }
  void SetSignalParams(Double_t limit, UInt_t rank) { fAllSignalsSingleTcalParams->AddAt(limit, rank); }
  
  /** Create more Methods if you need them! **/
  
 private:
  TArrayF* fAllSignalsSingleTcalParams; // Calibration Parameters for all signals of one detector
  Int_t fNumDetectors;            // number of detectors 
  Int_t fNumSignals;              // =fNumDetectors if RawPos , but =1 if RawTof
  Int_t fNumParsPerSignal;
  const R3BSofSciSingleTcalPar& operator=(const R3BSofSciSingleTcalPar&); /*< an assignment operator>*/
  
  R3BSofSciSingleTcalPar(const R3BSofSciSingleTcalPar&); // a copy constructor
  
  ClassDef(R3BSofSciSingleTcalPar, 1);
};

#endif //__R3BSOFTCALPAR_H__
