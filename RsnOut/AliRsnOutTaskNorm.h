#ifndef AliRsnOutTaskNorm_cxx
#define AliRsnOutTaskNorm_cxx

#include <AliRsnOutTask.h>
class AliRsnOutValue;
class AliRsnOutTaskNorm : public AliRsnOutTask {

public:
  AliRsnOutTaskNorm(const char *name = "rsn", const char *title = "");
  virtual ~AliRsnOutTaskNorm();

  virtual void Exec(Option_t *option);

  void AddRange(AliRsnOutValue *range);
  TList *GetRanges() const { return fRanges; }

private:
  TList *fRanges;

  ClassDef(AliRsnOutTaskNorm, 1)
};

#endif
