#include "AliRsnOutValue.h"

ClassImp(AliRsnOutValue)

AliRsnOutValue::AliRsnOutValue(Int_t id, Double_t min, Double_t max)
  : TObject(), fId(id), fMin(min), fMax(max), fArray(0) {}

AliRsnOutValue::AliRsnOutValue(Int_t id, TArrayD *arr)
  : TObject(), fId(id), fMin(0.0), fMax(0.0), fArray(arr) {}
AliRsnOutValue::~AliRsnOutValue() {}

void AliRsnOutValue::SetAll(Int_t id, Double_t min, Double_t max) {
  fId = id;
  fMin = min;
  fMax = max;
}
