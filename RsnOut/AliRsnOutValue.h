#ifndef AliRsnOutValue_cxx
#define AliRsnOutValue_cxx

#include <TObject.h>

class AliRsnOutValue : public TObject {

public:

  AliRsnOutValue(Int_t id=0, Double_t min=0.0, Double_t max=1.0);
  virtual ~AliRsnOutValue();

  void SetAll(Int_t id=0, Double_t min=0.0, Double_t max=1.0);
  void SetId(Int_t id) { fId = id; }
  void SetMin(Double_t min) { fMin = min; }
  void SetMax(Double_t max) { fMax = max; }

  Int_t GetId() const { return fId; }
  Double_t GetMin() const { return fMin; }
  Double_t GetMax() const { return fMax; }

private:

  Int_t fId;
  Double_t fMin;
  Double_t fMax;

  ClassDef(AliRsnOutValue, 1)

};

#endif

