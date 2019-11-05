#ifndef AliRsnOutTaskBinMgr_cxx
#define AliRsnOutTaskBinMgr_cxx

#include <AliRsnOutTask.h>
class AliRsnOutTaskBin;
class AliRsnOutTaskFit;
class AliRsnOutTaskBinMgr : public AliRsnOutTask {

public:
  AliRsnOutTaskBinMgr(const char *name = "binMgr", const char *title = "");
  virtual ~AliRsnOutTaskBinMgr();

  void SetListOfVartiations(TList *l) { fListOfVariations = l; }
  void SetListOfAdditionalCuts(TList *l) { fListOfAdditionalCuts = l; }
  void SetFitTemplate(AliRsnOutTaskFit*f) { fFitTemplate = f; }
  void SetBinTemplate(AliRsnOutTaskBin *bt) { fBinTmpl = bt; }
  void Init();
  
  AliRsnOutTaskFit *GetFitTemplate() { return fFitTemplate; }
  void GenerateBinTemplate(TList *norms, TList *fits);
  void GenerateBinVariations(Int_t index, AliRsnOutTask *task);
  void GenerateBinVsBin(AliRsnOutTask *task, Int_t first, Int_t second);

  virtual void Exec(Option_t *option);

private:
  TList *fListOfVariations; // List of AliRsnOutValue with array
  TList *fListOfAdditionalCuts; // List of Additional cuts
  AliRsnOutTaskFit *fFitTemplate; //->
  AliRsnOutTaskBin *fBinTmpl; //->

  ClassDef(AliRsnOutTaskBinMgr, 1)
};

#endif
