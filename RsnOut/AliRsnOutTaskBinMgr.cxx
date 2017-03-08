#include <THnSparse.h>
#include <TAxis.h>
#include "AliRsnOutTaskBinMgr.h"

#include <AliRsnOutTaskBin.h>
#include <AliRsnOutTaskInput.h>
#include <AliRsnOutTaskNorm.h>
#include <AliRsnOutTaskFit.h>

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
    
    Printf("Calling GenerateBinVariations %d %s",index-1, task->GetName());
    GenerateBinVariations(index-1, task);

    AliRsnOutTaskBin *b2;
    // Adding parameter distribution (P - no_otehr_cuts)
    for (Int_t i = index-1; i < index ;i++) {
      Printf("i=%d index=%d", i, index);


      AliRsnOutValue *v = (AliRsnOutValue *) fListOfVariations->At(i);
      if (v) {
        b = new AliRsnOutTaskBin(TString::Format("bin%d",v->GetId()).Data());
        Int_t id = v->GetId();
        TArrayD *arr = v->GetArray();
        
        for (Int_t i = 0; i < arr->GetSize()-1;i++) {
          Printf("Adding id=%d min=%.0f max=%.0f", id, arr->At(i), arr->At(i+1)-1);
          b2 = (AliRsnOutTaskBin *) fBinTmpl->Clone();
          b2->AddCut(new AliRsnOutValue(id, arr->At(i), arr->At(i+1)-1));
          b->Add(b2);
        }
        task->Add(b);
      }      
    }

    // Adding 2D variations
    AliRsnOutTaskBin *b3;
    if (index == 2) {
      AliRsnOutValue *v1 = (AliRsnOutValue *) fListOfVariations->At(0);
      AliRsnOutValue *v2 = (AliRsnOutValue *) fListOfVariations->At(1);
      if (v1&&v2) {
        Printf("Creating %d vs %d", v1->GetId(),v2->GetId());
        b = new AliRsnOutTaskBin(TString::Format("bin%d_vs_bin%d",v1->GetId(),v2->GetId()).Data());
        TArrayD *arr1 = v1->GetArray();
        TArrayD *arr2 = v2->GetArray();
        for (Int_t i = 0; i < arr1->GetSize()-1;i++) {
          b2 = new AliRsnOutTaskBin(TString::Format("bin_%d",i).Data());
          b2->AddCut(new AliRsnOutValue(v1->GetId(), arr1->At(i), arr1->At(i+1)-1));
          for (Int_t j = 0; j < arr2->GetSize()-1;j++) {
            Printf("Adding id=%d min=%.0f max=%.0f vs id=%d min=%.0f max=%.0f", v1->GetId(), arr1->At(i), arr1->At(i+1)-1, v2->GetId(), arr2->At(j), arr2->At(j+1)-1);
            b3 = (AliRsnOutTaskBin *) fBinTmpl->Clone();
            b3->AddCut(new AliRsnOutValue(v2->GetId(), arr2->At(j), arr2->At(j+1)-1));
            b2->Add(b3);
          }
          b->Add(b2);
        }


        task->Add(b);
      }
    }


}

void AliRsnOutTaskBinMgr::GenerateBinTemplate(TList *norms,TList *fits) {

  if (fBinTmpl) delete fBinTmpl;

  fBinTmpl = new AliRsnOutTaskBin();

  // Loop over norms
  TIter nextNorm(norms);
  AliRsnOutValue *vNorm;
  while ((vNorm = (AliRsnOutValue *)nextNorm()))
  {
    AliRsnOutTaskNorm *tNorm = new AliRsnOutTaskNorm();
    tNorm->AddRange(vNorm);
    fBinTmpl->Add(tNorm);

    // Loop over fits
    TIter nextFit(fits);
    AliRsnOutValue *vFit;
    while ((vFit = (AliRsnOutValue *)nextFit()))
    {
      AliRsnOutTaskFit *tFit = new AliRsnOutTaskFit();
      tFit->SetFit(vFit);
      tNorm->Add(tFit);
    }
  }
}


