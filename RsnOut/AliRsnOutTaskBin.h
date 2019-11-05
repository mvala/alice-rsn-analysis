#ifndef AliRsnOutTaskBin_cxx
#define AliRsnOutTaskBin_cxx

#include <AliRsnOutTask.h>
#include <AliRsnOutValue.h>

class TEfficiency;
class AliRsnOutTaskInput;
class AliRsnOutTaskBin : public AliRsnOutTask {

public:
  AliRsnOutTaskBin(const char *name = "bin", const char *title = "",
                   Bool_t isCutsOnly = kFALSE);
  virtual ~AliRsnOutTaskBin();

  virtual void Exec(Option_t *option);
  virtual void ExecPost(Option_t *option);
  virtual void UpdateTask();

  AliRsnOutValue *GetValue() { return &fValue; }
  TList *GetListOfCuts() const { return fCuts; }
  TList *GetListOfAdditionalCuts() const { return fAdditionalCuts; }
  void AddCut(AliRsnOutValue *cut);
  void AddAdditionalCut(AliRsnOutValue *cut);

  void ApplyCuts(THnSparse *sigBg, THnSparse *bg, Bool_t updateOnly = kFALSE);

  void SetCutsOnly(Bool_t isCutsOnly = kTRUE) { fCutsOnly = isCutsOnly; }

private:
  AliRsnOutValue fValue;
  TList *fCuts;
  TList *fAdditionalCuts;
  Bool_t fCutsOnly;
  AliRsnOutTaskInput *fInputTask;

  ClassDef(AliRsnOutTaskBin, 1)
};

#endif
