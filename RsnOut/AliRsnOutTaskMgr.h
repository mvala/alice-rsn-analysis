#ifndef AliRsnOutTaskMgr_cxx
#define AliRsnOutTaskMgr_cxx

#include <AliRsnOutTask.h>

class AliRsnOutTaskMgr : public AliRsnOutTask {

public:

  AliRsnOutTaskMgr(const char *name="RsnMgr", const char *title="Rsn Task Manager");
  virtual ~AliRsnOutTaskMgr();

  // User functions
  virtual void Exec(Option_t *option);
  virtual void ExecPost(Option_t *option);

protected:

  Bool_t fAddDirectoryStatus;


  ClassDef(AliRsnOutTaskMgr, 1)

};

#endif

