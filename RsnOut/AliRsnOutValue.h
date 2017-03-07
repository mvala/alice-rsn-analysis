#ifndef AliRsnOutValue_cxx
#define AliRsnOutValue_cxx

#include <TObject.h>
#include <TArrayD.h>

class AliRsnOutValue : public TObject {

public:
  AliRsnOutValue(Int_t id = 0, Double_t min = 0.0, Double_t max = 1.0);
  AliRsnOutValue(Int_t id, TArrayD *arr);
  virtual ~AliRsnOutValue();

  void SetAll(Int_t id = 0, Double_t min = 0.0, Double_t max = 1.0);
  void SetId(Int_t id) { fId = id; }
  void SetMin(Double_t min) { fMin = min; }
  void SetMax(Double_t max) { fMax = max; }
  void SetArray(TArrayD *arr) { fArray = arr; }  

  Int_t GetId() const { return fId; }
  Double_t GetMin() const { return fMin; }
  Double_t GetMax() const { return fMax; }
  TArrayD *GetArray() const { return fArray; }

private:
  Int_t fId;
  Double_t fMin;
  Double_t fMax;
  TArrayD *fArray;

  ClassDef(AliRsnOutValue, 1)
};

#endif
