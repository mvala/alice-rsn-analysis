#ifndef AliRsnOutTaskResult_cxx
#define AliRsnOutTaskResult_cxx

#include <AliRsnOutTask.h>

class AliRsnOutTaskResult : public AliRsnOutTask {

public:

  AliRsnOutTaskResult(const char *name="rsn", const char *title="");
  virtual ~AliRsnOutTaskResult();

  virtual void Exec(Option_t *option);

private:

  ClassDef(AliRsnOutTaskResult, 1)

};

#endif

