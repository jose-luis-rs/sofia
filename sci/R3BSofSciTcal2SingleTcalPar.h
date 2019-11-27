#ifndef __R3BSOFSCITCAL2SINGLETCALPAR_H__
#define __R3BSOFSCITCAL2SINGLETCALPAR_H__

#include "FairTask.h"
#include "TH1F.h"

class TClonesArray;
class R3BSofSingleTcalPar;
class R3BEventHeader;


// for the engineering run (fNumDetectors(1) instead of fNumDetectors(2)
#define NUMBER_OF_DETECTORS 1 
#define NUMBER_OF_CHANNELS 3 // 2PMT + Tref


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
  

  /** Virtual method called by Exec when HitFinder done with PosRaw  ***/
  virtual void ExecRawPos();

  /** Virtual method called by Exec when HitFinder done with TofRaw (S2-CaveC) ***/
  /*** TO DO WHEN SCINTILLATOR AT S2 ***/
  virtual void ExecRawTof();

  
  /** Virtual method calculate the PosRaw Single Tcal Parameters **/
  virtual void CalculateRawPosSingleTcalParams();
  
  /** Virtual method calculate the TofRaw Single Tcal Parameters **/
  /*** TO DO WHEN SCINTILLATOR AT S2 ***/
  virtual void CalculateRawTofSingleTcalParams();
  
  void SetOutputFile(const char *outFile);
  
  /** Accessor functions **/
  const Double_t GetNumDetectors() {return fNumDetectors;}
  const Double_t GetNumSignals() {return fNumSignals;}
  const Int_t GetMinStatistics(){return fMinStatistics;}

  void SetNumDetectors(Int_t NumberOfDetectors)   {
    fNumDetectors=NumberOfDetectors;
  }  
  void SetNumSignals(Int_t NumberOfSignals)     {
    // if RawPos : fNumSignals = fNumDetectors
    // if RawTof : fNumSignals = 1 
    fNumSignals=NumberOfSignals;  
  }
  void SetNumSingleTcalParsPerSignal(Int_t NumberOfSingleTcalParsPerSignal) {
    fNumSingleTcalParsPerSignal=NumberOfSingleTcalParsPerSignal;
  }
  void SetMinStatistics(Int_t minstat){
    fMinStatistics=minstat;
  }



 protected:
  Int_t fNumDetectors;  // number of detectors 2 if FRS, 1 if Cave C only
  Int_t fNumSignals;    // number of signal    1 if RawTof used, fNumDetectors if RawPos used
  Int_t fNumSingleTcalParsPerSignal; // =2 for each signal
  Int_t fMinStatistics; // minimum statistics to proceed to the calibration

  // calibration parameters
  R3BSofSingleTcalPar* fSingleTcalPar;  

  // input data
  TClonesArray* fTcal;         
   
  // histograms
  TH1D** fh_RawPosMult1;
#if NUMBER_OF_DETECTORS==2
  TH1D* fh_RawTofMult1;
#endif
  char* fOutputFile;
  
 public:
  ClassDef(R3BSofSciTcal2SingleTcalPar,0);
};

#endif //__R3BSOFSCITCAL2SINGLETCALPAR_H__
