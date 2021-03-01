#if defined(__MAKECINT__) || defined(__MAKECLING__)
#include <TFile.h>
#include <TFolder.h>
#include <TIter.h>
#endif

void convert2ndmvr(const char *outFileName = "out.root",
                   const char *inFileName = "test-results-final.root",
                   const char *folder = "test",
                   const char *base = "mgr/DATA/binMgr/pt_vs_mult",
                   const char *postfix = "norm[1.10,1.15]/fit_0[0.997,1.050]",
                   const char *object = "hSignal") {
  TFile *file = TFile::Open(inFileName);
  if (!file)
    return;
  TFolder *root = (TFolder *)file->Get("test");

  TFolder *ff = (TFolder *)root->FindObject(base);

  if (!ff)
    return;

  TH2D *hh = (TH2D *)ff->FindObject("eff_histo");
  if (!hh) {
    Printf("Histogram 'eff_histo' was not found !!!");
    return;
  }
  hh->SetNameTitle("main", "Main histogram");
  hh->SetDrawOption("colz");

  TFile *fOut = TFile::Open(outFileName, "RECREATE");
  if (!fOut)
    return;

  TFolder *rootOut = new TFolder("root", "Main root folder");
  TFolder *folderCurrentI,*folderCurrentJ;

  Int_t i = 0, j = 0;
  TIter next(ff->GetListOfFolders());
  while (TFolder *obj = (TFolder *)next()) {

    if (!obj->IsFolder())
      continue;
    i++;
    folderCurrentI = rootOut->AddFolder(TString::Format("%d", i).Data(),"");
    // obj->Print();
    j = 0;
    TIter next2(obj->GetListOfFolders());
    while (TFolder *obj2 = (TFolder *)next2()) {

      if (!obj2->IsFolder())
        continue;
      j++;
      // obj2->Print();
      TString n = TString::Format("%s/%s/%s/%s/%s", base, obj->GetName(),
                                  obj2->GetName(), postfix, object);

      TH1 *h = (TH1 *)root->FindObject(n.Data());
      if (!h)
        continue;
      Printf("%s [%d,%d]", n.Data(), i, j);
      // h->Print();
        
      folderCurrentJ = folderCurrentI->AddFolder(TString::Format("%d", j).Data(),"");
      folderCurrentJ->Add(h);
      hh->SetBinContent(i,j,1);
    }
  }

  rootOut->Write();
  hh->Write();
  fOut->Close();
}