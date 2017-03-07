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


  GenerateBinVariations(fListOfVariations->GetSize(), this);
  return;


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
      b = (AliRsnOutTaskBin *) fBinTmpl->Clone();
      b->AddCut(new AliRsnOutValue(id, arr->At(i), arr->At(i+1)-1));

      bMgr->Add(b);
    }

    Add(bMgr);
  }

}

void AliRsnOutTaskBinMgr::Exec(Option_t * /*option*/) {

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

  Printf("%s", GetName());
}


void AliRsnOutTaskBinMgr::GenerateBinVariations(Int_t index, AliRsnOutTask *task) {

    if (index < 0) return;

    AliRsnOutTaskBin *b;
    if (!index) {
        Printf("Doing no_cut");
        b = (AliRsnOutTaskBin *) fBinTmpl->Clone();
        b->SetName("no_cut");
        task->Add(b);
        
        return;
    }
    
    GenerateBinVariations(index-1, task);

    for (Int_t i = index-1; i < index ;i++) {
      Printf("i=%d index=%d", i, index);

      b = new AliRsnOutTaskBin(TString::Format("bin%d",i).Data());
      AliRsnOutValue *v = (AliRsnOutValue *) fListOfVariations->At(i);
      if (v) {     
        Int_t id = v->GetId();
        TArrayD *arr = v->GetArray();
        AliRsnOutTaskBin *b2;
        for (Int_t i = 0; i < arr->GetSize()-1;i++) {
          Printf("Adding id=%d min=%.0f max=%.0f", id, arr->At(i), arr->At(i+1)-1);
          b2 = (AliRsnOutTaskBin *) fBinTmpl->Clone();
          b2->AddCut(new AliRsnOutValue(id, arr->At(i), arr->At(i+1)-1));
          b->Add(b2);
        }
        task->Add(b);
      }

      
    }


}
