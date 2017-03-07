#if defined(__MAKECINT__) || defined(__MAKECLING__)
#include <AliRsnOutTask.h>
#include <AliRsnOutTaskBin.h>
#include <AliRsnOutTaskFit.h>
#include <AliRsnOutTaskInput.h>
#include <AliRsnOutTaskNorm.h>
#include <TBufferJSON.h>
#include <TFile.h>
#include <TList.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TTask.h>
#endif

TObject *AddBin(Int_t id, Double_t min, Double_t max, TList *norms,
                TList *fits);

void RunRsnOutMgrs(const char *outFileName = "RsnOutMgrResult.root",
                   const char *inFileName = "file://RsnOutMgr.root",
                   TString names = "pt:ctj:ctt") {

  TFile::SetCacheFileDir(gSystem->HomeDirectory(), 1, 1);

  AliRsnOutTaskMgr *tMgr = 0;
  TFile *f = TFile::Open(inFileName, "READ");

  TFile *fOut = TFile::Open(outFileName, "RECREATE");

  TObjArray *a = names.Tokenize(":");
  TObjString *s;
  TString sname;
  for (Int_t i = 0; i < a->GetEntries(); ++i) {
    s = (TObjString *)a->At(i);
    sname = s->GetString();
    tMgr = (AliRsnOutTaskMgr *)f->Get(sname.Data());
    if (tMgr) {
      tMgr->ExecuteTask("");
      fOut->cd();
      tMgr->Write();
    }
  }

  f->Close();

  //	gROOT->GetListOfBrowsables()->Add(tMgr);
  //	new TBrowser;
}

TObject *AddBin(Int_t id, Double_t min, Double_t max, TList *norms,
                TList *fits) {
  AliRsnOutTaskBin *tBin = new AliRsnOutTaskBin();
  tBin->GetValue()->SetId(0);
  tBin->AddCut(new AliRsnOutValue(id, min, max));

  TIter next(norms);
  AliRsnOutValue *vNorm;
  while ((vNorm = (AliRsnOutValue *)next())) {

    AliRsnOutTaskNorm *tNorm = new AliRsnOutTaskNorm();
    tNorm->AddRange(vNorm);
    tBin->Add(tNorm);

    TIter next(fits);
    AliRsnOutValue *vFit;
    while ((vFit = (AliRsnOutValue *)next())) {
      AliRsnOutTaskFit *tFit = new AliRsnOutTaskFit();
      tFit->SetFit(vFit);
      tNorm->Add(tFit);
    }
  }

  return tBin;
}
