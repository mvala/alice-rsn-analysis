#include <THnSparse.h>
#include <TAxis.h>
#include "AliRsnOutTaskBinMgr.h"

#include <AliRsnOutTaskBin.h>
#include <AliRsnOutTaskInput.h>

ClassImp(AliRsnOutTaskBinMgr)

  AliRsnOutTaskBinMgr::AliRsnOutTaskBinMgr(const char *name,
                                             const char *title)
  : AliRsnOutTask(name, title), fListOfVariations(0),fBinTmpl(0) {}

AliRsnOutTaskBinMgr::~AliRsnOutTaskBinMgr() {}
void AliRsnOutTaskBinMgr::Init() {



  AliRsnOutTaskBin *binIntegrated = (AliRsnOutTaskBin *) fBinTmpl->Clone();
  binIntegrated->SetName("no_cut");
  Add(binIntegrated);

  if (!fListOfVariations) return;


  AliRsnOutTaskBin *b;
  if (fListOfVariations->GetSize()==1) {
    AliRsnOutTaskBin *bMgr = new AliRsnOutTaskBin();
    AliRsnOutValue *v = (AliRsnOutValue *) fListOfVariations->At(0);
    Int_t id = v->GetId();
    TArrayD *arr = v->GetArray();
    for (Int_t i = 0; i < arr->GetSize()-1;i++) {
      Printf("Adding id=%d min=%.0f max=%.0f", id, arr->At(i), arr->At(i+1)-1);
      b = new AliRsnOutTaskBin();
      b->AddCut(new AliRsnOutValue(id, arr->At(i), arr->At(i+1)-1));
      b->Add((AliRsnOutTaskBin *) fBinTmpl->Clone());
      bMgr->Add(b);
    }

    Add(bMgr);
  }

}

void AliRsnOutTaskBinMgr::Exec(Option_t * /*option*/) {
  Printf("%s", GetName());
    AliRsnOutTaskInput *input = dynamic_cast<AliRsnOutTaskInput *>(GetParent());
  if (!input) return;

      THnSparse *s = input->GetSigBg();
    if (!s) {
      s = input->GetMCGen();
    }
  AliRsnOutTask *t;
  AliRsnOutValue *v;
  for (Int_t i=0;i< fListOfVariations->GetSize();i++) {
    t = (AliRsnOutTask *) GetListOfTasks()->At(i+1);
    v = (AliRsnOutValue *) fListOfVariations->At(i);
    if (!v) continue;
    Int_t id = v->GetId();
    if (s) t->SetName(s->GetAxis(id)->GetName());
  }
}
