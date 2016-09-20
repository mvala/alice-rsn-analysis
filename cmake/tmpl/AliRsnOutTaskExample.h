#ifndef AliRsnOutTaskExample_cxx
#define AliRsnOutTaskExample_cxx

#include <AliRsnOutTask.h>

class AliRsnOutTaskExample : public AliRsnOutTask {

public:
  AliRsnOutTaskExample(const char *name = "rsn", const char *title = "");
  virtual ~AliRsnOutTaskExample();

  virtual void Exec(Option_t *option);

private:
  ClassDef(AliRsnOutTaskExample, 1)
};

#endif
