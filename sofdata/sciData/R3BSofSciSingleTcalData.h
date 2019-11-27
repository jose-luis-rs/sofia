#ifndef R3BSOFSCISINGLETCALITEM_H
#define R3BSOFSCISINGLETCALITEM_H

#include "TObject.h"

class R3BSofSciSingleTcalData : public TObject
{
  public:
    // Default Constructor
    R3BSofSciSingleTcalData();

    // Standard Constructor
    R3BSofSciSingleTcalData(UShort_t detector, Double_t left_traw, Double_t right_traw, Double_t mean_traw, Double_t posraw);

    // Destructor
    virtual ~R3BSofSciSingleTcalData() {}

    // Getters
    inline const UShort_t& GetDetector()           const { return fDetector; }
    inline const Double_t& GetPmtLeftRawTimeNs()   const { return fPmtLeftRawTimeNs; }
    inline const Double_t& GetPmtRightRawTimeNs()  const { return fPmtRightRawTimeNs; }
    inline const Double_t& GetRawTimeNs()          const { return fRawTimeNs; }
    inline const Double_t& GetRawPosNs()           const { return fRawPosNs; }

  private:
    UShort_t fDetector; 
    Double_t fPmtLeftRawTimeNs;
    Double_t fPmtRightRawTimeNs;
    Double_t fRawTimeNs;           // 0.5*(Tleft + Traw)
    Double_t fRawPosNs;            // Tleft - Traw

  public:
    ClassDef(R3BSofSciSingleTcalData, 2)
};

#endif
