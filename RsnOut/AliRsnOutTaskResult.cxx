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
    : AliRsnOutTask(name, title), fData(0), fMC(0) {}

AliRsnOutTaskResult::~AliRsnOutTaskResult() {}

void AliRsnOutTaskResult::Exec(Option_t * /*option*/) {

  // Printf("%s", GetName());

  AliRsnOutTaskInput *tInputData = dynamic_cast<AliRsnOutTaskInput *>(fData);
  if (!tInputData)
    return;

  AliRsnOutTaskBinMgr *tBinMgr;
  AliRsnOutTaskBin *tBinMgrElement;
  TIter nextBinMgr(tInputData->GetListOfTasks());
  // Loop over all Bin Managers
  while ((tBinMgr = (AliRsnOutTaskBinMgr *)nextBinMgr())) {
    tBinMgr->Print();
    TIter nextBin(tBinMgr->GetListOfTasks());
    // Loop over all bin combinations in bin manager
    while ((tBinMgrElement = (AliRsnOutTaskBin *)nextBin())) {
      ProcessBinMgrElement(tBinMgrElement);
    }
  }

  // return;

  // AliRsnOutTaskInput *tInputMC = dynamic_cast<AliRsnOutTaskInput *>(fMC);
  // TGraphAsymmErrors *mcEff = 0;

  // if (tInputMC)
  //   mcEff = tInputMC->GetMCEfficiency();

  // AliRsnOutTaskBin *tBin;
  // AliRsnOutTaskNorm *tNorm;
  // AliRsnOutTaskFit *tFit;
  // AliRsnOutValue *v;

  // // Number of bins for different result parameter (BC,FC,...)
  // Int_t nResultsBins = 10;

  // Int_t nAxis = 0; // (+1xResultBin -1xBinMgr)
  // Int_t nBinAxis = 0;
  // TString sClassName;
  // AliRsnOutTask *t = (AliRsnOutTask *)tInputData->GetListOfTasks()->At(0);
  // while (t) {
  //   sClassName = t->ClassName();
  //   if (sClassName.CompareTo("AliRsnOutTaskBin"))
  //     nBinAxis++;
  //   Printf("%s %s", t->GetName(), t->ClassName());
  //   t = (AliRsnOutTask *)t->GetListOfTasks()->At(0);
  //   nAxis++;
  // }

  // Int_t bins[nAxis];
  // Double_t mins[nAxis];
  // Double_t maxs[nAxis];

  // Int_t iAxis = 0;
  // t = (AliRsnOutTask *)tInputData->GetListOfTasks()->At(0);
  // while (t) {
  //   bins[iAxis] = t->GetListOfTasks()->GetEntries();
  //   mins[iAxis] = 0;
  //   maxs[iAxis] = t->GetListOfTasks()->GetEntries();

  //   Printf("%s %s", t->GetName(), t->ClassName());
  //   t = (AliRsnOutTask *)t->GetListOfTasks()->At(0);
  //   iAxis++;
  // }

  // bins[iAxis] = nResultsBins;
  // mins[iAxis] = 0;
  // maxs[iAxis] = nResultsBins;

  // Long64_t nEvents = tInputData->GetNEvents();
  // Printf("nAxis = %d", nAxis);

  // THnSparseD *s =
  //     new THnSparseD("sparse", "Results Sparse", nAxis, bins, mins, maxs);
  // s->Print();

  // iAxis = 0;
  // t = (AliRsnOutTask *)tInputData;
  // while (t) {
  //   Printf("Variable Bins %s %s", t->GetName(), t->ClassName());
  //   if (t) {
  //     const Int_t nVariableBins = t->GetListOfTasks()->GetEntries();
  //     Double_t varBins[nVariableBins + 1];

  //     Int_t iBin;
  //     for (iBin = 0; iBin < t->GetListOfTasks()->GetEntries(); ++iBin) {
  //       tBin = dynamic_cast<AliRsnOutTaskBin
  //       *>(t->GetListOfTasks()->At(iBin));
  //       if (!tBin)
  //         break;
  //       v = (AliRsnOutValue *)tBin->GetValue();
  //       varBins[iBin] = v->GetMin();
  //       Printf("varBins[%d]=%f", iBin, varBins[iBin]);
  //       if (iBin == t->GetListOfTasks()->GetEntries() - 1) {
  //         varBins[iBin + 1] = v->GetMax();
  //         Printf("varBins[%d+1]=%f", iBin, varBins[iBin + 1]);
  //       }
  //     }
  //     if (tBin) {
  //       s->GetAxis(iAxis)->Set(nVariableBins, varBins);
  //       s->GetAxis(iAxis)->SetName(TString::Format("bin%d", iAxis).Data());
  //       iAxis++;
  //     }
  //     t = (AliRsnOutTask *)t->GetListOfTasks()->At(0);
  //     t = dynamic_cast<AliRsnOutTaskBin *>(t);

  //   } else {
  //     break;
  //   }
  // }
  // s->GetAxis(iAxis++)->SetName("norm");
  // s->GetAxis(iAxis++)->SetName("fit");
  // s->GetAxis(iAxis++)->SetName("value");

  // fOutput->Add(s);
  // s->Print();

  // Int_t iSparseBin[nAxis];

  // TFolder *dRoot = new TFolder("hist", "Final Results");
  // fOutput->Add(dRoot);

  // FillSparse(tInputData, s, iSparseBin, 0);
  // FolderFromSparse(tInputData, s, 0, dRoot, -1);

  // // for (Int_t iTask=0;iTask<nBinAxis;iTask++) {
  // //   FolderFromSparse(tInputData,s,0, dRoot,i);
  // // }

  // return;

  // // Int_t iSparseBin[nAxis];
  // Double_t x, y, ey;
  // for (Int_t iBin = 0; iBin < tInputData->GetListOfTasks()->GetEntries();
  //      ++iBin) {
  //   tBin = (AliRsnOutTaskBin *)tInputData->GetListOfTasks()->At(iBin);
  //   v = (AliRsnOutValue *)tBin->GetValue();
  //   x = (v->GetMax() + v->GetMin()) / 2;
  //   iSparseBin[0] = s->GetAxis(0)->FindBin(x);
  //   Double_t valMCEff, errMCEff;
  //   if (mcEff) {
  //     Double_t xx;
  //     mcEff->GetPoint(iBin, xx, valMCEff);
  //     errMCEff = mcEff->GetErrorY(iBin);
  //   }
  //   for (Int_t iNorm = 0; iNorm < tBin->GetListOfTasks()->GetEntries();
  //        ++iNorm) {
  //     tNorm = (AliRsnOutTaskNorm *)tBin->GetListOfTasks()->At(iNorm);
  //     iSparseBin[1] = iNorm + 1;
  //     for (Int_t iFit = 0; iFit < tNorm->GetListOfTasks()->GetEntries();
  //          ++iFit) {
  //       tFit = (AliRsnOutTaskFit *)tNorm->GetListOfTasks()->At(iFit);
  //       iSparseBin[2] = iFit + 1;

  //       if (mcEff) {
  //         // MC eff
  //         iSparseBin[3] = 3;
  //         s->SetBinContent(iSparseBin, valMCEff);
  //         s->SetBinError(iSparseBin, errMCEff);
  //       }

  //       // Raw Bin Counting
  //       iSparseBin[3] = 1;
  //       tFit->GetYieldBinCounting(y, ey);
  //       s->SetBinContent(iSparseBin, y);
  //       s->SetBinError(iSparseBin, ey);
  //       if (mcEff) {
  //         // Corrected spectra Bin Counting
  //         iSparseBin[3] = 4;
  //         s->SetBinContent(iSparseBin, y / valMCEff / nEvents);
  //         s->SetBinError(iSparseBin,
  //                        y / valMCEff *
  //                          TMath::Sqrt(TMath::Power(ey / y, 2) +
  //                                      TMath::Power(errMCEff / valMCEff, 2))
  //                                      /
  //                          nEvents);
  //       }

  //       // Raw Fit Function
  //       iSparseBin[3] = 2;
  //       tFit->GetYieldFitFunction(y, ey);
  //       s->SetBinContent(iSparseBin, y);
  //       s->SetBinError(iSparseBin, ey);
  //       if (mcEff) {
  //         // Corrected spectra Fit Function
  //         iSparseBin[3] = 5;
  //         s->SetBinContent(iSparseBin, y / valMCEff / nEvents);
  //         s->SetBinError(iSparseBin,
  //                        y / valMCEff *
  //                          TMath::Sqrt(TMath::Power(ey / y, 2) +
  //                                      TMath::Power(errMCEff / valMCEff, 2))
  //                                      /
  //                          nEvents);
  //       }

  //       // Chi2
  //       iSparseBin[3] = 6;
  //       s->SetBinContent(iSparseBin, tFit->GetChi2());
  //       s->SetBinError(iSparseBin, 0);

  //       // Ndf
  //       iSparseBin[3] = 7;
  //       s->SetBinContent(iSparseBin, tFit->GetNdf());
  //       s->SetBinError(iSparseBin, 0);

  //       // Reduced Chi2 = Chi2/Ndf
  //       iSparseBin[3] = 8;
  //       s->SetBinContent(iSparseBin, tFit->GetReducedChi2());
  //       s->SetBinError(iSparseBin, 0);

  //       // Prob
  //       iSparseBin[3] = 9;
  //       s->SetBinContent(iSparseBin, tFit->GetProb());
  //       s->SetBinError(iSparseBin, 0);
  //     }
  //   }
  // }

  // TFolder *dRoot = new TFolder("hist", "Final Results");
  // fOutput->Add(dRoot);
  // TFolder *dNorm, *dFit;
  // TH1D *h;
  // tBin = (AliRsnOutTaskBin *)tInputData->GetListOfTasks()->At(0);
  // if (!tBin) return;
  // for (Int_t iNorm = 0; iNorm < tBin->GetListOfTasks()->GetEntries();
  // ++iNorm) {
  //   tNorm = (AliRsnOutTaskNorm *)tBin->GetListOfTasks()->At(iNorm);
  //   if (!tNorm) continue;
  //   dNorm = dRoot->AddFolder(tNorm->GetName(), tNorm->GetTitle());
  //   s->GetAxis(1)->SetRange(iNorm + 1, iNorm + 1);
  //   for (Int_t iFit = 0; iFit < tNorm->GetListOfTasks()->GetEntries();
  //   ++iFit) {
  //     tFit = (AliRsnOutTaskFit *)tNorm->GetListOfTasks()->At(iFit);
  //     if (!tFit) continue;
  //     dFit = dNorm->AddFolder(tFit->GetName(), tFit->GetTitle());
  //     s->GetAxis(2)->SetRange(iFit + 1, iFit + 1);
  //     s->GetAxis(3)->SetRange(1, 1);
  //     h = s->Projection(0);
  //     h->SetName("hRawBC");
  //     h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
  //     dFit->Add(h);
  //     s->GetAxis(3)->SetRange(2, 2);
  //     h = s->Projection(0);
  //     h->SetName("hRawFF");
  //     h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
  //     dFit->Add(h);
  //     if (mcEff) {
  //       s->GetAxis(3)->SetRange(3, 3);
  //       h = s->Projection(0);
  //       h->SetName("hMCEff");
  //       h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
  //       dFit->Add(h);
  //       s->GetAxis(3)->SetRange(4, 4);
  //       h = s->Projection(0);
  //       h->SetName("hCorrBC");
  //       h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
  //       dFit->Add(h);
  //       s->GetAxis(3)->SetRange(5, 5);
  //       h = s->Projection(0);
  //       h->SetName("hCorrFF");
  //       h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
  //       dFit->Add(h);
  //     }
  //     s->GetAxis(3)->SetRange(6, 6);
  //     h = s->Projection(0);
  //     h->SetName("hChi2");
  //     h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
  //     dFit->Add(h);
  //     s->GetAxis(3)->SetRange(7, 7);
  //     h = s->Projection(0);
  //     h->SetName("hNdf");
  //     h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
  //     dFit->Add(h);
  //     s->GetAxis(3)->SetRange(8, 8);
  //     h = s->Projection(0);
  //     h->SetName("hReducedChi2");
  //     h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
  //     dFit->Add(h);
  //     s->GetAxis(3)->SetRange(9, 9);
  //     h = s->Projection(0);
  //     h->SetName("hProb");
  //     h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
  //     dFit->Add(h);
  //   }
  // }
}

void AliRsnOutTaskResult::ProcessBinMgrElement(AliRsnOutTaskBin *bme) {
  bme->Print();

  TFolder *folder = new TFolder(bme->GetName(), "");
  fOutput->Add(folder);

  // Create spares obejct
  THnSparse *s = CreateSparse(bme, folder);

  // Fill Sparse object
  Int_t iSparseBin[s->GetNdimensions()];
  FillSparse(bme, s, iSparseBin, 0);

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

  Printf("nAxis=%d nBinAxis=%d", nAxis, nBinAxis);
  TString names[nAxis];
  Int_t bins[nAxis];
  Double_t mins[nAxis];
  Double_t maxs[nAxis];
  Int_t iAxis = 0;
  TList *l;
  t = (AliRsnOutTask *)bme->GetListOfTasks()->At(0);
  while (t) {
    sClassName = t->ClassName();
    Printf("sClassName=%s", sClassName.Data());
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
    Printf("names[%d]=%s bins=%d", iAxis, names[iAxis].Data(), bins[iAxis]);
    s->GetAxis(iAxis)->SetName(names[iAxis].Data());

    if (iAxis < nBinAxis) {
      Int_t nVariableBins = t->GetListOfTasks()->GetSize();
      Double_t varBins[nVariableBins + 1];
      GetBinsFromTask(t, varBins);
      s->GetAxis(iAxis)->Set(nVariableBins, varBins);
    }

    t = (AliRsnOutTask *)t->GetListOfTasks()->At(0);
  }

  // const Int_t nVariableBins = t->GetListOfTasks()->GetEntries();
  //     Double_t varBins[nVariableBins + 1];

  //     Int_t iBin;
  //     for (iBin = 0; iBin < t->GetListOfTasks()->GetEntries(); ++iBin) {
  //       tBin = dynamic_cast<AliRsnOutTaskBin
  //       *>(t->GetListOfTasks()->At(iBin));
  //       if (!tBin)
  //         break;
  //       v = (AliRsnOutValue *)tBin->GetValue();
  //       varBins[iBin] = v->GetMin();
  //       Printf("varBins[%d]=%f", iBin, varBins[iBin]);
  //       if (iBin == t->GetListOfTasks()->GetEntries() - 1) {
  //         varBins[iBin + 1] = v->GetMax();
  //         Printf("varBins[%d+1]=%f", iBin, varBins[iBin + 1]);
  //       }
  //     }
  //     if (tBin) {
  //       s->GetAxis(iAxis)->Set(nVariableBins, varBins);
  //       s->GetAxis(iAxis)->SetName(TString::Format("bin%d", iAxis).Data());
  //       iAxis++;
  //     }

  folder->Add(s);
  s->Print();
  return s;
}

void AliRsnOutTaskResult::FillSparse(AliRsnOutTask *task, THnSparse *s,
                                     Int_t *sparseBin, Int_t level) {
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
      FillSparse(t, s, sparseBin, level + 1);
    } else if (tNorm) {
      // Printf("Norm %s", t->GetName());
      sparseBin[level] = iTask + 1;
      // Printf("%s sparseBin[%d]=%d", t->GetName(), level, sparseBin[level]);
      FillSparse(t, s, sparseBin, level + 1);
    } else if (tFit) {
      // Printf("Fit %s", t->GetName());
      sparseBin[level] = iTask + 1;
      // Printf("%s sparseBin[%d]=%d", t->GetName(), level, sparseBin[level]);
      FillSparse(t, s, sparseBin, level + 1);
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

      // Chi2
      // sparseBin[level + 1] = 6;
      sparseBin[level + 1] = iBin + 3;
      s->SetBinContent(sparseBin, hResultPar->GetBinContent(iBin));
      s->SetBinError(sparseBin, hResultPar->GetBinError(iBin));
      iBin++;

      // Ndf
      sparseBin[level + 1] = iBin + 3;
      s->SetBinContent(sparseBin, hResultPar->GetBinContent(iBin));
      s->SetBinError(sparseBin, hResultPar->GetBinError(iBin));
      iBin++;

      // Reduced Chi2 = Chi2/Ndf
      sparseBin[level + 1] = iBin + 3;
      s->SetBinContent(sparseBin, hResultPar->GetBinContent(iBin));
      s->SetBinError(sparseBin, hResultPar->GetBinError(iBin));
      iBin++;

      // Prob
      sparseBin[level + 1] = iBin + 3;
      s->SetBinContent(sparseBin, hResultPar->GetBinContent(iBin));
      s->SetBinError(sparseBin, hResultPar->GetBinError(iBin));
      iBin++;

      // Printf("FillSparse:: Values ->>>>>");
      // for (Int_t jj = 0; jj < s->GetNdimensions() - 1; jj++)
      //   printf("%d ", sparseBin[jj]);
      // for (Int_t ii = 1; ii < 10; ii++) {
      //   sparseBin[level + 1] = ii;
      //   printf("bc=%f ", s->GetBinContent(sparseBin));
      // }
      // Printf("");
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
        Printf("FolderFromSparse:  %s folder=%s level=%d range[%d,%d] "
               "projLevel=%d projX=%d projY=%d",
               t->GetName(), f->GetName(), level, iTask + 1, iTask + 1,
               projLevel, projX, projY);
        f = folder->AddFolder(t->GetName(), t->GetTitle());
        s->GetAxis(level)->SetRange(iTask + 1, iTask + 1);
      } else {
        Printf("FolderFromSparse:  %s folder=%s level=%d range=%d,%d "
               "projLevel=%d projX=%d projY=%d",
               t->GetName(), f->GetName(), level, 0, 0, projLevel, projX,
               projY);
        s->GetAxis(level)->SetRange(0, 0);
      }
      Int_t projLevelX = projLevel;
      if (projLevelX >= 0)
        projLevelX = projLevel + 1;

      FolderFromSparse(t, s, level + 1, f, projLevelX, projX, projY);
      if ((projX >= 0) && (projLevel == projX)) {
        Printf("FolderFromSparse:  %s folder=%s level=%d reset", t->GetName(),
               f->GetName(), level);
      }
      Printf("FolderFromSparse:  %s folder=%s level=%d reset", t->GetName(),
             f->GetName(), level);
      s->GetAxis(level)->SetRange(0, 0);

    } else if (tNorm) {
      s->GetAxis(level)->SetRange(iTask + 1, iTask + 1);
      f = folder->AddFolder(t->GetName(), t->GetTitle());
      Printf("FolderFromSparse:  %s folder=%s level=%d range[%d,%d]",
             t->GetName(), f->GetName(), level, iTask + 1, iTask + 1);
      FolderFromSparse(t, s, level + 1, f, projLevel, projX, projY);
      Printf("FolderFromSparse:  %s folder=%s level=%d reset", t->GetName(),
             f->GetName(), level);
      s->GetAxis(level)->SetRange(0, 0);
    } else if (tFit) {
      s->GetAxis(level)->SetRange(iTask + 1, iTask + 1);
      f = folder->AddFolder(t->GetName(), t->GetTitle());
      Printf("FolderFromSparse:  %s folder=%s level=%d range[%d,%d]",
             t->GetName(), f->GetName(), level, iTask + 1, iTask + 1);
      FolderFromSparse(t, s, level + 1, f, projLevel, projX, projY);
      Printf("FolderFromSparse:  %s folder=%s level=%d reset", t->GetName(),
             f->GetName(), level);
      s->GetAxis(level)->SetRange(0, 0);
    } else {
      Printf("??? %s", t->GetName());
    }
  }

  // We Fill sparse values
  if (!task->GetListOfTasks()->GetSize()) {
    TH1 *h;
    TH2 *h2;
    Printf("We are filling sparse (level=%d) projLevel=%d projX=%d projY=%d",
           level, projLevel, projX, projY);
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

void AliRsnOutTaskResult::SetData(AliRsnOutTask *data) {
  if (!data)
    return;
  fData = data;
  fName = TString::Format("%s_%s", fData ? fData->GetName() : "no_data",
                          fMC ? fMC->GetName() : "no_eff");
  fTitle = TString::Format("%s (%s)", fData ? fData->GetTitle() : "no_data",
                           fMC ? fMC->GetTitle() : "No Eff");
}

void AliRsnOutTaskResult::SetMC(AliRsnOutTask *mc) {
  if (!mc)
    return;
  fMC = mc;
  fName = TString::Format("%s_%s", fData ? fData->GetName() : "no_data",
                          fMC ? fMC->GetName() : "no_eff");
  fTitle = TString::Format("%s (%s)", fData ? fData->GetTitle() : "no_data",
                           fMC ? fMC->GetTitle() : "No Eff");
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
