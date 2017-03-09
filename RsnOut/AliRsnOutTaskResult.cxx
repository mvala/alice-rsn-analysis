#include <TFolder.h>
#include <TGraphAsymmErrors.h>
#include <TH1.h>
#include <THnSparse.h>

#include "AliRsnOutTaskBin.h"
#include "AliRsnOutTaskBinMgr.h"

#include "AliRsnOutTaskFit.h"
#include "AliRsnOutTaskInput.h"
#include "AliRsnOutTaskNorm.h"

#include "AliRsnOutTaskResult.h"

ClassImp(AliRsnOutTaskResult)

    AliRsnOutTaskResult::AliRsnOutTaskResult(const char *name,
                                             const char *title)
    : AliRsnOutTask(name, title), fData(0), fMC(0) {}

AliRsnOutTaskResult::~AliRsnOutTaskResult() {}

void AliRsnOutTaskResult::Exec(Option_t * /*option*/) {

  Printf("%s", GetName());

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

  return;

  AliRsnOutTaskInput *tInputMC = dynamic_cast<AliRsnOutTaskInput *>(fMC);
  TGraphAsymmErrors *mcEff = 0;

  if (tInputMC)
    mcEff = tInputMC->GetMCEfficiency();

  AliRsnOutTaskBin *tBin;
  AliRsnOutTaskNorm *tNorm;
  AliRsnOutTaskFit *tFit;
  AliRsnOutValue *v;

  // Number of bins for different result parameter (BC,FC,...)
  Int_t nResultsBins = 10;

  Int_t nAxis = 0; // (+1xResultBin -1xBinMgr)
  Int_t nBinAxis = 0;
  TString sClassName;
  AliRsnOutTask *t = (AliRsnOutTask *)tInputData->GetListOfTasks()->At(0);
  while (t) {
    sClassName = t->ClassName();
    if (sClassName.CompareTo("AliRsnOutTaskBin"))
      nBinAxis++;
    Printf("%s %s", t->GetName(), t->ClassName());
    t = (AliRsnOutTask *)t->GetListOfTasks()->At(0);
    nAxis++;
  }

  Int_t bins[nAxis];
  Double_t mins[nAxis];
  Double_t maxs[nAxis];

  Int_t iAxis = 0;
  t = (AliRsnOutTask *)tInputData->GetListOfTasks()->At(0);
  while (t) {
    bins[iAxis] = t->GetListOfTasks()->GetEntries();
    mins[iAxis] = 0;
    maxs[iAxis] = t->GetListOfTasks()->GetEntries();

    Printf("%s %s", t->GetName(), t->ClassName());
    t = (AliRsnOutTask *)t->GetListOfTasks()->At(0);
    iAxis++;
  }

  bins[iAxis] = nResultsBins;
  mins[iAxis] = 0;
  maxs[iAxis] = nResultsBins;

  Long64_t nEvents = tInputData->GetNEvents();
  Printf("nAxis = %d", nAxis);

  THnSparseD *s =
      new THnSparseD("sparse", "Results Sparse", nAxis, bins, mins, maxs);
  s->Print();

  iAxis = 0;
  t = (AliRsnOutTask *)tInputData;
  while (t) {
    Printf("Variable Bins %s %s", t->GetName(), t->ClassName());
    if (t) {
      const Int_t nVariableBins = t->GetListOfTasks()->GetEntries();
      Double_t varBins[nVariableBins + 1];

      Int_t iBin;
      for (iBin = 0; iBin < t->GetListOfTasks()->GetEntries(); ++iBin) {
        tBin = dynamic_cast<AliRsnOutTaskBin *>(t->GetListOfTasks()->At(iBin));
        if (!tBin)
          break;
        v = (AliRsnOutValue *)tBin->GetValue();
        varBins[iBin] = v->GetMin();
        Printf("varBins[%d]=%f", iBin, varBins[iBin]);
        if (iBin == t->GetListOfTasks()->GetEntries() - 1) {
          varBins[iBin + 1] = v->GetMax();
          Printf("varBins[%d+1]=%f", iBin, varBins[iBin + 1]);
        }
      }
      if (tBin) {
        s->GetAxis(iAxis)->Set(nVariableBins, varBins);
        s->GetAxis(iAxis)->SetName(TString::Format("bin%d", iAxis).Data());
        iAxis++;
      }
      t = (AliRsnOutTask *)t->GetListOfTasks()->At(0);
      t = dynamic_cast<AliRsnOutTaskBin *>(t);

    } else {
      break;
    }
  }
  s->GetAxis(iAxis++)->SetName("norm");
  s->GetAxis(iAxis++)->SetName("fit");
  s->GetAxis(iAxis++)->SetName("value");

  fOutput->Add(s);
  s->Print();

  Int_t iSparseBin[nAxis];

  TFolder *dRoot = new TFolder("hist", "Final Results");
  fOutput->Add(dRoot);

  FillSparse(tInputData, s, iSparseBin, 0);
  FolderFromSparse(tInputData, s, 0, dRoot, -1);

  // for (Int_t i=0;i<nBinAxis;i++) {
  //   FolderFromSparse(tInputData,s,0, dRoot,i);
  // }

  return;

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
  CreateSparse(bme);
}

void AliRsnOutTaskResult::CreateSparse(AliRsnOutTaskBin *bme) {
  // AliRsnOutTaskBin *tBin;
  //   AliRsnOutTaskNorm *tNorm;
  //   AliRsnOutTaskFit *tFit;
  //   AliRsnOutValue *v;

  //   // Number of bins for different result parameter (BC,FC,...)
  //   Int_t nResultsBins = 10;

  //   Int_t nAxis = 0; // (+1xResultBin -1xBinMgr)
  //   Int_t nBinAxis = 0;
  //   TString sClassName;
  //   AliRsnOutTask *t = (AliRsnOutTask *)tInputData->GetListOfTasks()->At(0);
  //   while (t) {
  //     sClassName = t->ClassName();
  //     if (sClassName.CompareTo("AliRsnOutTaskBin")) nBinAxis++;
  //     Printf("%s %s", t->GetName(), t->ClassName());
  //     t = (AliRsnOutTask *) t->GetListOfTasks()->At(0);
  //     nAxis++;
  //   }

  //   Int_t bins[nAxis];
  //   Double_t mins[nAxis];
  //   Double_t maxs[nAxis];

  //   Int_t iAxis = 0;
  //   t = (AliRsnOutTask *)tInputData->GetListOfTasks()->At(0);
  //   while (t) {
  //     bins[iAxis] = t->GetListOfTasks()->GetEntries();
  //     mins[iAxis] = 0;
  //     maxs[iAxis] = t->GetListOfTasks()->GetEntries();

  //     Printf("%s %s", t->GetName(), t->ClassName());
  //     t = (AliRsnOutTask *) t->GetListOfTasks()->At(0);
  //     iAxis++;
  //   }

  //   bins[iAxis] = nResultsBins;
  //   mins[iAxis] = 0;
  //   maxs[iAxis] = nResultsBins;

  //   Long64_t nEvents = tInputData->GetNEvents();
  //   Printf("nAxis = %d", nAxis);

  //   THnSparseD *s = new THnSparseD("sparse", "Results Sparse", nAxis, bins,
  //   mins, maxs);
  //   s->Print();
}

void AliRsnOutTaskResult::FillSparse(AliRsnOutTask *task, THnSparse *s,
                                     Int_t *sparseBin, Int_t index) {
  AliRsnOutValue *v;

  AliRsnOutTask *t;
  AliRsnOutTaskBin *tBin;
  AliRsnOutTaskNorm *tNorm;
  AliRsnOutTaskFit *tFit;

  Double_t x, y, ey;
  for (Int_t i = 0; i < task->GetListOfTasks()->GetEntries(); i++) {
    t = (AliRsnOutTask *)task->GetListOfTasks()->At(i);
    tBin = dynamic_cast<AliRsnOutTaskBin *>(t);
    tNorm = dynamic_cast<AliRsnOutTaskNorm *>(t);
    tFit = dynamic_cast<AliRsnOutTaskFit *>(t);

    if (tBin) {
      v = (AliRsnOutValue *)tBin->GetValue();
      x = (v->GetMax() + v->GetMin()) / 2;
      sparseBin[index] = s->GetAxis(index)->FindBin(x);
      Printf("%s sparseBin[%d]=%d x=%f", t->GetName(), index, sparseBin[index],
             x);
      FillSparse(t, s, sparseBin, index + 1);
    } else if (tNorm) {
      Printf("Norm %s", t->GetName());
      sparseBin[index] = i + 1;
      Printf("%s sparseBin[%d]=%d x=%f", t->GetName(), index, sparseBin[index],
             x);
      FillSparse(t, s, sparseBin, index + 1);
    } else if (tFit) {
      Printf("Fit %s", t->GetName());
      sparseBin[index] = i + 1;
      Printf("%s sparseBin[%d]=%d x=%f", t->GetName(), index, sparseBin[index],
             x);
      FillSparse(t, s, sparseBin, index + 1);
    } else {
      Printf("??? %s", t->GetName());
    }

    // We Fill sparse values
    if (!t->GetListOfTasks()->GetSize()) {
      Printf("We are filling sparse %d", i);
      // Raw Bin Counting
      sparseBin[index + 1] = 1;
      tFit->GetYieldBinCounting(y, ey);
      s->SetBinContent(sparseBin, y);
      s->SetBinError(sparseBin, ey);

      // Raw Fit Function
      sparseBin[index + 1] = 2;
      tFit->GetYieldFitFunction(y, ey);
      s->SetBinContent(sparseBin, y);
      s->SetBinError(sparseBin, ey);

      // Chi2
      sparseBin[index + 1] = 6;
      s->SetBinContent(sparseBin, tFit->GetChi2());
      s->SetBinError(sparseBin, 0);

      // Ndf
      sparseBin[index + 1] = 7;
      s->SetBinContent(sparseBin, tFit->GetNdf());
      s->SetBinError(sparseBin, 0);

      // Reduced Chi2 = Chi2/Ndf
      sparseBin[index + 1] = 8;
      s->SetBinContent(sparseBin, tFit->GetReducedChi2());
      s->SetBinError(sparseBin, 0);

      // Prob
      sparseBin[index + 1] = 9;
      s->SetBinContent(sparseBin, tFit->GetProb());
      s->SetBinError(sparseBin, 0);

      sparseBin[index + 1] = 1;
      for (Int_t ii = 0; ii < 5; ii++)
        printf("%d ", sparseBin[ii]);
      Printf("binContent=%f", s->GetBinContent(sparseBin));
    }
  }
}

void AliRsnOutTaskResult::FolderFromSparse(AliRsnOutTask *task, THnSparse *s,
                                           Int_t index, TFolder *folder,
                                           Int_t indexBinFixed) {
  AliRsnOutValue *v;

  AliRsnOutTask *t;
  AliRsnOutTaskBin *tBin;
  AliRsnOutTaskNorm *tNorm;
  AliRsnOutTaskFit *tFit;
  TFolder *f = 0;

  Double_t x, y, ey;
  for (Int_t i = 0; i < task->GetListOfTasks()->GetEntries(); i++) {
    t = (AliRsnOutTask *)task->GetListOfTasks()->At(i);
    tBin = dynamic_cast<AliRsnOutTaskBin *>(t);
    tNorm = dynamic_cast<AliRsnOutTaskNorm *>(t);
    tFit = dynamic_cast<AliRsnOutTaskFit *>(t);

    if (tBin) {
      if (indexBinFixed >= 0) {
        if (indexBinFixed == i) {
          s->GetAxis(index)->SetRange(i + 1, i + 1);
        } else {
          f = folder->AddFolder(t->GetName(), t->GetTitle());
        }

        Printf("FolderFromSparse:  %s folder=%s", t->GetName(), f->GetName());
        FolderFromSparse(t, s, index + 1, f, indexBinFixed);
        // TODO Check if it will reset range
        if (indexBinFixed == i)
          s->GetAxis(index)->SetRange(i + 1, i);
      } else {
        // We need to do it only for first bin
        if (i)
          continue;
        s->GetAxis(index)->SetRange(i + 1, i);
        s->GetAxis(index)->Print();
        Printf("FolderFromSparse: (no cuts) %s", t->GetName());
        FolderFromSparse(t, s, index + 1, folder, indexBinFixed);
      }
    } else if (tNorm) {
      s->GetAxis(index)->SetRange(i + 1, i + 1);
      f = folder->AddFolder(t->GetName(), t->GetTitle());
      Printf("FolderFromSparse:  %s folder=%s", t->GetName(), f->GetName());
      FolderFromSparse(t, s, index + 1, f, indexBinFixed);
      // TODO Check if it will reset range
      s->GetAxis(index)->SetRange(i + 1, i);
    } else if (tFit) {
      s->GetAxis(index)->SetRange(i + 1, i + 1);
      f = folder->AddFolder(t->GetName(), t->GetTitle());
      Printf("FolderFromSparse:  %s folder=%s", t->GetName(), f->GetName());
      FolderFromSparse(t, s, index + 1, f, indexBinFixed);
      // TODO Check if it will reset range
      s->GetAxis(index)->SetRange(i + 1, i);
    } else {
      Printf("??? %s", t->GetName());
    }

    // We Fill sparse values
    if (!t->GetListOfTasks()->GetSize()) {
      TH1 *h;
      Printf("We are filling sparse %d", i);
      s->GetAxis(index + 1)->SetRange(1, 1);
      h = s->Projection(0);
      h->SetName("hRawBC");
      h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
      f->Add(h);
      s->GetAxis(index + 1)->SetRange(2, 2);
      h = s->Projection(0);
      h->SetName("hRawFF");
      h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
      f->Add(h);
      s->GetAxis(index + 1)->SetRange(6, 6);
      h = s->Projection(0);
      h->SetName("hChi2");
      h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
      f->Add(h);
      s->GetAxis(index + 1)->SetRange(7, 7);
      h = s->Projection(0);
      h->SetName("hNdf");
      h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
      f->Add(h);
      s->GetAxis(index + 1)->SetRange(8, 8);
      h = s->Projection(0);
      h->SetName("hReducedChi2");
      h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
      f->Add(h);
      s->GetAxis(index + 1)->SetRange(9, 9);
      h = s->Projection(0);
      h->SetName("hProb");
      h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.1);
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
