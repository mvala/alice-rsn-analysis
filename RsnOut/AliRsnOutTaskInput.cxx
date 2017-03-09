#include "AliRsnOutTaskInput.h"
#include "AliRsnOutTaskBin.h"
#include "AliRsnOutTaskFit.h"
#include "AliRsnOutTaskNorm.h"
#include <TFile.h>
#include <TFolder.h>
#include <TGraphAsymmErrors.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <THnSparse.h>
#include <TList.h>
#include <TROOT.h>
#include <TStyle.h>

ClassImp(AliRsnOutTaskInput)

  AliRsnOutTaskInput::AliRsnOutTaskInput(const char *name, const char *title)
  : AliRsnOutTask(name, title),
    fFileName(),
    fListName(),
    fSigBgName(),
    fBgName(),
    fMCRecName(),
    fMCGenName(),
    fMCEff(0),
    fEventStat(0),
    fEffOnly(0),
    fFile(0),
    fList(0),
    fSigBg(0),
    fBg(0),
    fMCRec(0),
    fMCGen(0) {}

AliRsnOutTaskInput::~AliRsnOutTaskInput() { Clear(); }

void AliRsnOutTaskInput::Exec(Option_t * /*option*/) {

  Printf("%s",GetName());
  if (!fFile) {
    Printf("Opening file %s ...",fFileName.Data());
    fFile = TFile::Open(fFileName.Data());
    if (!fFile) return;
    fList = (TList *)fFile->Get(fListName.Data());
    if (!fList) return;

    TList *extra =
      (TList *)fFile->Get(TString::Format("%s_extra", fListName.Data()).Data());
    TH1 *hEvents = (TH1 *)extra->FindObject("hEventStat");
    if (hEvents) fEventStat = (TH1 *)hEvents->Clone();

    if (!fEffOnly) {
      fSigBg = (THnSparse *)fList->FindObject(fSigBgName.Data());
      fBg = (THnSparse *)fList->FindObject(fBgName.Data());
    }
    fMCRec = (THnSparse *)fList->FindObject(fMCRecName.Data());
    fMCGen = (THnSparse *)fList->FindObject(fMCGenName.Data());
  }
}

void AliRsnOutTaskInput::ExecPost(Option_t * /*option*/) {

  if (!fOutput) {
    fOutput = new TList();
    fOutput->SetOwner();
  }

  if (fEventStat) fOutput->Add(fEventStat);

  CalculateEfficiency();
  Clear();
}

void AliRsnOutTaskInput::Clear(Option_t * /*opt*/) {
  if (fList) SafeDelete(fList);
  if (fFile) SafeDelete(fFile);
  fSigBg = 0;
  fBg = 0;
  fMCRec = 0;
  fMCGen = 0;
}

void AliRsnOutTaskInput::CalculateEfficiency() {

  AliRsnOutTaskBin *tBin;
  AliRsnOutValue *v;

  const Int_t nVariableBins = fTasks->GetEntries();
  Double_t varBins[nVariableBins + 1];
  Int_t iBin;
  for (iBin = 0; iBin < fTasks->GetEntries(); ++iBin) {
    tBin = (AliRsnOutTaskBin *)fTasks->At(iBin);
    v = (AliRsnOutValue *)tBin->GetValue();
    varBins[iBin] = v->GetMin();
    if (iBin == fTasks->GetEntries() - 1) varBins[iBin + 1] = v->GetMax();
  }

  TH1D *hMCRec = 0;
  TH1D *hMCGen = 0;

  TEfficiency *fMCEfficiency = 0;
  Double_t tmp;
  for (Int_t iBin = 0; iBin < fTasks->GetEntries(); ++iBin) {
    tBin = (AliRsnOutTaskBin *)fTasks->At(iBin);
    v = (AliRsnOutValue *)tBin->GetValue();
    TH1 *mcGen = (TH1 *)tBin->GetOutput()->FindObject("hSignalMCGen");
    TH1 *mcRec = (TH1 *)tBin->GetOutput()->FindObject("hSignalMCRec");
    if (mcGen && mcRec) {
      if (!fMCEfficiency)
        fMCEfficiency = new TEfficiency("eff", "MC Efficiency;x;#epsilon",
                                        nVariableBins, varBins);
      fMCEfficiency->SetTotalEvents(iBin + 1, mcGen->Integral());
      fMCEfficiency->SetPassedEvents(iBin + 1, mcRec->Integral());
      if (!hMCGen) {
        hMCGen = new TH1D("hMCGen", "MC Gen", nVariableBins, varBins);
      }
      if (!hMCRec) {
        hMCRec = new TH1D("hMCRec", "MC Rec", nVariableBins, varBins);
      }

      tmp = mcGen->Integral();
      // check if this is correct (iBin + 1, iBin + 1)
      hMCGen->SetBinContent(iBin + 1, iBin + 1, tmp);
      hMCGen->SetBinError(iBin + 1, TMath::Sqrt(tmp));

      tmp = mcRec->Integral();
      hMCRec->SetBinContent(iBin + 1, tmp);
      hMCRec->SetBinError(iBin + 1, TMath::Sqrt(tmp));
    }
  }

  if (fMCEfficiency) {
    fMCEff = fMCEfficiency->CreateGraph();
    fOutput->Add(fMCEff);
  }

  if (hMCRec) {
    hMCRec->GetYaxis()->SetRangeUser(0, hMCRec->GetMaximum() * 1.1);
    fOutput->Add(hMCRec);
  }
  if (hMCGen) {
    hMCGen->GetYaxis()->SetRangeUser(0, hMCGen->GetMaximum() * 1.1);
    fOutput->Add(hMCGen);
  }
}

Long64_t AliRsnOutTaskInput::GetNEvents(Int_t bin) const {
  if (!fEventStat) return 0;
  return fEventStat->GetBinContent(bin);
}
