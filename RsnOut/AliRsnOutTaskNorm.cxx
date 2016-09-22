#include "AliRsnOutTaskNorm.h"
#include <AliRsnOutValue.h>
#include <TH1.h>

ClassImp(AliRsnOutTaskNorm)

  AliRsnOutTaskNorm::AliRsnOutTaskNorm(const char *name, const char *title)
  : AliRsnOutTask(name, title), fRanges(0) {}

AliRsnOutTaskNorm::~AliRsnOutTaskNorm() {}

void AliRsnOutTaskNorm::Exec(Option_t * /*option*/) {

  Printf(GetName());
  if (!fParent->GetOutput()) return;

  TH1 *hSigBgNorm = (TH1 *)fParent->GetOutput()->FindObject("hSignalBg");
  TH1 *hBgNorm = (TH1 *)fParent->GetOutput()->FindObject("hBg");
  if (hSigBgNorm && hBgNorm) {

    hSigBgNorm = (TH1 *)hSigBgNorm->Clone();
    hSigBgNorm->SetName("hSignalBg");
    hBgNorm = (TH1 *)hBgNorm->Clone();
    hBgNorm->SetName("hBgNorm");

    Double_t scale = 0.0;

    TIter next(fRanges);
    AliRsnOutValue *v;
    Double_t ss1, ss2;
    while ((v = (AliRsnOutValue *)next())) {
      Double_t s;
      s = hSigBgNorm->Integral(hSigBgNorm->FindBin((Int_t)v->GetMin()),
                               hSigBgNorm->FindBin((Int_t)v->GetMax()));
      s /= hBgNorm->Integral(hBgNorm->FindBin((Int_t)v->GetMin()),
                             hBgNorm->FindBin((Int_t)v->GetMax()));
      scale += s;
      ss1 = hSigBgNorm->Integral(hSigBgNorm->FindBin((Int_t)v->GetMin()),
                                 hSigBgNorm->FindBin((Int_t)v->GetMax()));
      ss2 = hBgNorm->Integral(hBgNorm->FindBin((Int_t)v->GetMin()),
                              hBgNorm->FindBin((Int_t)v->GetMax()));
    }

    scale /= fRanges->GetEntries();

    TH1 *hBgTmp = (TH1 *)hBgNorm->Clone();
    TH1 *hSigTmp = (TH1 *)hSigBgNorm->Clone();
    hBgTmp->Scale(scale);
    hSigTmp->Add(hBgTmp, -1);

    scale = ss1 / (ss2 - hSigTmp->GetMinimum() / scale);
    hBgNorm->Scale(scale);

    TH1 *hSig = (TH1 *)hSigBgNorm->Clone();
    hSig->SetName("hSignal");
    hSig->Add(hBgNorm, -1);

    fOutput->Add(hSigBgNorm);
    fOutput->Add(hBgNorm);
    fOutput->Add(hSig);
  }

  //	TH1 *hSigMCGen = (TH1*)
  // fParent->GetOutput()->FindObject("hSignalMCGen");
  //	TH1 *hSigMCRec = (TH1*)
  // fParent->GetOutput()->FindObject("hSignalMCRec");
  //
  //	// Check if has to be Cloned
  //	fOutput->Add(hSigMCGen);
  //	fOutput->Add(hSigMCRec);
}

void AliRsnOutTaskNorm::AddRange(AliRsnOutValue *range) {
  if (!range) return;
  if (!fRanges) {
    fRanges = new TList();
    fName = "norm";
  }
  fName += TString::Format("[%.2f,%.2f]", range->GetMin(), range->GetMax());
  fRanges->Add(range);
}
