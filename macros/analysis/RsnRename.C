#if defined(__MAKECINT__) || defined(__MAKECLING__)
#include <TFile.h>
#include <THnSparse.h>
#include <TList.h>
#include <TString.h>
#include <TSystem.h>
#endif

void RsnRename(const char *inputFile = "AnalysisResults.root",
               TString outputFile = "/tmp/AnalysisResults.root",
               const char *listname = "RsnOut_tpc3s",
               const char *prefix = "phippData_100_phi_",
               TString axixNames = "im:pt:mult:ctj:ctt") {

  // TFile::SetCacheFileDir(gSystem->HomeDirectory(), 1, 1);

  if (!outputFile.CompareTo(inputFile)) {
    Printf("Error: Input and output file is the same !!!");
    return;
  }

  TFile *fIn = TFile::Open(inputFile);
  if (!fIn)
    return;
  TList *l = (TList *)fIn->Get(listname);
  if (!l)
    return;

  TFile *fOut = TFile::Open(outputFile, "RECREATE");
  if (!fOut)
    return;
  TList *lOut = new TList();
  lOut->SetName(listname);
  lOut->SetOwner();

  TString strListExtra = listname;
  strListExtra += "_extra";

  TList *lOutExtra = new TList();
  lOutExtra->SetName(strListExtra.Data());
  lOutExtra->SetOwner();

  TIter next(l);
  TObject *o;
  while ((o = next())) {

    if (o->InheritsFrom("THnSparse")) {
      THnSparse *s = (THnSparse *)o;
      TString nameStr = s->GetName();
      nameStr.ReplaceAll(prefix, "");
      s->SetName(nameStr.Data());
      TObjArray *tok = axixNames.Tokenize(":");
      TAxis *a;
      for (Int_t i = 0; i < s->GetListOfAxes()->GetEntries(); i++) {
        a = (TAxis *)s->GetListOfAxes()->At(i);
        a->SetName(((TObjString *)tok->At(i))->GetString().Data());
      }
      o->Print();
      lOut->Add(o);
    } else {
      lOutExtra->Add(o);
    }
  }

  lOut->Write(listname, TObject::kSingleKey);
  lOutExtra->Write(strListExtra.Data(), TObject::kSingleKey);
  fOut->Close();
  fIn->Close();
}
