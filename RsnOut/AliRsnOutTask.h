#ifndef AliRsnOutTask_cxx
#define AliRsnOutTask_cxx

#include <TTask.h>

class AliRsnOutTask : public TTask {

public:

  AliRsnOutTask();
  AliRsnOutTask(const char *name, const char *title="");
  virtual ~AliRsnOutTask();

  virtual void Add(TTask *task);

  virtual void Exec(Option_t *option);

  void SetParent(AliRsnOutTask *task) { fParent = task; }
  AliRsnOutTask *GetPartent() const { return fParent; }

  TList *GetOutput() const { return fOutput; }

  virtual void Browse(TBrowser *b);

protected:

  AliRsnOutTask *fParent;//!
  TList *fOutput;

  ClassDef(AliRsnOutTask, 1)

};

#endif

