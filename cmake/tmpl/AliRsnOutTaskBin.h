#ifndef AliRsnOutTaskBin_cxx
#define AliRsnOutTaskBin_cxx

#include <AliRsnOutTask.h>

class AliRsnOutTaskBin : public AliRsnOutTask {

public:

  AliRsnOutTaskBin(const char *name="rsn", const char *title="");
  virtual ~AliRsnOutTaskBin();

  virtual void Exec(Option_t *option);

private:

  ClassDef(AliRsnOutTaskBin, 1)

};

#endif

