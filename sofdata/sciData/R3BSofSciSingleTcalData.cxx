#include "R3BSofSciSingleTcalData.h"

R3BSofSciSingleTcalData::R3BSofSciSingleTcalData()
  : fNumberOfDetectors(NUMBER_OF_DETECTORS)
{
}

R3BSofSciSingleTcalData::R3BSofSciSingleTcalData(UShort_t ndets) 
  : fNumberOfDetectors(ndets)
{
}

ClassImp(R3BSofSciSingleTcalData)
