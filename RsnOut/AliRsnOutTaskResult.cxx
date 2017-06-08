#include <TFolder.h>
#include <TGraphAsymmErrors.h>
#include <TH2.h>
#include <THnSparse.h>

#include "AliRsnOutTaskBin.h"
#include "AliRsnOutTaskBinMgr.h"

#include "AliRsnOutTaskFit.h"
#include "AliRsnOutTaskInput.h"
#include "AliRsnOutTaskNorm.h"

#include "AliRsnOutTaskResult.h"

ClassImp(AliRsnOutTaskResult);

AliRsnOutTaskResult::AliRsnOutTaskResult(const char *name, const char *title)
    : AliRsnOutTask(name, title), fData(0), fMC(0), fIsEventNormalization(kTRUE) {}

AliRsnOutTaskResult::~AliRsnOutTaskResult() {}

void AliRsnOutTaskResult::Exec(Option_t * /*option*/) {

  // Printf("%s", GetName());

  AliRsnOutTaskInput *tInputData = fData;
  if (!tInputData)
    return;

  AliRsnOutTaskInput *tInputMC = fMC;

  AliRsnOutTaskBinMgr *tBinMgr, *tBinMgrMC;
  AliRsnOutTaskBin *tBinMgrElement, *tBinMgrElementMC;
  TIter nextBinMgr(tInputData->GetListOfTasks());
  TList *lBinMgrMC = 0, *lBinMgrElementMC = 0;
  if (tInputMC)
    lBinMgrMC = tInputMC->GetListOfTasks();
  // Loop over all Bin Managers
  Int_t i = 0, j = 0;
  while ((tBinMgr = (AliRsnOutTaskBinMgr *)nextBinMgr())) {
    tBinMgr->Print();
    if (lBinMgrMC) {
      tBinMgrMC = (AliRsnOutTaskBinMgr *)lBinMgrMC->At(i++);
      lBinMgrElementMC = tBinMgrMC->GetListOfTasks();
    }

    TIter nextBin(tBinMgr->GetListOfTasks());
    // Loop over all bin combinations in bin manager
    j = 0;
    while ((tBinMgrElement = (AliRsnOutTaskBin *)nextBin())) {
      if (lBinMgrElementMC)
        tBinMgrElementMC = (AliRsnOutTaskBin *)lBinMgrElementMC->At(j++);

      ProcessBinMgrElement(tBinMgrElement, tBinMgrElementMC);
    }
  }
}

void AliRsnOutTaskResult::ProcessBinMgrElement(AliRsnOutTaskBin *bme,
                                               AliRsnOutTaskBin *bmeMC) {
  bme->Print();
  if (bmeMC)
    bmeMC->Print();

  TFolder *folder = new TFolder(bme->GetName(), "");
  fOutput->Add(folder);

  // Create spares obejct
  THnSparse *s = CreateSparse(bme, folder);

  // Fill Sparse object
  Int_t iSparseBin[s->GetNdimensions()];
  FillSparse(bme, bmeMC, s, iSparseBin, 0);

  // return;

  TString name(bme->GetName());
  name.ReplaceAll("_vs_", " ");
  TObjArray *arr = name.Tokenize(" ");

  Int_t nVs = arr->GetEntries() - 1;

  folder = folder->AddFolder("spectra", "Spectra");
  // Int_t nBins = 0;
  if (!nVs) {
    FolderFromSparse(bme, s, 0, folder, -1, -1, -1);
  } else {
    Printf("Doing 2D case !!!!");
    FolderFromSparse(bme, s, 0, folder, 0, 0, 1);
    FolderFromSparse(bme, s, 0, folder, 0, 1, 0);
    FolderFromSparse(bme, s, 0, folder, -2, 1, 0);
  }

  // // for (Int_t i=0;i<nBinAxis;i++) {
  // //   FolderFromSparse(tInputData,s,0, dRoot,i);
  // // }
}

THnSparse *AliRsnOutTaskResult::CreateSparse(AliRsnOutTaskBin *bme,
                                             TFolder *folder) {

  // Number of bins for different result parameter (BC,FC,...)
  Int_t nResultsBins = 10;

  // Setting up axis for sparse
  Int_t nAxis = 1; // (+1xResultBin)
  Int_t nBinAxis = 0;

  TString sClassName;
  AliRsnOutTask *t = (AliRsnOutTask *)bme->GetListOfTasks()->At(0);
  // Looping over first tasks to find out number of levels
  while (t) {
    nAxis++;
    sClassName = t->ClassName();
    if (!sClassName.CompareTo("AliRsnOutTaskBin"))
      nBinAxis++;
    t = (AliRsnOutTask *)t->GetListOfTasks()->At(0);
  }

  // Printf("nAxis=%d nBinAxis=%d", nAxis, nBinAxis);
  TString names[nAxis];
  Int_t bins[nAxis];
  Double_t mins[nAxis];
  Double_t maxs[nAxis];
  Int_t iAxis = 0;
  TList *l;
  t = (AliRsnOutTask *)bme->GetListOfTasks()->At(0);
  while (t) {
    sClassName = t->ClassName();
    // Printf("sClassName=%s", sClassName.Data());
    if (!sClassName.CompareTo("AliRsnOutTaskBin")) {
      names[iAxis] = t->GetName();
      names[iAxis] = names[iAxis](0, names[iAxis].First("["));
    }
    if (!sClassName.CompareTo("AliRsnOutTaskNorm"))
      names[iAxis] = "norm";
    if (!sClassName.CompareTo("AliRsnOutTaskFit"))
      names[iAxis] = "fit";

    l = t->GetParent()->GetListOfTasks();
    if (l) {
      bins[iAxis] = l->GetEntries();
      mins[iAxis] = 0;
      maxs[iAxis] = l->GetEntries();
      iAxis++;
      t = (AliRsnOutTask *)t->GetListOfTasks()->At(0);
    } else {
      break;
    }
  }
  Printf("iAxis=%d", iAxis);
  names[iAxis] = "par";
  bins[iAxis] = nResultsBins;
  mins[iAxis] = 0;
  maxs[iAxis] = nResultsBins;

  THnSparseD *s;
  s = new THnSparseD("values", "All Values", nAxis, bins, mins, maxs);

  t = (AliRsnOutTask *)bme;
  for (iAxis = 0; iAxis < nAxis; iAxis++) {
    // Printf("names[%d]=%s bins=%d", iAxis, names[iAxis].Data(), bins[iAxis]);
    s->GetAxis(iAxis)->SetName(names[iAxis].Data());

    if (iAxis < nBinAxis) {
      Int_t nVariableBins = t->GetListOfTasks()->GetSize();
      Double_t varBins[nVariableBins + 1];
      GetBinsFromTask(t, varBins);
      s->GetAxis(iAxis)->Set(nVariableBins, varBins);
    }

    t = (AliRsnOutTask *)t->GetListOfTasks()->At(0);
  }

  folder->Add(s);
  // s->Print();
  return s;
}

void AliRsnOutTaskResult::FillSparse(AliRsnOutTask *task, AliRsnOutTask *taskMC,
                                     THnSparse *s, Int_t *sparseBin,
                                     Int_t level) {
  AliRsnOutValue *v;

  AliRsnOutTask *t;
  AliRsnOutTaskBin *tBin;
  AliRsnOutTaskNorm *tNorm;
  AliRsnOutTaskFit *tFit;

  Double_t x;
  TList *l = task->GetListOfTasks();
  for (Int_t iTask = 0; iTask < l->GetEntries(); iTask++) {
    t = (AliRsnOutTask *)l->At(iTask);
    tBin = dynamic_cast<AliRsnOutTaskBin *>(t);
    tNorm = dynamic_cast<AliRsnOutTaskNorm *>(t);
    tFit = dynamic_cast<AliRsnOutTaskFit *>(t);
    if (tBin) {
      v = (AliRsnOutValue *)tBin->GetValue();
      x = (v->GetMax() + v->GetMin()) / 2;
      sparseBin[level] = s->GetAxis(level)->FindBin(x);
      // Printf("FillSparse:: %s sparseBin[%d]=%d x=%f", t->GetName(), level,
      //  sparseBin[level], x);
      FillSparse(t, taskMC, s, sparseBin, level + 1);
    } else if (tNorm) {
      // Printf("Norm %s", t->GetName());
      sparseBin[level] = iTask + 1;
      // Printf("%s sparseBin[%d]=%d", t->GetName(), level, sparseBin[level]);
      FillSparse(t, taskMC, s, sparseBin, level + 1);
    } else if (tFit) {
      // Printf("Fit %s", t->GetName());
      sparseBin[level] = iTask + 1;
      // Printf("%s sparseBin[%d]=%d", t->GetName(), level, sparseBin[level]);
      FillSparse(t, taskMC, s, sparseBin, level + 1);
    } else {
      Printf("??? %s", t->GetName());
    }

    // We Fill sparse values
    if (!t->GetListOfTasks()->GetSize()) {
      // Printf("FillSparse:: We are filling sparse %d (level+1=%d)", iTask,
      //  level + 1);
      TH1 *hResultPar = (TH1 *)tFit->GetOutput()->FindObject("fResultPar");
      if (!hResultPar)
        return;

      TGraphAsymmErrors *eff_graph = 0;
      TH2 *eff_histo = 0;

      Int_t iBin = 1;
      // Raw Bin Counting
      sparseBin[level + 1] = iBin;
      // tFit->GetYieldBinCounting(y, ey);
      s->SetBinContent(sparseBin, hResultPar->GetBinContent(iBin));
      s->SetBinError(sparseBin, hResultPar->GetBinError(iBin));
      iBin++;

      // Raw Fit Function
      sparseBin[level + 1] = iBin;
      // tFit->GetYieldBinCounting(y, ey);
      s->SetBinContent(sparseBin, hResultPar->GetBinContent(iBin));
      s->SetBinError(sparseBin, hResultPar->GetBinError(iBin));
      iBin++;

      if (taskMC) {
        eff_graph =
            (TGraphAsymmErrors *)taskMC->GetOutput()->FindObject("eff_graph");
        eff_histo = (TH2 *)taskMC->GetOutput()->FindObject("eff_histo");
      }
      Double_t valX, effVal, effErr;
      if (eff_graph || eff_histo) {
        // Eff Function
        sparseBin[level + 1] = iBin;
        // tFit->GetYieldBinCounting(y, ey);

        if (eff_graph) {
          eff_graph->Print("all");
          Int_t indexGr = sparseBin[level - 2] - 1;
          eff_graph->GetPoint(indexGr, valX, effVal);
          effErr = eff_graph->GetErrorYhigh(indexGr);
          s->SetBinContent(sparseBin, effVal);
          s->SetBinError(sparseBin, effErr);
        } else if (eff_histo) {

          Int_t iX = sparseBin[level - 3];
          Int_t iY = sparseBin[level - 2];
          eff_histo->Print("all");

          effVal = eff_histo->GetBinContent(iX, iY);
          effErr = eff_histo->GetBinError(iX, iY);
          Printf("EffXXXX bc=%f err=%f", eff_histo->GetBinContent(iX, iY),
                 eff_histo->GetBinError(iX, iY));
          s->SetBinContent(sparseBin, effVal);
          s->SetBinError(sparseBin, effErr);
        }
      }
      iBin++;
      task->GetPath()->Print();
      Int_t n = task->GetPath()->GetEntries();
      Int_t min = 0, max = 0;
      TString title;
      AliRsnOutTaskBin *tt = 0;
      for (Int_t i = 1; i <= 2; i++) {
        if (!min && !max) {
          tt = (AliRsnOutTaskBin *)task->GetPath()->At(n - i);
          if (tt) {
            title = tt->GetName();
            if (title.Contains("mult")) {
              AliRsnOutValue *v = tt->GetValue();
              min = v->GetMin();
              max = v->GetMax();
            }
          }
        }
      }
      Double_t nEvents = fData->GetNEvents(min, max);
      if (!fIsEventNormalization) nEvents = 1;
      Int_t nBinMC = 0;
      if (eff_graph || eff_histo) {
        nBinMC = 3;
        Printf("iBin=%d bc=%f eff=%f", iBin, hResultPar->GetBinContent(iBin-nBinMC), effVal);
        // Corr Bin Counting
        sparseBin[level + 1] = iBin;
        s->SetBinContent(sparseBin,
                         hResultPar->GetBinContent(iBin-nBinMC) / effVal / nEvents);
        Double_t e =
            GetErrorDivide(hResultPar->GetBinContent(iBin-nBinMC),
                           hResultPar->GetBinError(iBin-nBinMC), effVal, effErr);
        e /= nEvents;
        s->SetBinError(sparseBin, e);
      }
      iBin++;
      if (eff_graph || eff_histo) {
        Printf("iBin=%d ff=%f eff=%f", iBin, hResultPar->GetBinContent(iBin-nBinMC), effVal);
        // Corr Fit Function
        sparseBin[level + 1] = iBin;
        s->SetBinContent(sparseBin,
                         hResultPar->GetBinContent(iBin-nBinMC) / effVal / nEvents);

        Double_t e =
            GetErrorDivide(hResultPar->GetBinContent(iBin-nBinMC),
                           hResultPar->GetBinError(iBin-nBinMC), effVal, effErr);
        e /= nEvents;
        s->SetBinError(sparseBin, e);


      }
      iBin++;

      // Chi2
      // sparseBin[level + 1] = 6;
      sparseBin[level + 1] = iBin;
      s->SetBinContent(sparseBin, hResultPar->GetBinContent(iBin - nBinMC));
      s->SetBinError(sparseBin, hResultPar->GetBinError(iBin - nBinMC));
      iBin++;

      // Ndf
      sparseBin[level + 1] = iBin;
      s->SetBinContent(sparseBin, hResultPar->GetBinContent(iBin - nBinMC));
      s->SetBinError(sparseBin, hResultPar->GetBinError(iBin - nBinMC));
      iBin++;

      // Reduced Chi2 = Chi2/Ndf
      sparseBin[level + 1] = iBin;
      s->SetBinContent(sparseBin, hResultPar->GetBinContent(iBin - nBinMC));
      s->SetBinError(sparseBin, hResultPar->GetBinError(iBin - nBinMC));
      iBin++;

      // Prob
      sparseBin[level + 1] = iBin;
      s->SetBinContent(sparseBin, hResultPar->GetBinContent(iBin - nBinMC));
      s->SetBinError(sparseBin, hResultPar->GetBinError(iBin - nBinMC));
      iBin++;

      Printf("FillSparse:: Values ->>>>>");
      for (Int_t jj = 0; jj < s->GetNdimensions() - 1; jj++)
        printf("%d ", sparseBin[jj]);
      for (Int_t ii = 1; ii < 10; ii++) {
        sparseBin[level + 1] = ii;
        printf("bc=%f ", s->GetBinContent(sparseBin));
      }
      printf("\n");
    }
  }
}

void AliRsnOutTaskResult::FolderFromSparse(AliRsnOutTask *task, THnSparse *s,
                                           Int_t level, TFolder *folder,
                                           Int_t projLevel, Int_t projX,
                                           Int_t projY) {
  TFolder *f = folder;
  AliRsnOutTask *t;
  AliRsnOutTaskBin *tBin;
  AliRsnOutTaskNorm *tNorm;
  AliRsnOutTaskFit *tFit;

  for (Int_t iTask = 0; iTask < task->GetListOfTasks()->GetEntries(); iTask++) {
    t = (AliRsnOutTask *)task->GetListOfTasks()->At(iTask);
    tBin = dynamic_cast<AliRsnOutTaskBin *>(t);
    tNorm = dynamic_cast<AliRsnOutTaskNorm *>(t);
    tFit = dynamic_cast<AliRsnOutTaskFit *>(t);

    if (tBin) {

      // projectionID CAN be >0 when 2D parameters

      if ((projX >= 0) && (projLevel == projX)) {
        // Printf("FolderFromSparse:  %s folder=%s level=%d range[%d,%d] "
        //        "projLevel=%d projX=%d projY=%d",
        //        t->GetName(), f->GetName(), level, iTask + 1, iTask + 1,
        //        projLevel, projX, projY);
        f = folder->AddFolder(t->GetName(), t->GetTitle());
        s->GetAxis(level)->SetRange(iTask + 1, iTask + 1);
      } else {
        // Printf("FolderFromSparse:  %s folder=%s level=%d range=%d,%d "
        //        "projLevel=%d projX=%d projY=%d",
        //        t->GetName(), f->GetName(), level, 0, 0, projLevel, projX,
        //        projY);
        s->GetAxis(level)->SetRange(0, 0);
      }
      Int_t projLevelX = projLevel;
      if (projLevelX >= 0)
        projLevelX = projLevel + 1;

      FolderFromSparse(t, s, level + 1, f, projLevelX, projX, projY);
      // if ((projX >= 0) && (projLevel == projX)) {
      //   Printf("FolderFromSparse:  %s folder=%s level=%d reset",
      //   t->GetName(),
      //          f->GetName(), level);
      // }
      // Printf("FolderFromSparse:  %s folder=%s level=%d reset", t->GetName(),
      //        f->GetName(), level);
      s->GetAxis(level)->SetRange(0, 0);

    } else if (tNorm) {
      s->GetAxis(level)->SetRange(iTask + 1, iTask + 1);
      f = folder->AddFolder(t->GetName(), t->GetTitle());
      // Printf("FolderFromSparse:  %s folder=%s level=%d range[%d,%d]",
      //        t->GetName(), f->GetName(), level, iTask + 1, iTask + 1);
      FolderFromSparse(t, s, level + 1, f, projLevel, projX, projY);
      // Printf("FolderFromSparse:  %s folder=%s level=%d reset", t->GetName(),
      //        f->GetName(), level);
      s->GetAxis(level)->SetRange(0, 0);
    } else if (tFit) {
      s->GetAxis(level)->SetRange(iTask + 1, iTask + 1);
      f = folder->AddFolder(t->GetName(), t->GetTitle());
      // Printf("FolderFromSparse:  %s folder=%s level=%d range[%d,%d]",
      //        t->GetName(), f->GetName(), level, iTask + 1, iTask + 1);
      FolderFromSparse(t, s, level + 1, f, projLevel, projX, projY);
      // Printf("FolderFromSparse:  %s folder=%s level=%d reset", t->GetName(),
      //        f->GetName(), level);
      s->GetAxis(level)->SetRange(0, 0);
    } else {
      Printf("??? %s", t->GetName());
    }
  }

  // We Fill sparse values
  if (!task->GetListOfTasks()->GetSize()) {
    TH1 *h;
    TH2 *h2;
    // Printf("We are filling sparse (level=%d) projLevel=%d projX=%d projY=%d",
    //        level, projLevel, projX, projY);
    if (projLevel == -2) {
      s->GetAxis(level)->SetRange(1, 1);
      h2 = s->Projection(projY, projX);
      h2->SetName("hRawBC");
      h2->SetStats(0);
      f->Add(h2);
      s->GetAxis(level)->SetRange(2, 2);
      h2 = s->Projection(projY, projX);
      h2->SetName("hRawFF");
      h2->SetStats(0);
      f->Add(h2);
      s->GetAxis(level)->SetRange(3, 3);
      h2 = s->Projection(projY, projX);
      h2->SetName("hEff");
      h2->SetStats(0);
      f->Add(h2);
      s->GetAxis(level)->SetRange(4, 4);
      h2 = s->Projection(projY, projX);
      h2->SetName("hCorBC");
      h2->SetStats(0);
      f->Add(h2);
      s->GetAxis(level)->SetRange(5, 5);
      h2 = s->Projection(projY, projX);
      h2->SetName("hCorrFF");
      h2->SetStats(0);
      f->Add(h2);
      s->GetAxis(level)->SetRange(6, 6);
      h2 = s->Projection(projY, projX);
      h2->SetName("hChi2");
      h2->SetStats(0);
      f->Add(h2);
      s->GetAxis(level)->SetRange(7, 7);
      h2 = s->Projection(projY, projX);
      h2->SetName("hNdf");
      h2->SetStats(0);
      f->Add(h2);
      s->GetAxis(level)->SetRange(8, 8);
      h2 = s->Projection(projY, projX);
      h2->SetName("hReducedChi2");
      h2->SetStats(0);
      f->Add(h2);
      s->GetAxis(level)->SetRange(9, 9);
      h2 = s->Projection(projY, projX);
      h2->SetName("hProb");
      h2->SetStats(0);
      f->Add(h2);
    } else {
      Int_t proj = projY;
      if (projY < 0)
        proj = 0;
      // if (projLevel > 1) proj-=2;
      s->GetAxis(level)->SetRange(1, 1);
      h = s->Projection(proj);
      h->SetName("hRawBC");
      h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
      h->SetStats(0);
      f->Add(h);
      s->GetAxis(level)->SetRange(2, 2);
      h = s->Projection(proj);
      h->SetName("hRawFF");
      h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
      h->SetStats(0);
      f->Add(h);
      s->GetAxis(level)->SetRange(3, 3);
      h = s->Projection(proj);
      h->SetName("hEff");
      h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
      h->SetStats(0);
      f->Add(h);
      s->GetAxis(level)->SetRange(4, 4);
      h = s->Projection(proj);
      h->SetName("hCorBC");
      h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
      h->SetStats(0);
      f->Add(h);
      s->GetAxis(level)->SetRange(5, 5);
      h = s->Projection(proj);
      h->SetName("hCorFF");
      h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
      h->SetStats(0);
      f->Add(h);
      s->GetAxis(level)->SetRange(6, 6);
      h = s->Projection(proj);
      h->SetName("hChi2");
      h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
      h->SetStats(0);
      f->Add(h);
      s->GetAxis(level)->SetRange(7, 7);
      h = s->Projection(proj);
      h->SetName("hNdf");
      h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
      h->SetStats(0);
      f->Add(h);
      s->GetAxis(level)->SetRange(8, 8);
      h = s->Projection(proj);
      h->SetName("hReducedChi2");
      h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
      h->SetStats(0);
      f->Add(h);
      s->GetAxis(level)->SetRange(9, 9);
      h = s->Projection(proj);
      h->SetName("hProb");
      h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
      h->SetStats(0);
      f->Add(h);
    }
  }
}

void AliRsnOutTaskResult::SetData(AliRsnOutTaskInput *data) {
  if (!data)
    return;
  fData = data;
  fName = TString::Format("%s-%s", fData ? fData->GetName() : "no_data",
                          fMC ? fMC->GetName() : "no_mc");
  fTitle = TString::Format("%s (%s)", fData ? fData->GetTitle() : "No Data",
                           fMC ? fMC->GetTitle() : "No MC");
}

void AliRsnOutTaskResult::SetMC(AliRsnOutTaskInput *mc) {
  if (!mc)
    return;
  fMC = mc;
  fName = TString::Format("%s-%s", fData ? fData->GetName() : "no_data",
                          fMC ? fMC->GetName() : "no_mc");
  fTitle = TString::Format("%s (%s)", fData ? fData->GetTitle() : "No Data",
                           fMC ? fMC->GetTitle() : "No MC");
}

void AliRsnOutTaskResult::GetBinsFromTask(AliRsnOutTask *t, Double_t *varBins) {

  Int_t iBin = 0;
  TIter next(t->GetListOfTasks());
  AliRsnOutTaskBin *tBin;
  AliRsnOutValue *v;
  while ((tBin = dynamic_cast<AliRsnOutTaskBin *>(next()))) {

    v = (AliRsnOutValue *)tBin->GetValue();
    varBins[iBin] = v->GetMin();
    // Printf("varBins[%d]=%f", iBin, varBins[iBin]);
    if (iBin == t->GetListOfTasks()->GetEntries() - 1) {
      varBins[iBin + 1] = v->GetMax();
      // Printf("varBins[%d+1]=%f", iBin, varBins[iBin + 1]);
    }
    iBin++;
  }
}

Double_t AliRsnOutTaskResult::GetErrorDivide(Double_t val1, Double_t err1,
                                             Double_t val2, Double_t err2) {
  return val1 / val2 * TMath::Sqrt(TMath::Power(err1 / val1, 2) +
                                   TMath::Power(err2 / val2, 2));
}