#if defined(__MAKECINT__) || defined(__MAKECLING__)
#include <AliRsnOutTask.h>
#include <AliRsnOutTaskBin.h>
#include <AliRsnOutTaskInput.h>
#include <TBufferJSON.h>
#include <TFile.h>
#include <TList.h>
#include <TObjArray.h>
#include <TROOT.h>
#include <TString.h>
#include <TSystem.h>
#include <TTask.h>
#endif

void RunRsnOut(TString config = "RsnOutMgrNew.root", TString outPostfix="Results", Bool_t useLocalCache=kTRUE) {

  TString outFileName = config;
  outFileName.ReplaceAll(".root",TString::Format("%s.root", outPostfix.Data()).Data());

  TFile *f = TFile::Open(config, "RECREATE");

  AliRsnOutTaskMgr *tMgr = new AliRsnOutTaskMgr("mgr");

  AliRsnOutTaskInput *tInputMC = new AliRsnOutTaskInput("LHC15g3a3");
  tInputMC->SetFileName("root://alieos.saske.sk///eos/alike.saske.sk/alice/"
                        "alike/PWGLF/LF_pp_MC/376_20160308-1500/"
                        "merge_runlist_X/AnalysisResults.root");
  tInputMC->SetListName("RsnOut_tpc3s");
  tInputMC->SetSigBgName("Unlike");
  tInputMC->SetBgName("Mixing");
  tInputMC->SetMCRecName("Trues");
  tInputMC->SetMCGenName("Mother");
  tInputMC->SetEfficiencyOnly();
  tMgr->Add(tInputMC);


  AliRsnOutTaskInput *tInputData = new AliRsnOutTaskInput("LHC15f");
  tInputData->SetFileName("root://alieos.saske.sk///eos/alike.saske.sk/alice/"
                          "alike/PWGLF/LF_pp/389_20160307-1141/merge_runlist_4/"
                          "AnalysisResults.root");
  tInputData->SetListName("RsnOut_tpc3s");
  tInputData->SetSigBgName("Unlike");
  tInputData->SetBgName("Mixing");
  tMgr->Add(tInputData);

  TList *listVariations = new TList();
  TArrayD *arrPt = new TArrayD(3);
  arrPt->SetAt(6,0);
  arrPt->SetAt(11,1);
  arrPt->SetAt(16,2);
  listVariations->Add(new AliRsnOutValue(1,arrPt));
  
  TArrayD *arrMult = new TArrayD(3);
  arrMult->SetAt(21,0);
  arrMult->SetAt(31,1);
  arrMult->SetAt(51,2);
  listVariations->Add(new AliRsnOutValue(2,arrMult));

  TList *norms = new TList();
  norms->Add(new AliRsnOutValue(0, 1.10, 1.15));
  // norms->Add(new AliRsnOutValue(0, 1.11, 1.15));

  TList *fits = new TList();
  Int_t nFits = 3;
  for (Int_t fitId = 0; fitId < nFits; fitId++)
  {
    fits->Add(new AliRsnOutValue(fitId, 0.997, 1.050));
    // fits->Add(new AliRsnOutValue(fitId, 1.000, 1.080));
    // fits->Add(new AliRsnOutValue(fitId, 0.997, 1.130));
  }


  AliRsnOutTaskBinMgr *binMgr = new AliRsnOutTaskBinMgr("binMgr");
  binMgr->GenerateBinTemplate(norms, fits);
  binMgr->SetListOfVartiations(listVariations);
  binMgr->Init();
  
  tInputMC->Add((AliRsnOutTaskBinMgr *)binMgr->Clone());
  tInputData->Add((AliRsnOutTaskBinMgr *)binMgr->Clone());

  // // return;
  // // TMP
  // gROOT->GetListOfBrowsables()->Add(tMgr);
  // new TBrowser;
  // return;

  if (tMgr)
    tMgr->Write();
  f->Close();

  f = TFile::Open(config,"READ");
  tMgr = (AliRsnOutTaskMgr*) f->Get("mgr");

  if (!outPostfix.IsNull()) {
    if (useLocalCache)
      TFile::SetCacheFileDir(gSystem->HomeDirectory(), 1, 1);
    
    tMgr->ExecuteTask("");
    TFile *fOut = TFile::Open(outFileName.Data(), "RECREATE");
    if (tMgr) {
      fOut->cd();
      tMgr->Write();
    }
    fOut->Close();
  } else {
    gROOT->GetListOfBrowsables()->Add(tMgr);
    new TBrowser;
  }

  f->Close();
}
