#ifndef AliRsnOutTaskBin_cxx
#define AliRsnOutTaskBin_cxx

#include <AliRsnOutValue.h>
#include <AliRsnOutTask.h>

class AliRsnOutTaskBin : public AliRsnOutTask {

public:

  AliRsnOutTaskBin(const char *name="rsn", const char *title="");
  virtual ~AliRsnOutTaskBin();

  virtual void Exec(Option_t *option);

  AliRsnOutValue *GetValue() { return &fValue; }
  TList *GetListOfCuts() const { return fCuts; }
  void AddCut(AliRsnOutValue *cut);

  void ApplyCuts(THnSparse *sigBg,THnSparse *bg);

private:

  AliRsnOutValue fValue;
  TList *fCuts;

  ClassDef(AliRsnOutTaskBin, 1)

};

#endif

