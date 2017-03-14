#include "AliRsnOutTask.h"
#include <Riostream.h>
#include <TDirectory.h>
#include <TROOT.h>

ClassImp(AliRsnOutTask);

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

  if (!IsActive())
    return;

  fOption = option;

  // if (gDebug > 1) {
  TROOT::IndentLevel();
  std::cout << "Execute task:" << GetName() << " : " << GetTitle() << std::endl;
  TROOT::IncreaseDirLevel();
  // }

  Exec(option);

  fHasExecuted = kTRUE;
  ExecuteTasks(option);
  ExecPost(option);

  // if (gDebug > 1)
  TROOT::DecreaseDirLevel();
}

////////////////////////////////////////////////////////////////////////////////
/// Execute all the subtasks of a task.

void AliRsnOutTask::ExecuteTasks(Option_t *option) {
  TIter next(fTasks);
  AliRsnOutTask *task;
  while ((task = (AliRsnOutTask *)next())) {
    if (!task->IsActive())
      continue;
    if (task->fHasExecuted) {
      task->ExecuteTasks(option);
      continue;
    }

    // if (gDebug > 1) {
    TROOT::IndentLevel();
    std::cout << "Execute task:" << task->GetName() << " : " << task->GetTitle()
              << std::endl;
    TROOT::IncreaseDirLevel();
    // }
    task->Exec(option);
    task->fHasExecuted = kTRUE;
    task->ExecuteTasks(option);
    task->ExecPost(option);
    // if (gDebug > 1)
    TROOT::DecreaseDirLevel();
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

void AliRsnOutTask::DeleteOutput() {

  delete fOutput;
  fOutput = new TList();
  fOutput->SetName("out");
  fOutput->SetOwner();

  TIter next(fTasks);
  AliRsnOutTask *t;
  while ((t = (AliRsnOutTask *)next())) {
    t->DeleteOutput();
  }
}
