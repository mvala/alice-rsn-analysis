#include <TAxis.h>
#include <TDirectory.h>
#include <TEfficiency.h>
#include <TGraphAsymmErrors.h>
#include <TH1.h>
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
    TH1 *fSignalMCGen = (TH1 *)fOutput->FindObject("hSignalMCGen");
    TH1 *fSignalMCRec = (TH1 *)fOutput->FindObject("hSignalMCRec");

    if (fSignalMCGen && fSignalMCRec) {
      Print();
      fSignalMCGen->Print();
      fSignalMCRec->Print();
      AliRsnOutTask *parent = GetParent();
      AliRsnOutTaskBin *tBinVariation = this;
      AliRsnOutTask *tBinMgr = dynamic_cast<AliRsnOutTaskBinMgr *>(parent);
      while (!tBinMgr && parent) {
        tBinVariation = (AliRsnOutTaskBin *)parent;
        parent = parent->GetParent();
        tBinMgr = dynamic_cast<AliRsnOutTaskBinMgr *>(parent);
      }
      if (tBinVariation) {
        TEfficiency *eff = tBinVariation->GetEfficiency();
        Printf("XXXX %s", eff->Class_Name());
        if (!eff) {
          TList *listOfTasks = tBinVariation->GetListOfTasks();
          const Int_t nVariableBins = listOfTasks->GetEntries();
          Double_t varBins[nVariableBins + 1];
          Int_t iBin;
          AliRsnOutValue *v;
          AliRsnOutTaskBin *tBin;
          Printf("Creating TEfficiency %s %d", tBinVariation->GetName(),
                 listOfTasks->GetEntries());
          for (iBin = 0; iBin < listOfTasks->GetEntries(); iBin++) {
            tBin = (AliRsnOutTaskBin *)listOfTasks->At(iBin);
            v = (AliRsnOutValue *)tBin->GetValue();
            varBins[iBin] = v->GetMin();
            printf("x=%.2f ", varBins[iBin]);
            if (iBin == listOfTasks->GetEntries() - 1) {
              varBins[iBin + 1] = v->GetMax();
              Printf("x=%.2f", varBins[iBin + 1]);
            }
          }
          tBinVariation->SetEfficiency(new TEfficiency(
              "eff", "MC Efficiency;x;#epsilon", nVariableBins, varBins));
          eff = tBinVariation->GetEfficiency();
          // eff->SetDirectory(gDirectory);
          // tBinVariation->GetOutput()->Add(new TObject());
        }

        if (eff) {
          Printf("Filling eff bin %d", fID + 1);
          eff->SetTotalEvents(fID + 1, fSignalMCGen->Integral());
          eff->SetPassedEvents(fID + 1, fSignalMCRec->Integral());
          // eff->CreateGraph()->Print();
          tBinVariation->Print();
          tBinVariation->GetOutput()->Print();
        }
      }
      return;
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

  if (fEfficiency)
    fOutput->Add(fEfficiency->CreateGraph());
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
