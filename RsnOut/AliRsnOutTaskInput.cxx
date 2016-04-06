#include <TFile.h>
#include <TList.h>
#include <TH1.h>
#include <TStyle.h>
#include <TROOT.h>
#include <THnSparse.h>
#include "AliRsnOutTaskInput.h"

ClassImp(AliRsnOutTaskInput)

AliRsnOutTaskInput::AliRsnOutTaskInput(const char *name, const char *title) :
		AliRsnOutTask(name, title), fFileName(), fListName(), fSigBgName(), fBgName(), fFile(
				0), fList(0), fSigBg(0), fBg(0) {
}

AliRsnOutTaskInput::~AliRsnOutTaskInput() {
}

void AliRsnOutTaskInput::Exec(Option_t* /*option*/) {

	if (!fSigBg) {
		fFile = TFile::Open(fFileName.Data());
		if (!fFile)
			return;
		fList = (TList *) fFile->Get(fListName.Data());
		if (!fList)
			return;
		fSigBg = (THnSparse *) fList->FindObject(fSigBgName.Data());
		if (!fSigBg)
			return;
		fBg = (THnSparse *) fList->FindObject(fBgName.Data());
		if (!fBg)
			return;
	}
}

void AliRsnOutTaskInput::ExecPost(Option_t* /*option*/) {

	SafeDelete(fList);
	fFile->Close();
	SafeDelete(fFile);
	fSigBg = 0;
	fBg = 0;
}
