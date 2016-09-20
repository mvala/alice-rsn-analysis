#include "AliRsnOutTask.h"
#include <Riostream.h>
#include <TDirectory.h>
#include <TROOT.h>

ClassImp(AliRsnOutTask)

    AliRsnOutTask::AliRsnOutTask()
    : TTask("task", "Task"), fParent(0), fOutput(0) {}

AliRsnOutTask::AliRsnOutTask(const char *name, const char *title)
    : TTask(name, title), fParent(0), fOutput(0) {
  fOutput = new TList();
  fOutput->SetName("out");
  fOutput->SetOwner();
}

AliRsnOutTask::~AliRsnOutTask() {}

void AliRsnOutTask::Add(TTask *task) {
  if (!task)
    return;

  TTask::Add(task);

  if (task->InheritsFrom(AliRsnOutTask::Class())) {
    ((AliRsnOutTask *)task)->SetParent(this);
  }
}

void AliRsnOutTask::ExecuteTask(Option_t *option) {
  if (fgBeginTask) {
    Error("ExecuteTask", "Cannot execute task:%s, already running task: %s",
          GetName(), fgBeginTask->GetName());
    return;
  }
  if (!IsActive())
    return;

  fOption = option;
  fgBeginTask = this;
  fgBreakPoint = 0;

  if (fBreakin)
    return;
  if (gDebug > 1) {
    TROOT::IndentLevel();
    std::cout << "Execute task:" << GetName() << " : " << GetTitle()
              << std::endl;
    TROOT::IncreaseDirLevel();
  }

  Exec(option);

  fHasExecuted = kTRUE;
  ExecuteTasks(option);
  ExecPost(option);

  if (gDebug > 1)
    TROOT::DecreaseDirLevel();
  if (fBreakout)
    return;

  if (!fgBreakPoint) {
    fgBeginTask->CleanTasks();
    fgBeginTask = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Execute all the subtasks of a task.

void AliRsnOutTask::ExecuteTasks(Option_t *option) {
  TIter next(fTasks);
  AliRsnOutTask *task;
  while ((task = (AliRsnOutTask *)next())) {
    if (fgBreakPoint)
      return;
    if (!task->IsActive())
      continue;
    if (task->fHasExecuted) {
      task->ExecuteTasks(option);
      continue;
    }
    if (task->fBreakin == 1) {
      printf("Break at entry of task: %s\n", task->GetName());
      fgBreakPoint = this;
      task->fBreakin++;
      return;
    }

    if (gDebug > 1) {
      TROOT::IndentLevel();
      std::cout << "Execute task:" << task->GetName() << " : "
                << task->GetTitle() << std::endl;
      TROOT::IncreaseDirLevel();
    }
    task->Exec(option);
    task->fHasExecuted = kTRUE;
    task->ExecuteTasks(option);
    task->ExecPost(option);
    if (gDebug > 1)
      TROOT::DecreaseDirLevel();
    if (task->fBreakout == 1) {
      printf("Break at exit of task: %s\n", task->GetName());
      fgBreakPoint = this;
      task->fBreakout++;
      return;
    }
  }
}

void AliRsnOutTask::Exec(Option_t * /*option*/) {}

void AliRsnOutTask::ExecPost(Option_t * /*option*/) {}

void AliRsnOutTask::Browse(TBrowser *b) {
  fTasks->Browse(b);
  if (fOutput)
    fOutput->Browse(b);
}

void AliRsnOutTask::Export(TDirectory *parent) {

  if (!parent)
    return;
  TDirectory *out = parent->mkdir(GetName(), GetTitle());
  if (!out)
    return;

  TIter next(fTasks);
  AliRsnOutTask *t;
  while ((t = (AliRsnOutTask *)next())) {
    t->Export(out);
  }
  out->cd();
  if (fOutput)
    fOutput->Write();

  parent->cd();
}
