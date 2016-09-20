#include <TAxis.h>
#include <TH1.h>
#include <THnSparse.h>

#include "AliRsnOutTaskBin.h"
#include "AliRsnOutTaskInput.h"

ClassImp(AliRsnOutTaskBin)

    AliRsnOutTaskBin::AliRsnOutTaskBin(const char *name, const char *title)
    : AliRsnOutTask(name, title), fValue(), fCuts(0) {}

AliRsnOutTaskBin::~AliRsnOutTaskBin() { SafeDelete(fCuts); }

void AliRsnOutTaskBin::Exec(Option_t * /*option*/) {

  AliRsnOutTaskInput *input = dynamic_cast<AliRsnOutTaskInput *>(GetParent());
  if (!input)
    return;

  THnSparse *sigBg = input->GetSigBg();
  THnSparse *bg = input->GetBg();
  THnSparse *mcRec = input->GetMCRec();
  THnSparse *mcGen = input->GetMCGen();

  if (sigBg) {
    ApplyCuts(sigBg, bg);

    TH1 *hSigBg = (TH1 *)sigBg->Projection(fValue.GetId())->Clone();
    hSigBg->SetName("hSignalBg");
    fOutput->Add(hSigBg);

    TH1 *hBg = (TH1 *)bg->Projection(fValue.GetId())->Clone();
    hBg->SetName("hBg");
    fOutput->Add(hBg);
  }

  if (mcGen && mcRec) {
    ApplyCuts(mcGen, mcRec);

    TH1 *hMCGen = (TH1 *)mcGen->Projection(fValue.GetId())->Clone();
    hMCGen->SetName("hSignalMCGen");
    fOutput->Add(hMCGen);

    TH1 *hMCRec = (TH1 *)mcRec->Projection(fValue.GetId())->Clone();
    hMCRec->SetName("hSignalMCRec");
    fOutput->Add(hMCRec);
  }

  Printf("%s", GetName());
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

  // TODO reset all cuts with Range(0,0)

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
