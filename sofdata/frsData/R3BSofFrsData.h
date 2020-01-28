// ---------------------------------------------------------------------------
// -----                                                                 -----
// -----                      R3BSofFrsData                              -----
// -----                  Created 28/01/2020 by J.L. Rodriguez           -----
// -----                                                                 -----
// ---------------------------------------------------------------------------

#ifndef R3BSofFrsData_H
#define R3BSofFrsData_H
#include "TObject.h"

class R3BSofFrsData : public TObject
{

  public:
    // Default Constructor
    R3BSofFrsData();

    /** Standard Constructor
     *@param fZ      Z of fragments
     *@param fAq     A/q of fragments
     **/
    R3BSofFrsData(Double_t z, Double_t aq);

    // Destructor
    virtual ~R3BSofFrsData() {}

    // Getters
    inline const Double_t GetZ() const { return fZ; }
    inline const Double_t GetAq() const { return fAq; }

  protected:
    Double_t fZ, fAq;    // ID

  public:
    ClassDef(R3BSofFrsData, 1)
};

#endif
