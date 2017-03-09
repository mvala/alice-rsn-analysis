#if defined(__MAKECINT__) || defined(__MAKECLING__)
#include <AliRsnOutTaskMgr.h>
#include <TFile.h>
#include <TKey.h>
#endif

void ExportTasks(const char *filename = "file://RsnOutMgrResult.root",
                 const char *filenameOut = "RsnOutFolder.root") {

  TFile::SetCacheFileDir(gSystem->HomeDirectory(), 1, 1);

  TFile *f = TFile::Open(filename, "READ");
  if (!f)
    return;

  TFile *fOut = TFile::Open(filenameOut, "RECREATE");
  TList *keys = f->GetListOfKeys();
  TKey *key;
  AliRsnOutTaskMgr *tMgr;
  for (Int_t i = 0; i < keys->GetEntries(); ++i) {
    key = (TKey *)keys->At(i);
    Printf("%d %s", i, key->GetName());
    tMgr = (AliRsnOutTaskMgr *)f->Get(key->GetName());
    tMgr->Export(fOut);
  }
  fOut->Close();
  f->Close();
}
