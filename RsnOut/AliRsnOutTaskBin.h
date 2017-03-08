#ifndef AliRsnOutTaskBin_cxx
#define AliRsnOutTaskBin_cxx

#include <AliRsnOutTask.h>
#include <AliRsnOutValue.h>
class AliRsnOutTaskInput;
class AliRsnOutTaskBin : public AliRsnOutTask {

public:
  AliRsnOutTaskBin(const char *name = "bin", const char *title = "",
                   Bool_t isCutsOnly = kFALSE);
  virtual ~AliRsnOutTaskBin();

  virtual void Exec(Option_t *option);
  virtual void ExecPost(Option_t *option);

  AliRsnOutValue *GetValue() { return &fValue; }
  TList *GetListOfCuts() const { return fCuts; }
  void AddCut(AliRsnOutValue *cut);

  void ApplyCuts(THnSparse *sigBg, THnSparse *bg);

private:
  AliRsnOutValue fValue;
  TList *fCuts;
  Bool_t fCutsOnly;
  AliRsnOutTaskInput *fInputTask;

  ClassDef(AliRsnOutTaskBin, 1)
};

#endif
