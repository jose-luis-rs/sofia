// ------------------------------------------------------------
// -----                R3BMusOnlineSpectra               -----
// -----    Created 06/10/19  by J.L. Rodriguez-Sanchez   -----
// -----           Fill SOFIA online histograms           -----
// ------------------------------------------------------------

#ifndef R3BMusOnlineSpectra_H
#define R3BMusOnlineSpectra_H

#include "FairTask.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <array>
#include "TMath.h"
#include <cstdlib>
#include "TH1.h"
#include "TH2F.h"
#include "TCanvas.h"

#define NbSectionsMus 1
#define NbAnodesMus 8

class TClonesArray;
class R3BEventHeader;

/**
 * This taks reads MUSIC data and plots online histograms 
 */
class R3BMusOnlineSpectra : public FairTask {

 public:
  /**
   * Default constructor.
   * Creates an instance of the task with default parameters.
   */
  R3BMusOnlineSpectra();

  /**
   * Standard constructor.
   * Creates an instance of the task.
   * @param name a name of the task.
   * @param iVerbose a verbosity level.
   */
  R3BMusOnlineSpectra(const char* name, Int_t iVerbose = 1);

  /**
   * Destructor.
   * Frees the memory used by the object.
   */
  virtual ~R3BMusOnlineSpectra();

  /**
   * Method for task initialization.
   * This function is called by the framework before
   * the event loop.
   * @return Initialization status. kSUCCESS, kERROR or kFATAL.
   */
  virtual InitStatus Init();

  /**
   * Method for event loop implementation.
   * Is called by the framework every time a new event is read.
   * @param option an execution option.
   */
  virtual void Exec(Option_t* option);

  /**
   * A method for finish of processing of an event.
   * Is called by the framework for each event after executing
   * the tasks.
   */
  virtual void FinishEvent();

  /**
   * Method for finish of the task execution.
   * Is called by the framework after processing the event loop.
   */
  virtual void FinishTask();

  /**
   * Methods to clean histograms.
   */
  virtual void Reset_Histo();

  private:

  TClonesArray* fMappedItemsMus;         /**< Array with mapped items. */

  // check for trigger should be done globablly (somewhere else)
  R3BEventHeader* header;               /**< Event header.      */
  Int_t fNEvents;        	   	/**< Event counter.     */

  //Canvas
  TCanvas* cTwimMap_E[NbSectionsMus];
  TCanvas* cTwimMap_EvsDT[NbSectionsMus];
  TCanvas* cTwimMap_DT[NbSectionsMus];
  TCanvas* cTwimMap_ESum, * cTwimMap_ESum1, *cTwimMap_ESum2;

  //Histograms for Mapped data
  TH1F* fh1_twimmap_E[NbSectionsMus][NbAnodesMus];
  TH1F* fh1_twimmap_DT[NbSectionsMus][NbAnodesMus];
  TH1F* fh1_twim_ESum[3];
  TH2F* fh2_twim_ESum;
  TH2F* fh2_twim_EneRawVsDriftTime[NbSectionsMus][NbAnodesMus];

  //Histograms for Cal data


  public:
  ClassDef(R3BMusOnlineSpectra, 1)
};

#endif
