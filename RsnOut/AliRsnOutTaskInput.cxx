#include <TFile.h>
#include <TList.h>
#include <TH1.h>
#include <THnSparse.h>
#include "AliRsnOutTaskInput.h"

ClassImp(AliRsnOutTaskInput)

AliRsnOutTaskInput::AliRsnOutTaskInput(const char *name, const char *title) :
		AliRsnOutTask(name, title), fFileName(), fListName(), fSigBgName(), fBgName(), fSigBg(
				0), fBg(0) {
}

AliRsnOutTaskInput::~AliRsnOutTaskInput() {
}

void AliRsnOutTaskInput::Exec(Option_t* /*option*/) {

	if (!fSigBg) {
		TFile *f = TFile::Open(fFileName.Data());
		if (!f)
			return;
		TList *l = (TList *) f->Get(fListName.Data());
		if (!l)
			return;
		fSigBg = (THnSparse *) l->FindObject(fSigBgName.Data());
		if (!fSigBg)
			return;
		fBg = (THnSparse *) l->FindObject(fBgName.Data());
		if (!fBg)
			return;
	}
	fSigBg->Print();
	fBg->Print();
}
