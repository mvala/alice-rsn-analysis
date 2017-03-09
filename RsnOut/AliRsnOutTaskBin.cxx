#include <TAxis.h>
#include <TH1.h>
#include <THnSparse.h>

#include "AliRsnOutTaskBin.h"
#include "AliRsnOutTaskBinMgr.h"
#include "AliRsnOutTaskInput.h"

ClassImp(AliRsnOutTaskBin)

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
      if (parent) {
        Printf("AliRsnOutTaskBin::while %s input=%p", parent->GetName(),
               fInputTask);
      }
      parent = parent->GetParent();
      fInputTask = dynamic_cast<AliRsnOutTaskInput *>(parent);
    }
    Printf("AliRsnOutTaskBin: %p", fInputTask);
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

  Printf("%s", GetName());
}

void AliRsnOutTaskBin::ExecPost(Option_t * /*option*/) {

  AliRsnOutTaskBin *b = dynamic_cast<AliRsnOutTaskBin *>(fTasks->At(0));
  if (!b)
    return;

  THnSparse *sigBg = fInputTask->GetSigBg();
  THnSparse *bg = fInputTask->GetBg();
  THnSparse *mcRec = fInputTask->GetMCRec();
  THnSparse *mcGen = fInputTask->GetMCGen();

  for (Int_t i = 0; i < fTasks->GetSize(); i++) {
    b = (AliRsnOutTaskBin *)fTasks->At(i);
    // Printf("Reseting sparses axis in %s", b->GetName());
    TIter next(b->GetListOfCuts());
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

void AliRsnOutTaskBin::ApplyCuts(THnSparse *one, THnSparse *two) {

  if (!fCuts)
    return;

  TIter next(fCuts);
  AliRsnOutValue *v;
  fName = "";
  Double_t minVal, maxVal;
  while ((v = (AliRsnOutValue *)next())) {
    one->GetAxis(v->GetId())->SetRange((Int_t)v->GetMin(), (Int_t)v->GetMax());
    if (two)
      two->GetAxis(v->GetId())
          ->SetRange((Int_t)v->GetMin(), (Int_t)v->GetMax());
    minVal = one->GetAxis(v->GetId())->GetBinLowEdge((Int_t)v->GetMin());
    maxVal = one->GetAxis(v->GetId())->GetBinUpEdge((Int_t)v->GetMax());
    fName += TString::Format(
        "%s[%.2f,%.2f]", one->GetAxis(v->GetId())->GetName(), minVal, maxVal);
    fValue.SetMin(minVal);
    fValue.SetMax(maxVal);
  }
}
