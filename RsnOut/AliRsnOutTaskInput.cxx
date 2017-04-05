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

ClassImp(AliRsnOutTaskInput);

AliRsnOutTaskInput::AliRsnOutTaskInput(const char *name, const char *title)
    : AliRsnOutTask(name, title), fFileName(), fListName(), fSigBgName(),
      fBgName(), fMCRecName(), fMCGenName(), fEventStat(0), fEffOnly(0),
      fFile(0), fList(0), fSigBg(0), fBg(0), fMCRec(0), fMCGen(0) {}

AliRsnOutTaskInput::~AliRsnOutTaskInput() { Clear(); }

void AliRsnOutTaskInput::Clear(Option_t * /*opt*/) {
  SafeDelete(fList);
  SafeDelete(fFile);

  fSigBg = 0;
  fBg = 0;
  fMCRec = 0;
  fMCGen = 0;
}

void AliRsnOutTaskInput::Exec(Option_t * /*option*/) {

  // Printf("%s", GetName());
  UpdateTask();
}

void AliRsnOutTaskInput::ExecPost(Option_t * /*option*/) {

  if (fEventStat)
    fOutput->Add(fEventStat);

  // Clear();
}
void AliRsnOutTaskInput::UpdateTask() {

  if (!fFile) {
    Printf("Opening file %s ...", fFileName.Data());
    fFile = TFile::Open(fFileName.Data());
    if (!fFile)
      return;
    fList = (TList *)fFile->Get(fListName.Data());
    if (!fList)
      return;

    TList *extra = (TList *)fFile->Get(
        TString::Format("%s_extra", fListName.Data()).Data());

    fEventStat = (TH1F *)extra->FindObject("hAEventsVsMulti");

    if (!fEffOnly) {
      fSigBg = (THnSparse *)fList->FindObject(fSigBgName.Data());
      fBg = (THnSparse *)fList->FindObject(fBgName.Data());
    }
    fMCRec = (THnSparse *)fList->FindObject(fMCRecName.Data());
    fMCGen = (THnSparse *)fList->FindObject(fMCGenName.Data());
  }
}

Double_t AliRsnOutTaskInput::GetNEvents(Double_t min, Double_t max) const {
  if (!fEventStat)
    return 0;

  if (min > max) {
    Printf("Number of events : %.0f ", fEventStat->Integral());
    return fEventStat->Integral();
  }

  Int_t binMin = fEventStat->FindBin(min);
  Int_t binMax = fEventStat->FindBin(max);

  Printf("Number of events : %.0f [%d,%d]",
         fEventStat->Integral(binMin, binMax), binMin, binMax);

  return fEventStat->Integral(binMin, binMax);
}
