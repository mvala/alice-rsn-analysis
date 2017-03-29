#ifndef AliRsnOutTask_cxx
#define AliRsnOutTask_cxx

#include <TTask.h>
class AliRsnOutTask : public TTask {

public:
  AliRsnOutTask();
  AliRsnOutTask(const char *name, const char *title = "");
  virtual ~AliRsnOutTask();

  virtual void Add(TTask *task);

  virtual void ExecuteTask(Option_t *option);
  virtual void ExecuteTasks(Option_t *option);
  virtual void UpdateTask();
  virtual void UpdateTasks();

  // User functions
  virtual void Exec(Option_t *option);
  virtual void ExecPost(Option_t *option);

  void SetParent(AliRsnOutTask *task) { fParent = task; }
  AliRsnOutTask *GetParent() const { return fParent; }

  TList *GetOutput() const { return fOutput; }
  void DeleteOutput();

  Int_t GetID() const { return fID; }
  void SetID(Int_t id) { fID = id; }

  virtual void Browse(TBrowser *b);

  virtual void Export(TDirectory *root);

protected:
  Int_t fID;
  AliRsnOutTask *fParent; //||
  TList *fOutput;

  ClassDef(AliRsnOutTask, 1)
};

#endif
