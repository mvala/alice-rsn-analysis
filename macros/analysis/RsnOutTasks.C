#if defined(__MAKECINT__) || defined(__MAKECLING__)
#include <TSystem.h>
#include <TROOT.h>
#include <TFile.h>
#include <TList.h>
#include <TTask.h>
#include <TBufferJSON.h>
#include <AliRsnOutTask.h>
#include <AliRsnOutTaskInput.h>
#include <AliRsnOutTaskBin.h>
#include <AliRsnOutTaskNorm.h>
#include <AliRsnOutTaskFit.h>
#endif

TObject *AddBin(Int_t id, Double_t min, Double_t max,TList *norms, TList *fits);

void RsnOutTasks(Bool_t buildJSON=kFALSE) {

	Int_t cutAxis=1;
	Int_t binStart=6;
	Int_t binEnd = 50;
	Int_t binStep = 1;

	cutAxis = 3;
	binStart = 1;
	binEnd = 21;
	binStep = 1;

	binEnd = 1;

	TFile::SetCacheFileDir(gSystem->HomeDirectory(), 1, 1);

	AliRsnOutTaskMgr *tMgr = new AliRsnOutTaskMgr();

	AliRsnOutTaskInput *tInput = new AliRsnOutTaskInput();
	tInput->SetFileName("root://alieos.saske.sk///eos/alike.saske.sk/alice/alike/PWGLF/LF_pp/389_20160307-1141/merge_runlist_4/AnalysisResults.root");
	tInput->SetListName("RsnOut_tpc3s");
	tInput->SetSigBgName("Unlike");
	tInput->SetBgName("Mixing");
	tMgr->Add(tInput);

	TList *norms = new TList();
	norms->Add(new AliRsnOutValue(0,1.10,1.15));

	TList *fits = new TList();
	Int_t nFits = 3;
	for (Int_t fitId=0;fitId<nFits;fitId++) {
		fits->Add(new AliRsnOutValue(fitId,0.997,1.050));
		fits->Add(new AliRsnOutValue(fitId,1.000,1.080));
		fits->Add(new AliRsnOutValue(fitId,0.997,1.130));
	}

	AliRsnOutTaskBin *tBin;
	for (Int_t i=binStart;i<=binEnd;i+=binStep) {
		tBin = (AliRsnOutTaskBin *) AddBin(cutAxis,i,i+binStep-1,norms,fits);
		tInput->Add(tBin);
	}

	tMgr->ExecuteTask();

//	if (buildJSON) {
//		TString json = TBufferJSON::ConvertToJSON(tMgr);
//		Printf("size=%d", json.Length());
//		std::ofstream out("output.json");
//		out << json;
//		out.close();
//	}

	TFile *f = TFile::Open("RsnOutMgr.root","RECREATE");
	tMgr->Write();
	f->Close();

//	gROOT->GetListOfBrowsables()->Add(tMgr);
//	new TBrowser;
}

TObject *AddBin(Int_t id, Double_t min, Double_t max,TList *norms, TList *fits) {
	AliRsnOutTaskBin *tBin = new AliRsnOutTaskBin();
	tBin->GetValue()->SetId(0);
	tBin->AddCut(new AliRsnOutValue(id,min,max));

	TIter next(norms);
	AliRsnOutValue *vNorm;
	while ((vNorm = (AliRsnOutValue*)next())) {

		AliRsnOutTaskNorm *tNorm = new AliRsnOutTaskNorm();
		tNorm->AddRange(vNorm);
		tBin->Add(tNorm);

		TIter next(fits);
		AliRsnOutValue *vFit;
		while ((vFit = (AliRsnOutValue*)next())) {
			AliRsnOutTaskFit *tFit = new AliRsnOutTaskFit();
			tFit->SetFit(vFit);
			tNorm->Add(tFit);
		}
	}

	return tBin;
}
