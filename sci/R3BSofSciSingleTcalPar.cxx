#include "R3BSofSciSingleTcalPar.h"

#include "FairLogger.h"
#include "FairParamList.h"

#include "TArrayF.h"
#include "TMath.h"
#include "TString.h"

#include <iostream>

#define MAX_SINGLETCALPAR 4

using std::cout;
using std::endl;

// ---- Standard Constructor ---------------------------------------------------
R3BSofSciSingleTcalPar::R3BSofSciSingleTcalPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name, title, context)
  , fNumDetectors(0)
  , fNumSignals(0)
  , fNumTcalParsPerSignal(0)
{
  fAllSignalsSingleTcalParams = new TArrayF(MAX_SINGLETCALPAR);
}

// ----  Destructor ------------------------------------------------------------
R3BSofSciSingleTcalPar::~R3BSofSciSingleTcalPar()
{
    clear();
    if (fAllSignalsSingleTcalParams)
    {
        delete fAllSignalsSingleTcalParams;
        fAllSignalsSingleTcalParams == NULL;
    }
}

// ----  Method clear ----------------------------------------------------------
void R3BSofSciSingleTcalPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BSofSciSingleTcalPar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BSofSciSingleTcalPar::putParams() called";
    if (!list)
    {
        return;
    }

    Int_t array_size = fNumSignals * fNumTcalParsPerSignal;
    LOG(INFO) << "Array Size: " << array_size;

    fAllSignalsSingleTcalParams->Set(array_size);

    list->add("SingleTcalPar", *fAllSignalsSingleTcalParams);
    list->add("nDetectorsSingleTcalPar", fNumDetectors);
    list->add("nSignalsSingleTcalPar", fNumSignals);
    list->add("nSingleTcalParsPerSignal", fNumSingleTcalParsPerSignal);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BSofSciSingleTcalPar::getParams(FairParamList* list)
{
    LOG(INFO) << "R3BSofSciSingleTcalPar::getParams() called";
    if (!list)
    {
        return kFALSE;
    }
    if (!list->fill("nDetectorsSingleTcalPar", &fNumDetectors))
    {
        return kFALSE;
    }
    if (!list->fill("nSignalsSingleTcalPar", &fNumSignals))
    {
        return kFALSE;
    }
    if (!list->fill("nSingleTcalParsPerSignal", &fNumSingleTcalParsPerSignal))
    {
        return kFALSE;
    }

    Int_t array_size = fNumSignals * fNumTcalParsPerSignal;
    LOG(INFO) << "Array Size: " << array_size;
    fAllSignalsSingleTcalParams->Set(array_size);

    if (!(list->fill("SingleTcalPar", fAllSignalsSingleTcalParams)))
    {
        LOG(INFO) << "---Could not initialize fAllSignalsSingleTcalParams";
        return kFALSE;
    }

    return kTRUE;
}

// ----  Method printParams ----------------------------------------------------
void R3BSofSciSingleTcalPar::printParams()
{
    LOG(INFO) << "R3BSofSciSingleTcalPar: SofSciSingleTcal Parameters: ";
    Int_t array_size = fNumSignals * fNumTcalParsPerSignal;

    cout << "--- --------------------------------------------" << endl;
    cout << "--- Single Tcal Parameters :  " << endl;
    cout << "--- --------------------------------------------" << endl;
    for (Int_t param=0; param < array_size; sig++)
      {
	cout << "LIMIT " << bin << " = " << fAllSignalsSingleTcalParams->GetAt(param) << endl;
      }
}
