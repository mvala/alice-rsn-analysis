#if defined(__MAKECINT__) || defined(__MAKECLING__)
#include <TFile.h>
#include <AliRsnOutTaskMgr.h>
#endif

void ExportTasks(const char *filename="file://RsnOutMgr.root",const char *filenameOut = "RsnOutFolder.root") {

	TFile::SetCacheFileDir(gSystem->HomeDirectory(), 1, 1);

	TFile *f = TFile::Open(filename, "READ");
	if (!f) return;
	AliRsnOutTaskMgr *tMgr = (AliRsnOutTaskMgr*) f->Get("RsnMgr");
	if (!tMgr) return;

	TFile *fOut = TFile::Open(filenameOut, "RECREATE");
	tMgr->Export(fOut);
	fOut->Close();
	f->Close();
}
