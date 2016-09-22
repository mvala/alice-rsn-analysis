#include "AliRsnOutValue.h"

ClassImp(AliRsnOutValue)

  AliRsnOutValue::AliRsnOutValue(Int_t id, Double_t min, Double_t max)
  : TObject(), fId(id), fMin(min), fMax(max) {}

AliRsnOutValue::~AliRsnOutValue() {}

void AliRsnOutValue::SetAll(Int_t id, Double_t min, Double_t max) {
  fId = id;
  fMin = min;
  fMax = max;
}
