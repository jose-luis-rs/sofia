#ifndef __R3BSOFSCITCAL2SINGLETCALPAR_H__
#define __R3BSOFSCITCAL2SINGLETCALPAR_H__

#include "FairTask.h"
#include "TH1F.h"

class TClonesArray;
class R3BSofSingleTcalPar;
class R3BEventHeader;

class R3BSofSciTcal2SingleTcalPar : public FairTask {
  
 public:
  
  /** Default constructor **/
  R3BSofSciTcal2SingleTcalPar();
  
  /** Standard constructor **/
  R3BSofSciTcal2SingleTcalPar(const char* name, Int_t iVerbose=1);
  
  /** Destructor **/
  virtual ~R3BSofSciTcal2SingleTcalPar();
  
  /** Virtual method Init **/
  virtual InitStatus Init();
  
  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  
  /** Virtual method FinishEvent **/
  virtual void FinishEvent();
  
  /** Virtual method FinishTask **/
  virtual void FinishTask(); 
  
  /** Virtual method Reset **/
  virtual void Reset();
  
  /** Virtual method ReInit **/
  virtual InitStatus ReInit();
  
  /** Virtual method calculate the Vftx Tcal Parameters **/
  virtual void CalculateRawPosSingleTcalParams();
  
  void SetOutputFile(const char *outFile);
  
  /** Accessor functions **/
  const Double_t GetNumDetectors() {return fNumDetectors;}
  const Double_t GetNumSignals() {return fNumSignals;}
  const Int_t GetMinStatistics(){return fMinStatistics;}

  void SetNumDetectors(Int_t NumberOfDetectors)   {
    fNumDetectors=NumberOfDetectors;
  }  
  void SetNumSignals(Int_t NumberOfDetectors)     {
    fNumSignals=NumberOfDetectors;  
  }
  void SetNumSingleTcalParsPerSignal(Int_t NumberOfSingleTcalParsPerSignal) {
    fNumSingleTcalParsPerSignal=NumberOfSingleTcalParsPerSignal;
  }
  void SetMinStatistics(Int_t minstat){
    fMinStatistics=minstat;
  }



 protected:
  Int_t fNumDetectors;  // number of detectors 
  Int_t fNumSignals;    // number of signal    
  Int_t fNumSingleTcalParsPerSignal; // =2 for each signal
  Int_t fMinStatistics; // minimum statistics to proceed to the calibration

  // calibration parameters
  R3BSofSingleTcalPar* fSingleTcalPar;  

  // input data
  TClonesArray* fTcal;         
   
  // histograms
  TH1D** fh_RawPosMult1;
  char* fOutputFile;
  
 public:
  ClassDef(R3BSofSciTcal2SingleTcalPar,0);
};

#endif //__R3BSOFSCITCAL2SINGLETCALPAR_H__
