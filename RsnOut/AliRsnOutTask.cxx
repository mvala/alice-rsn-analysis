#include "AliRsnOutTask.h"
#include <Riostream.h>
#include <TDirectory.h>
#include <TFolder.h>
#include <TROOT.h>

ClassImp(AliRsnOutTask);

AliRsnOutTask::AliRsnOutTask()
    : TTask("task", "Task"), fID(0), fParent(0), fOutput(0), fPath(0) {
  fPath = new TList();
}

AliRsnOutTask::AliRsnOutTask(const char *name, const char *title)
    : TTask(name, title), fID(0), fParent(0), fOutput(0), fPath(0) {
  fOutput = new TList();
  fOutput->SetName("out");
  fOutput->SetOwner();
  fPath = new TList();
}

AliRsnOutTask::~AliRsnOutTask() {}

void AliRsnOutTask::Add(TTask *task) {
  if (!task)
    return;

  if (task->InheritsFrom(AliRsnOutTask::Class())) {
    AliRsnOutTask *t = (AliRsnOutTask *)task;
    t->SetParent(this);
    t->SetID(fTasks->GetEntries());
  }

  TTask::Add(task);
}

void AliRsnOutTask::ExecuteTask(Option_t *option) {

  if (!IsActive())
    return;

  fOption = option;
  fgBeginTask = this;
  fgBreakPoint = 0;

  if (fBreakin)
    return;

  TROOT::IndentLevel();
  std::cout << "Execute task:" << GetName() << " : " << GetTitle() << std::endl;
  TROOT::IncreaseDirLevel();

  Exec(option);

  fHasExecuted = kTRUE;
  ExecuteTasks(option);
  ExecPost(option);

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

    TROOT::IndentLevel();
    std::cout << "Execute task:" << task->GetName() << " : " << task->GetTitle()
              << std::endl;
    TROOT::IncreaseDirLevel();

    task->Exec(option);
    task->fHasExecuted = kTRUE;
    task->ExecuteTasks(option);

    TROOT::DecreaseDirLevel();

    TROOT::IndentLevel();
    std::cout << "Post Execute task:" << task->GetName() << " : "
              << task->GetTitle() << std::endl;
    task->ExecPost(option);

    if (task->fBreakout == 1) {
      printf("Break at exit of task: %s\n", task->GetName());
      fgBreakPoint = this;
      task->fBreakout++;
      return;
    }
  }
}

void AliRsnOutTask::UpdateTask() {}
void AliRsnOutTask::UpdateTasks() {

  fPath->Clear();
  if (fParent) {
    TIter next(fParent->GetPath());
    AliRsnOutTask *tt;
    while ((tt = (AliRsnOutTask *)next())) {
      fPath->Add(tt);
    }
    fPath->Add(fParent);
  }

  TIter next(fTasks);
  AliRsnOutTask *task;
  while ((task = (AliRsnOutTask *)next())) {
    task->UpdateTask();
    task->UpdateTasks();
  }
}

void AliRsnOutTask::Exec(Option_t * /*option*/) {}

void AliRsnOutTask::ExecPost(Option_t * /*option*/) {}

void AliRsnOutTask::Browse(TBrowser *b) {
  fTasks->Browse(b);
  if (fOutput)
    fOutput->Browse(b);
}

void AliRsnOutTask::Export(TDirectory *root) {

  if (!root)
    return;
  TDirectory *out = root->mkdir(GetName(), GetTitle());
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

  root->cd();
}

void AliRsnOutTask::Export(TFolder *root) {

  if (!root)
    return;
  TFolder *out = 0;
  out = root->AddFolder(GetName(), GetTitle(), fOutput);
  if (!out)
    return;

  TIter next(fTasks);
  AliRsnOutTask *t;
  while ((t = (AliRsnOutTask *)next())) {
    t->Export(out);
  }
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
