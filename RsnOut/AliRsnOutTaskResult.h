#ifndef AliRsnOutTaskResult_cxx
#define AliRsnOutTaskResult_cxx

#include <AliRsnOutTask.h>

class AliRsnOutTaskResult : public AliRsnOutTask {

public:
  AliRsnOutTaskResult(const char *name = "results", const char *title = "");
  virtual ~AliRsnOutTaskResult();

  virtual void Exec(Option_t *option);

  void SetData(AliRsnOutTask *data);
  void SetMC(AliRsnOutTask *mc);

private:
  AliRsnOutTask *fData;
  AliRsnOutTask *fMC;
  ClassDef(AliRsnOutTaskResult, 1)
};

#endif
