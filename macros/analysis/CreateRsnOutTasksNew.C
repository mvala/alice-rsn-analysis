#if defined(__MAKECINT__) || defined(__MAKECLING__)
#include <AliRsnOutTask.h>
#include <AliRsnOutTaskBin.h>
#include <AliRsnOutTaskFit.h>
#include <AliRsnOutTaskInput.h>
#include <AliRsnOutTaskNorm.h>
#include <TBufferJSON.h>
#include <TFile.h>
#include <TList.h>
#include <TObjArray.h>
#include <TROOT.h>
#include <TString.h>
#include <TSystem.h>
#include <TTask.h>
#endif

AliRsnOutTaskBin *AddBin(Int_t id, Double_t min, Double_t max, TList *norms,
                         TList *fits);

void CreateRsnOutTasksNew(const char*config = "RsnOutMgrNew.root", const char* outFileName="RsnOutMgrNewResults.root")
{

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
  binMgr->SetBinTemplate(GenerateBinTemplate(norms, fits));
  binMgr->SetListOfVartiations(listVariations);


  binMgr->Init();
  
  tInputMC->Add((AliRsnOutTaskBinMgr *)binMgr->Clone());
  tInputData->Add((AliRsnOutTaskBinMgr *)binMgr->Clone());



  if (tMgr)
    tMgr->Write();

  f->Close();

  f = TFile::Open(config,"READ");
  TFile *fOut = TFile::Open(outFileName, "RECREATE");


  tMgr = (AliRsnOutTaskMgr*) f->Get("mgr");
  tMgr->ExecuteTask("");
    
  if (tMgr) {
    fOut->cd();
    tMgr->Write();
  }

  fOut->Close();

  f->Close();

  // gROOT->GetListOfBrowsables()->Add(tMgr);
  // new TBrowser;


}

AliRsnOutTaskBin *GenerateBinTemplate(TList *norms,TList *fits) {
  AliRsnOutTaskBin *tBin = new AliRsnOutTaskBin();
  // tBin->GetValue()->SetId(0);
  // tBin->AddCut(new AliRsnOutValue(id, min, max));

  // Loop over norms
  TIter nextNorm(norms);
  AliRsnOutValue *vNorm;
  while ((vNorm = (AliRsnOutValue *)nextNorm()))
  {
    AliRsnOutTaskNorm *tNorm = new AliRsnOutTaskNorm();
    tNorm->AddRange(vNorm);
    tBin->Add(tNorm);

    // Loop over fits
    TIter nextFit(fits);
    AliRsnOutValue *vFit;
    while ((vFit = (AliRsnOutValue *)nextFit()))
    {
      AliRsnOutTaskFit *tFit = new AliRsnOutTaskFit();
      tFit->SetFit(vFit);
      tNorm->Add(tFit);
    }
  }
  return tBin;
}












AliRsnOutTaskMgr *CreateRsnOutMgr(TString name, Int_t cutAxis = 1,
                                  Int_t binStart = 6, Int_t binEnd = 50,
                                  Int_t binStep = 1)
{

  AliRsnOutTaskMgr *tMgr = new AliRsnOutTaskMgr(name);



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

  // const Int_t multNBins = 11;
  // Int_t multBins[multNBins] = {1, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
  const Int_t multNBins = 3;
  Int_t multBins[multNBins] = {1, 50, 100};

  AliRsnOutTaskBin *tBin;

  for (Int_t iMult = 0; iMult < multNBins-1; iMult++)
  {
    AliRsnOutTaskBin *tBinCutOnly = new AliRsnOutTaskBin("bin", "", kTRUE);
    tBinCutOnly->GetValue()->SetId(0);
    tBinCutOnly->AddCut(new AliRsnOutValue(2, multBins[iMult] + 1, multBins[iMult + 1]));
    AliRsnOutTaskBin *tBinCutOnlyMC = (AliRsnOutTaskBin *)tBinCutOnly->Clone();

    for (Int_t i = binStart; i <= binEnd; i += binStep)
    {
      tBin = AddBin(cutAxis, i, i + binStep - 1, norms, fits);
      tBinCutOnly->Add(tBin);
      tBin = AddBin(cutAxis, i, i + binStep - 1, norms, fits);
      tBinCutOnlyMC->Add(tBin);
    }
    tInputData->Add(tBinCutOnly);
    tInputMC->Add(tBinCutOnlyMC);
  }

  AliRsnOutTask *tResultsAll = new AliRsnOutTask("results", "Results");

  AliRsnOutTaskResult *tResult = new AliRsnOutTaskResult();
  tResult->SetData(tInputData);
  tResult->SetMC(tInputMC);

  tResultsAll->Add(tResult);
  tMgr->Add(tResultsAll);

  return tMgr;
}

AliRsnOutTaskBin *AddBin(Int_t id, Double_t min, Double_t max, TList *norms,
                         TList *fits)
{
  AliRsnOutTaskBin *tBin = new AliRsnOutTaskBin();
  tBin->GetValue()->SetId(0);
  tBin->AddCut(new AliRsnOutValue(id, min, max));

  // Loop over norms
  TIter nextNorm(norms);
  AliRsnOutValue *vNorm;
  while ((vNorm = (AliRsnOutValue *)nextNorm()))
  {
    AliRsnOutTaskNorm *tNorm = new AliRsnOutTaskNorm();
    tNorm->AddRange(vNorm);
    tBin->Add(tNorm);

    // Loop over fits
    TIter nextFit(fits);
    AliRsnOutValue *vFit;
    while ((vFit = (AliRsnOutValue *)nextFit()))
    {
      AliRsnOutTaskFit *tFit = new AliRsnOutTaskFit();
      tFit->SetFit(vFit);
      tNorm->Add(tFit);
    }
  }
  return tBin;
}
