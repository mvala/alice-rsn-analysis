#if ROOT_VERSION_CODE > ROOT_VERSION(6,0,0)
R__LOAD_LIBRARY(libRooFit.so)
R__LOAD_LIBRARY(libRsnOut.so)
void rootlogon() {

}
#else
void rootlogon() {
  gSystem->Load("libRHTTP.so");
  gSystem->Load("libRooFit.so");
  gSystem->Load("libRsnOut.so");

}
#endif

