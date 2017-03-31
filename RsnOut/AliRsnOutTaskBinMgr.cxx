#include "AliRsnOutTaskBinMgr.h"
#include <TAxis.h>
#include <THnSparse.h>

#include <AliRsnOutTaskBin.h>
#include <AliRsnOutTaskFit.h>
#include <AliRsnOutTaskInput.h>
#include <AliRsnOutTaskNorm.h>

ClassImp(AliRsnOutTaskBinMgr);

AliRsnOutTaskBinMgr::AliRsnOutTaskBinMgr(const char *name, const char *title)
    : AliRsnOutTask(name, title), fListOfVariations(0), fBinTmpl(0) {}

AliRsnOutTaskBinMgr::~AliRsnOutTaskBinMgr() {}
void AliRsnOutTaskBinMgr::Init() {
  GenerateBinVariations(fListOfVariations->GetSize(), this);
}

void AliRsnOutTaskBinMgr::Exec(Option_t * /*option*/) {

  AliRsnOutTaskInput *input = dynamic_cast<AliRsnOutTaskInput *>(GetParent());
  if (!input)
    return;

  THnSparse *s = input->GetSigBg();
  if (!s) {
    s = input->GetMCGen();
  }

  AliRsnOutTask *t;
  AliRsnOutValue *v;
  TString str;
  // Renaming tasks
  for (Int_t i = 0; i < fListOfVariations->GetSize(); i++) {
    v = (AliRsnOutValue *)fListOfVariations->At(i);
    if (!v)
      continue;
    Int_t id = v->GetId();
    for (Int_t j = 0; j < fTasks->GetSize(); j++) {
      t = (AliRsnOutTask *)fTasks->At(j);
      if (s) {
        str = t->GetName();
        str.ReplaceAll(TString::Format("bin%d", id), s->GetAxis(id)->GetName());
        t->SetName(str.Data());
      }
    }
  }

  // Printf("%s", GetName());
}

void AliRsnOutTaskBinMgr::GenerateBinTemplate(TList *norms, TList *fits) {

  if (fBinTmpl) {
    delete fBinTmpl;
    fBinTmpl = 0;
  }

  fBinTmpl = new AliRsnOutTaskBin();

  // Loop over norms
  TIter nextNorm(norms);
  AliRsnOutValue *vNorm;
  while ((vNorm = (AliRsnOutValue *)nextNorm())) {
    AliRsnOutTaskNorm *tNorm = new AliRsnOutTaskNorm();
    tNorm->AddRange(vNorm);
    fBinTmpl->Add(tNorm);

    // Loop over fits
    TIter nextFit(fits);
    AliRsnOutValue *vFit;
    while ((vFit = (AliRsnOutValue *)nextFit())) {
      AliRsnOutTaskFit *tFit = new AliRsnOutTaskFit();
      tFit->SetFit(vFit);
      tNorm->Add(tFit);
    }
  }
}

void AliRsnOutTaskBinMgr::GenerateBinVariations(Int_t index,
                                                AliRsnOutTask *task) {

  if (index < 0)
    return;

  AliRsnOutTaskBin *b;
  if (!index) {
    // Printf("Doing no_cut");
    // b = (AliRsnOutTaskBin *)fBinTmpl->Clone();
    // b->SetName("no_cut");
    // b->SetCutsOnly();
    // task->Add(b);
    return;
  }

  Printf("Calling GenerateBinVariations %d %s", index - 1, task->GetName());
  GenerateBinVariations(index - 1, task);

  AliRsnOutTaskBin *b2;
  // Adding parameter distribution (P - no_otehr_cuts)
  for (Int_t i = index - 1; i < index; i++) {
    Printf("i=%d index=%d", i, index);
    AliRsnOutValue *v = (AliRsnOutValue *)fListOfVariations->At(i);
    if (v) {
      b = new AliRsnOutTaskBin(TString::Format("bin%d", v->GetId()).Data());
      b->SetCutsOnly();
      Int_t id = v->GetId();
      TArrayI *arr = v->GetArray();

      for (Int_t i = 0; i < arr->GetSize() - 1; i++) {
        Printf("Adding id=%d min=%d max=%d", id, arr->At(i),
               arr->At(i + 1) - 1);
        b2 = (AliRsnOutTaskBin *)fBinTmpl->Clone();
        b2->AddCut(new AliRsnOutValue(id, arr->At(i), arr->At(i + 1) - 1));
        b->Add(b2);
      }
      task->Add(b);
    }
  }
  if (index == 2) {
    GenerateBinVsBin(task, 0, 1);
  }

  // Not tested yet
  if (index == 3) {
    GenerateBinVsBin(task, 0, 2);
    GenerateBinVsBin(task, 1, 2);
  }
}

void AliRsnOutTaskBinMgr::GenerateBinVsBin(AliRsnOutTask *task, Int_t first,
                                           Int_t second) {
  // Adding 2D variations
  AliRsnOutTaskBin *b, *b2, *b3;

  AliRsnOutValue *v1 = (AliRsnOutValue *)fListOfVariations->At(first);
  AliRsnOutValue *v2 = (AliRsnOutValue *)fListOfVariations->At(second);
  if (v1 && v2) {
    Printf("Creating %d vs %d", v1->GetId(), v2->GetId());
    b = new AliRsnOutTaskBin(
        TString::Format("bin%d_vs_bin%d", v1->GetId(), v2->GetId()).Data());
    b->SetCutsOnly();
    TArrayI *arr1 = v1->GetArray();
    TArrayI *arr2 = v2->GetArray();
    for (Int_t i = 0; i < arr1->GetSize() - 1; i++) {
      b2 = new AliRsnOutTaskBin(TString::Format("bin_%d", i).Data());
      b2->AddCut(
          new AliRsnOutValue(v1->GetId(), arr1->At(i), arr1->At(i + 1) - 1));
      // b2->SetCutsOnly();
      for (Int_t j = 0; j < arr2->GetSize() - 1; j++) {
        Printf("Adding id=%d min=%d max=%d vs id=%d min=%d max=%d", v1->GetId(),
               arr1->At(i), arr1->At(i + 1) - 1, v2->GetId(), arr2->At(j),
               arr2->At(j + 1) - 1);
        b3 = (AliRsnOutTaskBin *)fBinTmpl->Clone();
        b3->AddCut(
            new AliRsnOutValue(v2->GetId(), arr2->At(j), arr2->At(j + 1) - 1));
        // b3->SetCutsOnly();
        b2->Add(b3);
      }
      b->Add(b2);
    }
    task->Add(b);
  }
}
