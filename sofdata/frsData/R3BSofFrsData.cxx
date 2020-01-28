// ---------------------------------------------------------------------------
// -----                                                                 -----
// -----                      R3BSofFrsData                              -----
// -----                  Created 28/01/2020 by J.L. Rodriguez           -----
// -----                                                                 -----
// ---------------------------------------------------------------------------

#include "R3BSofFrsData.h"

R3BSofFrsData::R3BSofFrsData()
    : fZ(0)
    , fAq(0)
{
}

//------------------------------

R3BSofFrsData::R3BSofFrsData(Double_t z, Double_t aq)
    : fZ(z)
    , fAq(aq)
{
}

ClassImp(R3BSofFrsData)
