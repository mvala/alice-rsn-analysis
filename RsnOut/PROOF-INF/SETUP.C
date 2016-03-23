R__LOAD_LIBRARY(libRsnOut.so)
Int_t SETUP() {
  if (!TClass::GetClass("AliRsnOutTask")) {
    return 1;
  }
  return 0;
}
