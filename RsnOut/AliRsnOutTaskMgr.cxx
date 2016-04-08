#include <TROOT.h>
#include <TH1.h>
#include <TStyle.h>
#include "AliRsnOutTaskMgr.h"

ClassImp(AliRsnOutTaskMgr)

AliRsnOutTaskMgr::AliRsnOutTaskMgr(const char *name, const char *title) : AliRsnOutTask(name,title),fAddDirectoryStatus(kTRUE) {
}

AliRsnOutTaskMgr::~AliRsnOutTaskMgr() {
}

void AliRsnOutTaskMgr::Exec(Option_t* /*option*/) {

	// Histograms will not be added to the list of in-memory objects
	fAddDirectoryStatus = TH1::AddDirectoryStatus();
	TH1::AddDirectory(kFALSE);

	// Turning on batch mode
	gROOT->SetBatch();

	// Setting style
	gStyle->SetMarkerStyle(20);
	gStyle->SetHistLineColor(kBlack);
	gStyle->SetFuncColor(kBlue);

}

void AliRsnOutTaskMgr::ExecPost(Option_t* /*option*/) {

	// Turning off batch mode
	gROOT->SetBatch(kFALSE);

	// Returning back status
	TH1::AddDirectory(fAddDirectoryStatus);
}
