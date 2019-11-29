#ifndef R3BSOFSCISINGLETCALITEM_H
#define R3BSOFSCISINGLETCALITEM_H

#include "TObject.h"

#define NUMBER_OF_DETECTORS 1

class R3BSofSciSingleTcalData : public TObject
{
  public:
    // Default Constructor
    R3BSofSciSingleTcalData();

    // Standard Constructor
    R3BSofSciSingleTcalData(UShort_t detector);

    // Destructor
    virtual ~R3BSofSciSingleTcalData() {}

    // Getters
    inline const UShort_t& GetNumberOfDetectors()           const { return fNumberOfDetectors; }
    inline const Double_t& GetRawTimeNs(UShort_t det)       const { return fRawTimeNs[det-1]; }
    inline const Double_t& GetRawPosNs(UShort_t det)        const { return fRawPosNs[det-1]; }
#if NUMBER_OF_DETECTORS==2
    inline const Double_t& GetRawTofNs()                    const { return fRawTofNs;}
#endif

    // Modifiers
    void SetNumberofDetectors(UShort_t nDets)       {fNumberOfDetectors=nDets;}
    void SetRawTimeNs(UShort_t det, Double_t time)  {fRawTimeNs[det-1]=time;}
    void SetRawPosNs(UShort_t det, Double_t pos)    {fRawPosNs[det-1]=pos;}
#if NUMBER_OF_DETECTORS==2
    void SetRawTofNs(Double_t tof)                  {fRawTofNs=tof;}
#endif

  private:
    UShort_t fNumberOfDetectors; 
    Double_t fRawTimeNs[NUMBER_OF_DETECTORS];           // 0.5*(Tleft + Traw)
    Double_t fRawPosNs[NUMBER_OF_DETECTORS];            // Tleft - Traw
#if NUMBER_OF_DETECTORS==2
    Double_t fRawTofNs;
#endif
  public:
    ClassDef(R3BSofSciSingleTcalData, 2)
};

#endif
