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
      fInputTask(0) {}

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
      nVarBinsArr.SetAt(fTasks->GetSize(), 0);
      tBin2 = dynamic_cast<AliRsnOutTaskBin *>(tBin->GetListOfTasks()->At(0));
      if (tBin2) {
        nDim++;
        nVarBinsArr.Set(2);
        nVarBinsArr.SetAt(tBin->GetListOfTasks()->GetSize(), 1);
      }
    }

    if (nDim > 1)
      Printf("AAA hEffGen0 %d %d", (Int_t)nVarBinsArr.GetAt(0),
             (Int_t)nVarBinsArr.GetAt(1));
    else
      Printf("AAA hEffGen0 %d ndim=%d", (Int_t)nVarBinsArr.GetAt(0), nDim);
    Int_t nVarBins[nDim];
    nVarBins[0] = -1;
    nVarBins[1] = -1;
    if (nDim > 0)
      nVarBins[0] = (Int_t)nVarBinsArr.GetAt(0);
    if (nDim > 1)
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
        Printf("[0] x=%.2f ", varBins[0][iBin]);
        if (iBin == fTasks->GetEntries() - 1) {
          varBins[0][iBin + 1] = v->GetMax();
          Printf("[0] x=%.2f", varBins[0][iBin + 1]);
        }

        if (nDim == 2) {
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
            Printf("[1] x=%.2f ", varBins[1][jBin]);
            if (jBin == l->GetEntries() - 1) {
              varBins[1][jBin + 1] = v2->GetMax();
              Printf("[1] x=%.2f", varBins[1][jBin + 1]);
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
          TH2 *hEffGen2D = (TH2 *)fOutput->FindObject("hEffGen2D");
          TH2 *hEffRec2D = (TH2 *)fOutput->FindObject("hEffRec2D");

          if (!hEffGen2D) {
            hEffGen2D = new TH2D("hEffGen2D", "GEN", nVarBins[0], varBins[0],
                                 nVarBins[1], varBins[1]);
            // hEffGen2D->GetXaxis()->Set(nVarBins[0], varBins[0]);
            // hEffGen2D->GetYaxis()->Set(nVarBins[1], varBins[1]);
            hEffGen2D->Print("all");
            fOutput->Add(hEffGen2D);
          }
          if (!hEffRec2D) {
            hEffRec2D = new TH2D("hEffRec2D", "REC", nVarBins[0], varBins[0],
                                 nVarBins[1], varBins[1]);
            // hEffRec2D->GetXaxis()->Set(nVarBins[0], varBins[0]);
            // hEffRec2D->GetYaxis()->Set(nVarBins[1], varBins[1]);
            hEffGen2D->Print("all");
            fOutput->Add(hEffRec2D);
          }

          // 2D case
          for (jBin = 0; jBin < l->GetEntries(); jBin++) {
            tBin2 = (AliRsnOutTaskBin *)l->At(jBin);
            if (!tBin2)
              continue;
            v2 = (AliRsnOutValue *)tBin2->GetValue();
            if (!v2)
              continue;
            // Fill

            TH1 *fSignalMCGen =
                (TH1 *)tBin2->GetOutput()->FindObject("hSignalMCGen");
            TH1 *fSignalMCRec =
                (TH1 *)tBin2->GetOutput()->FindObject("hSignalMCRec");
            if (fSignalMCGen && fSignalMCRec) {
              Double_t integral = fSignalMCGen->Integral();
              Printf("Filling eff Hist %d %d gen=%f", iBin + 1, jBin + 1,
                     integral);
              hEffGen2D->Print("all");

              hEffGen2D->SetBinContent(iBin + 1, jBin + 1, integral);
              hEffGen2D->SetBinError(iBin + 1, jBin + 1, TMath::Sqrt(integral));
              integral = fSignalMCRec->Integral();
              hEffRec2D->SetBinContent(iBin + 1, jBin + 1, integral);
              hEffRec2D->SetBinError(iBin + 1, jBin + 1, TMath::Sqrt(integral));
            }
          }

        } else {
          // 1D case
          TH1 *hEffGen = (TH1 *)fOutput->FindObject("hEffGen");
          TH1 *hEffRec = (TH1 *)fOutput->FindObject("hEffRec");
          if (!hEffGen) {
            hEffGen = new TH1D("hEffGen", "GEN", nVarBins[0], varBins[0]);
            fOutput->Add(hEffGen);
          }
          if (!hEffRec) {
            hEffRec = new TH1D("hEffRec", "REC", nVarBins[0], varBins[0]);
            fOutput->Add(hEffRec);
          }

          Printf("Filling eff Hist %d", iBin);
          TH1 *fSignalMCGen =
              (TH1 *)tBin->GetOutput()->FindObject("hSignalMCGen");
          TH1 *fSignalMCRec =
              (TH1 *)tBin->GetOutput()->FindObject("hSignalMCRec");
          if (fSignalMCGen && fSignalMCRec) {
            Double_t integral = fSignalMCGen->Integral();
            hEffGen->SetBinContent(iBin + 1, integral);
            hEffGen->SetBinError(iBin + 1, TMath::Sqrt(integral));
            integral = fSignalMCRec->Integral();
            hEffRec->SetBinContent(iBin + 1, integral);
            hEffRec->SetBinError(iBin + 1, TMath::Sqrt(integral));
          }
        }
      }
    }
    // Here we finish eff
    if (nDim == 1) {
      TH1 *hEffGen = (TH1 *)fOutput->FindObject("hEffGen");
      TH1 *hEffRec = (TH1 *)fOutput->FindObject("hEffRec");

      if (hEffGen && hEffRec &&
          TEfficiency::CheckConsistency(*hEffRec, *hEffGen)) {
        hEffGen->Print("all");
        hEffRec->Print("all");
        TEfficiency *eff = new TEfficiency(*hEffRec, *hEffGen);
        fOutput->Add(eff->CreateGraph());
      }
    } else if (nDim == 2) {
      TH2 *hEffGen = (TH2 *)fOutput->FindObject("hEffGen2D");
      TH2 *hEffRec = (TH2 *)fOutput->FindObject("hEffRec2D");
      hEffGen->Print();
      if (hEffGen && hEffRec &&
          TEfficiency::CheckConsistency(*hEffRec, *hEffGen)) {
        hEffGen->Print("all");
        hEffRec->Print("all");
        TEfficiency *eff = new TEfficiency(*hEffRec, *hEffGen);
        fOutput->Add(eff->CreateHistogram());
      }
    }
  }

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
    fTitle = "";
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
      fTitle +=
          TString::Format("%s[%d,%d]", one->GetAxis(v->GetId())->GetName(),
                          (Int_t)v->GetMin(), (Int_t)v->GetMax());
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
