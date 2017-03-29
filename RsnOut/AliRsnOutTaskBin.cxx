#include <TAxis.h>
#include <TDirectory.h>
#include <TEfficiency.h>
#include <TGraphAsymmErrors.h>
#include <TH2.h>
#include <THnSparse.h>

#include "AliRsnOutTaskBin.h"
#include "AliRsnOutTaskBinMgr.h"
#include "AliRsnOutTaskInput.h"

ClassImp(AliRsnOutTaskBin);

AliRsnOutTaskBin::AliRsnOutTaskBin(const char *name, const char *title,
                                   Bool_t isCutsOnly)
    : AliRsnOutTask(name, title), fValue(), fCuts(0), fCutsOnly(isCutsOnly),
      fInputTask(0), fEfficiency(0) {}

AliRsnOutTaskBin::~AliRsnOutTaskBin() { SafeDelete(fCuts); }

void AliRsnOutTaskBin::Exec(Option_t * /*option*/) {

  if (!fInputTask) {
    AliRsnOutTask *parent = GetParent();
    fInputTask = dynamic_cast<AliRsnOutTaskInput *>(parent);
    while (!fInputTask && parent) {
      parent = parent->GetParent();
      fInputTask = dynamic_cast<AliRsnOutTaskInput *>(parent);
    }
    if (!fInputTask)
      return;
  }

  THnSparse *sigBg = fInputTask->GetSigBg();
  THnSparse *bg = fInputTask->GetBg();
  THnSparse *mcRec = fInputTask->GetMCRec();
  THnSparse *mcGen = fInputTask->GetMCGen();

  if (sigBg) {
    ApplyCuts(sigBg, bg);

    if (!fCutsOnly) {
      TH1 *hSigBg = (TH1 *)sigBg->Projection(fValue.GetId())->Clone();
      hSigBg->SetName("hSignalBg");
      fOutput->Add(hSigBg);

      TH1 *hBg = (TH1 *)bg->Projection(fValue.GetId())->Clone();
      hBg->SetName("hBg");
      fOutput->Add(hBg);
    }
  }

  if (mcGen && mcRec) {
    ApplyCuts(mcGen, mcRec);
    if (!fCutsOnly) {
      TH1 *hMCGen = (TH1 *)mcGen->Projection(fValue.GetId())->Clone();
      hMCGen->SetName("hSignalMCGen");
      fOutput->Add(hMCGen);

      TH1 *hMCRec = (TH1 *)mcRec->Projection(fValue.GetId())->Clone();
      hMCRec->SetName("hSignalMCRec");
      fOutput->Add(hMCRec);
      // TEfficiency *fMCEfficiency = 0;
      // if (!fMCEfficiency)
      //   fMCEfficiency = new TEfficiency("eff", "MC Efficiency;x;#epsilon",
      //                                   nVariableBins, varBins);
      // fMCEfficiency->SetTotalEvents(iBin + 1, mcGen->Integral());
      // fMCEfficiency->SetPassedEvents(iBin + 1, mcRec->Integral());
    }
  }

  // Printf("%s", GetName());
}

void AliRsnOutTaskBin::ExecPost(Option_t * /*option*/) {

  AliRsnOutTaskBin *b = dynamic_cast<AliRsnOutTaskBin *>(fTasks->At(0));
  if (!b) {
    return;
  }

  AliRsnOutTask *tBinMgr = dynamic_cast<AliRsnOutTaskBinMgr *>(fParent);
  if (tBinMgr) {
    Int_t iBin, jBin;
    AliRsnOutTaskBin *tBin, *tBin2;
    AliRsnOutValue *v, *v2;

    TArrayI nVarBinsArr;
    Int_t nDim = 0;

    tBin = dynamic_cast<AliRsnOutTaskBin *>(fTasks->At(0));
    if (tBin) {
      nDim++;
      nVarBinsArr.Set(1);
      nVarBinsArr.SetAt(tBin->GetListOfTasks()->GetSize(), 0);
      tBin2 = dynamic_cast<AliRsnOutTaskBin *>(tBin->GetListOfTasks()->At(0));
      if (tBin2) {
        nDim++;
        nVarBinsArr.Set(2);
        nVarBinsArr.SetAt(tBin2->GetListOfTasks()->GetSize(), 1);
      }
    }

    Int_t nVarBins[nVarBinsArr.GetSize()];
    nVarBins[0] = -1;
    nVarBins[1] = -1;
    if (nVarBinsArr.GetSize() > 0)
      nVarBins[0] = (Int_t)nVarBinsArr.GetAt(0);
    if (nVarBinsArr.GetSize() > 1)
      nVarBins[1] = (Int_t)nVarBinsArr.GetAt(1);

    Int_t maxBins = TMath::Max(nVarBins[0], nVarBins[1]);
    Double_t varBins[nDim][maxBins + 1];

    if (tBin) {

      // Get Variable binings
      for (iBin = 0; iBin < fTasks->GetEntries(); iBin++) {
        tBin = (AliRsnOutTaskBin *)fTasks->At(iBin);
        if (!tBin)
          continue;
        v = (AliRsnOutValue *)tBin->GetValue();
        if (!v)
          continue;

        varBins[0][iBin] = v->GetMin();
        printf("x=%.2f ", varBins[0][iBin]);
        if (iBin == fTasks->GetEntries() - 1) {
          varBins[0][iBin + 1] = v->GetMax();
          Printf("x=%.2f", varBins[0][iBin + 1]);
        }

        if (!iBin) {
          TList *l = tBin->GetListOfTasks();
          // 2D case
          for (jBin = 0; jBin < l->GetEntries(); jBin++) {
            tBin2 = (AliRsnOutTaskBin *)l->At(jBin);
            if (!tBin2)
              continue;
            v2 = (AliRsnOutValue *)tBin2->GetValue();
            if (!v2)
              continue;

            varBins[1][jBin] = v2->GetMin();
            printf("x=%.2f ", varBins[1][jBin]);
            if (jBin == l->GetEntries() - 1) {
              varBins[1][jBin + 1] = v2->GetMax();
              Printf("x=%.2f", varBins[1][jBin + 1]);
            }
          }
        }
      }

      // Fill
      for (iBin = 0; iBin < fTasks->GetEntries(); iBin++) {
        tBin = (AliRsnOutTaskBin *)fTasks->At(iBin);
        if (!tBin)
          continue;
        v = (AliRsnOutValue *)tBin->GetValue();
        if (!v)
          continue;
        TList *l = tBin->GetListOfTasks();
        tBin2 = dynamic_cast<AliRsnOutTaskBin *>(l->At(0));
        if (tBin2) {
          // 2D case
          for (jBin = 0; jBin < l->GetEntries(); jBin++) {
            tBin2 = (AliRsnOutTaskBin *)l->At(jBin);
            if (!tBin2)
              continue;
            v2 = (AliRsnOutValue *)tBin2->GetValue();
            if (!v2)
              continue;
          }
          // varBins[0][iBin] = v->GetMin();
          // printf("x=%.2f ", varBins[0][iBin]);
          // if (iBin == listOfTasks->GetEntries() - 1) {
          //   varBins[0][iBin + 1] = v->GetMax();
          //   Printf("x=%.2f", varBins[0][iBin + 1]);
          // }
          TH1 *hEffRec2D = (TH1 *)fOutput->FindObject("hEffRec2D");
          TH1 *hEffGen2D = (TH1 *)fOutput->FindObject("hEffGen2D");
          if (!hEffRec2D) {
            hEffRec2D = new TH2D("hEffRec0", "REC", nVarBins[0], varBins[0],
                                 nVarBins[1], varBins[1]);
          }
          if (!hEffGen2D) {
            hEffGen2D = new TH2D("hEffGen0", "GEN", nVarBins[0], varBins[0],
                                 nVarBins[1], varBins[1]);
            hEffGen2D->Print("all");
          }

        } else {
          // 1D case
          TH1 *hEffRec = (TH1 *)fOutput->FindObject("hEffRec");
          TH1 *hEffGen = (TH1 *)fOutput->FindObject("hEffGen");
          if (!hEffRec) {
            hEffRec = new TH1D("hEffRec", "REC", nVarBins[0], varBins[0]);
          }
          if (!hEffGen) {
            hEffGen = new TH1D("hEffGen", "GEN", nVarBins[0], varBins[0]);
            hEffGen->Print("all");
          }
        }
      }
    }
  }

  // TH1 *fSignalMCGen = (TH1 *)fOutput->FindObject("hSignalMCGen");
  // TH1 *fSignalMCRec = (TH1 *)fOutput->FindObject("hSignalMCRec");

  // if (fSignalMCGen && fSignalMCRec) {
  //   Print();
  //   fSignalMCGen->Print();
  //   fSignalMCRec->Print();
  //   AliRsnOutTask *parent = GetParent();
  //   AliRsnOutTaskBin *tBinVariation = this;
  //   AliRsnOutTask *tBinMgr = dynamic_cast<AliRsnOutTaskBinMgr *>(parent);
  //   while (!tBinMgr && parent) {
  //     tBinVariation = (AliRsnOutTaskBin *)parent;
  //     parent = parent->GetParent();
  //     tBinMgr = dynamic_cast<AliRsnOutTaskBinMgr *>(parent);
  //   }

  //   Int_t effdim = 1;
  //   if (tBinVariation) {
  //     TEfficiency *eff = tBinVariation->GetEfficiency();
  //     TList *lOutput = tBinVariation->GetOutput();
  //     TH1 *hEffRec = (TH1 *)lOutput->FindObject("hEffRec");
  //     TH1 *hEffGen = (TH1 *)lOutput->FindObject("hEffGen");

  //     Printf("XXXX %s", eff->Class_Name());
  //     if (!hEffRec) {
  //       TList *listOfTasks = tBinVariation->GetListOfTasks();
  //       Int_t nVariableBins[2];
  //       nVariableBins[0] = listOfTasks->GetEntries();

  //       TString bvName = tBinVariation->GetName();
  //       if (bvName.Contains("_vs_")) {
  //         effdim = 2;
  //         AliRsnOutTaskBin *t = (AliRsnOutTaskBin *)listOfTasks->At(0);
  //         nVariableBins[1] = t->GetListOfTasks()->GetEntries();
  //       }
  //       Int_t maxBins = TMath::Max(nVariableBins[0], nVariableBins[1]);
  //       Double_t varBins[2][maxBins + 1];
  //       Int_t iBin;
  //       AliRsnOutValue *v;
  //       AliRsnOutTaskBin *tBin;
  //       Printf("Creating TEfficiency %s %d", tBinVariation->GetName(),
  //              listOfTasks->GetEntries());
  //       for (iBin = 0; iBin < listOfTasks->GetEntries(); iBin++) {
  //         tBin = (AliRsnOutTaskBin *)listOfTasks->At(iBin);
  //         v = (AliRsnOutValue *)tBin->GetValue();
  //         varBins[0][iBin] = v->GetMin();
  //         printf("x=%.2f ", varBins[0][iBin]);
  //         if (iBin == listOfTasks->GetEntries() - 1) {
  //           varBins[0][iBin + 1] = v->GetMax();
  //           Printf("x=%.2f", varBins[0][iBin + 1]);
  //         }
  //       }
  //       listOfTasks = tBin->GetListOfTasks();
  //       nVariableBins[1] = listOfTasks->GetEntries();
  //       for (iBin = 0; iBin < listOfTasks->GetEntries(); iBin++) {
  //         tBin = (AliRsnOutTaskBin *)listOfTasks->At(iBin);
  //         v = (AliRsnOutValue *)tBin->GetValue();
  //         varBins[1][iBin] = v->GetMin();
  //         printf("x=%.2f ", varBins[1][iBin]);
  //         if (iBin == listOfTasks->GetEntries() - 1) {
  //           varBins[1][iBin + 1] = v->GetMax();
  //           Printf("x=%.2f", varBins[1][iBin + 1]);
  //         }
  //       }

  //       if (effdim == 1) {
  //         hEffRec = new TH1D("hEffRec", "REC", nVariableBins[0],
  //         varBins[0]);
  //         hEffGen = new TH1D("hEffGen", "GEN", nVariableBins[0],
  //         varBins[0]);
  //         tBinVariation->GetOutput()->Add(hEffRec);
  //         tBinVariation->GetOutput()->Add(hEffGen);
  //         // eff = new TEfficiency(hEffRec, hEffGen);
  //         // tBinVariation->SetEfficiency(eff);
  //       } else if (effdim == 2) {
  //         eff = new TEfficiency("eff", "MC Efficiency;x;#epsilon",
  //                               nVariableBins[0], varBins[0],
  //                               nVariableBins[1], varBins[1]);
  //         tBinVariation->SetEfficiency(eff);
  //       }
  //     }

  //     if (hEffRec && hEffGen) {

  //       if (effdim == 1) {
  //         Printf("Filling eff bin %d", fID + 1);
  //         hEffRec->SetBinContent(fID + 1, fSignalMCRec->Integral());
  //         hEffGen->SetBinContent(fID + 1, fSignalMCGen->Integral());
  //         // eff->CreateGraph()->Print();
  //         tBinVariation->Print();
  //         tBinVariation->GetOutput()->Print();
  //       } else if (effdim == 2) {
  //         Printf("TODO 2D Filling eff bin %d", fID + 1);
  //       }
  //     }
  //   }
  //   return;
  // }

  THnSparse *sigBg = fInputTask->GetSigBg();
  THnSparse *bg = fInputTask->GetBg();
  THnSparse *mcRec = fInputTask->GetMCRec();
  THnSparse *mcGen = fInputTask->GetMCGen();
  fTasks->Print();
  for (Int_t i = 0; i < fTasks->GetSize(); i++) {
    b = (AliRsnOutTaskBin *)fTasks->At(i);
    Printf("Reseting sparses axis in %s", b->GetName());
    TList *l = b->GetListOfCuts();
    if (!l)
      continue;
    TIter next(l);
    AliRsnOutValue *v;
    while ((v = (AliRsnOutValue *)next())) {
      if (sigBg)
        sigBg->GetAxis(v->GetId())->SetRange(0, 0);
      if (bg)
        bg->GetAxis(v->GetId())->SetRange(0, 0);
      if (mcRec)
        mcRec->GetAxis(v->GetId())->SetRange(0, 0);
      if (mcGen)
        mcGen->GetAxis(v->GetId())->SetRange(0, 0);
    }
  }
}

// if (fEfficiency) {
//   Int_t dim = fEfficiency->GetDimension();
//   if (dim == 1) {
//     TGraphAsymmErrors *graph = fEfficiency->CreateGraph();
//     if (graph)
//       fOutput->Add(graph);
//   } else if (dim == 2) {
//     TH2 *hist2d = fEfficiency->CreateHistogram();
//     if (hist2d)
//       fOutput->Add(hist2d);
//   }
// }
// }

void AliRsnOutTaskBin::AddCut(AliRsnOutValue *cut) {
  if (!cut)
    return;
  if (!fCuts) {
    fCuts = new TList();
    fName = TString::Format("bin[%d,%d]", (Int_t)cut->GetMin(),
                            (Int_t)cut->GetMax());
  }
  fCuts->Add(cut);
}

void AliRsnOutTaskBin::ApplyCuts(THnSparse *one, THnSparse *two,
                                 Bool_t updateOnly) {

  if (fCuts) {

    TIter next(fCuts);
    AliRsnOutValue *v;
    fName = "";
    Double_t minVal, maxVal;
    while ((v = (AliRsnOutValue *)next())) {
      if (!updateOnly) {
        one->GetAxis(v->GetId())
            ->SetRange((Int_t)v->GetMin(), (Int_t)v->GetMax());
        if (two)
          two->GetAxis(v->GetId())
              ->SetRange((Int_t)v->GetMin(), (Int_t)v->GetMax());
      }
      minVal = one->GetAxis(v->GetId())->GetBinLowEdge((Int_t)v->GetMin());
      maxVal = one->GetAxis(v->GetId())->GetBinUpEdge((Int_t)v->GetMax());
      fName += TString::Format(
          "%s[%.2f,%.2f]", one->GetAxis(v->GetId())->GetName(), minVal, maxVal);
      fValue.SetMin(minVal);
      fValue.SetMax(maxVal);
    }
  } else {
    // fName = TString::Format("%s", one->GetAxis(fValue.GetId())->GetName());
  }
}

void AliRsnOutTaskBin::UpdateTask() {
  if (!fInputTask) {
    AliRsnOutTask *parent = GetParent();
    fInputTask = dynamic_cast<AliRsnOutTaskInput *>(parent);
    while (!fInputTask && parent) {
      parent = parent->GetParent();
      fInputTask = dynamic_cast<AliRsnOutTaskInput *>(parent);
    }
    if (!fInputTask)
      return;
  }

  if (fInputTask->GetSigBg())
    ApplyCuts(fInputTask->GetSigBg(), 0, kTRUE);
  else if (fInputTask->GetMCRec())
    ApplyCuts(fInputTask->GetMCRec(), 0, kTRUE);
}
