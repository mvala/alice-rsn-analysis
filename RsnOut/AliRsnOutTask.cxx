#include "AliRsnOutTask.h"

ClassImp(AliRsnOutTask)

AliRsnOutTask::AliRsnOutTask(const char *name, const char *title) : TTask(name,title), fParent(0), fOutput(0) {
}

AliRsnOutTask::~AliRsnOutTask() {
}

void AliRsnOutTask::Add(TTask* task) {
	if (!task)
		return;

	TTask::Add(task);

	if (task->InheritsFrom(AliRsnOutTask::Class())) {
		((AliRsnOutTask*) task)->SetParent(this);
	}
}

void AliRsnOutTask::Exec(Option_t* /*option*/) {
	Printf("%s",GetName());
}
