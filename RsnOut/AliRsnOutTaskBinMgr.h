#ifndef AliRsnOutTaskBinMgr_cxx
#define AliRsnOutTaskBinMgr_cxx

#include <AliRsnOutTask.h>
class AliRsnOutTaskBin;
class AliRsnOutTaskBinMgr : public AliRsnOutTask {

public:
  AliRsnOutTaskBinMgr(const char *name = "binMgr", const char *title = "");
  virtual ~AliRsnOutTaskBinMgr();

  void SetListOfVartiations(TList *l) { fListOfVariations = l; }
  void SetBinTemplate(AliRsnOutTaskBin *bt) { fBinTmpl = bt; }
  void Init();

  void GenerateBinVariations(Int_t index, AliRsnOutTask *task);
  
  virtual void Exec(Option_t *option);

private:
  TList *fListOfVariations; // List of AliRsnOutValue with array
  AliRsnOutTaskBin *fBinTmpl;

  ClassDef(AliRsnOutTaskBinMgr, 1)
};

#endif
