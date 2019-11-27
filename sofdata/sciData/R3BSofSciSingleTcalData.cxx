#include "R3BSofSciSingleTcalData.h"

R3BSofSciSingleTcalData::R3BSofSciSingleTcalData()
    : fDetector(0)
    , fPmtLeftRawTimeNs(0)
    , fPmtRightRawTimeNs(0)
    , fRawTimeNs(0)
    , fRawPosNs(0)
{
}

R3BSofSciSingleTcalData::R3BSofSciSingleTcalData(UShort_t detector, 
						 Double_t tl, 
						 Double_t tr
						 Double_t t,
						 Double_t pos)
    : fDetector(detector)
    , fPmtLeftRawTimeNs(tl)
    , fPmtRightRawTimeNs(tr)
    , fRawTimeNs(t)
    , fRawPosNs(pos)
{
}
ClassImp(R3BSofSciSingleTcalData)
