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

TObject *AddBin(Int_t id, Double_t min, Double_t max);

void RsnOutTasks(Bool_t verbose=kFALSE) {

	TFile::SetCacheFileDir(gSystem->HomeDirectory(), 1, 1);

	AliRsnOutTask *tMgr = new AliRsnOutTask("RsnMgr", "Rsn Task Manager");

	AliRsnOutTaskInput *tInput = new AliRsnOutTaskInput("RsnInput");
	tInput->SetFileName("root://alieos.saske.sk///eos/alike.saske.sk/alice/alike/PWGLF/LF_pp/389_20160307-1141/merge_runlist_4/AnalysisResults.root");
	tInput->SetListName("RsnOut_tpc3s");
	tInput->SetSigBgName("Unlike");
	tInput->SetBgName("Mixing");
	tMgr->Add(tInput);

	AliRsnOutTaskBin *tBin;

	Int_t step=5;
	Int_t max = 50;
	for (Int_t i=1;i<=max;i+=step) {
		tBin = (AliRsnOutTaskBin *) AddBin(1,i,i+step-1);
		tInput->Add(tBin);
	}
//	AliRsnOutTaskBin *tBin = new AliRsnOutTaskBin("RsnBin");
//	tBin->GetValue()->SetId(0);
//	tBin->AddCut(new AliRsnOutValue(1,6,10));
////	tBin->AddCut(new AliRsnOutValue(1,11,15));
////	tBin->AddCut(new AliRsnOutValue(1,16,20));
////	tBin->AddCut(new AliRsnOutValue(1,21,25));
////	tBin->AddCut(new AliRsnOutValue(3,1,1));
//	tInput->Add(tBin);
//
//	AliRsnOutTaskNorm *tNorm = new AliRsnOutTaskNorm("RsnNorm");
//	tNorm->AddRange(new AliRsnOutValue(0,1.05,1.10));
//	tBin->Add(tNorm);
//
//	AliRsnOutTaskFit *tFit = new AliRsnOutTaskFit("RsnFit");
//	tFit->AddFit(new AliRsnOutValue(0,1.000,1.040));
//	tFit->AddFit(new AliRsnOutValue(0,0.997,1.050));
//
//	tNorm->Add(tFit);
//
//
//
//	tBin = new AliRsnOutTaskBin("RsnBin");
//	tBin->GetValue()->SetId(0);
////	tBin->AddCut(new AliRsnOutValue(1,6,10));
//	tBin->AddCut(new AliRsnOutValue(1,11,15));
////	tBin->AddCut(new AliRsnOutValue(1,16,20));
////	tBin->AddCut(new AliRsnOutValue(1,21,25));
////	tBin->AddCut(new AliRsnOutValue(3,1,1));
//	tInput->Add(tBin);
//
//	tNorm = new AliRsnOutTaskNorm("RsnNorm");
//	tNorm->AddRange(new AliRsnOutValue(0,1.05,1.10));
//	tBin->Add(tNorm);
//
//	tFit = new AliRsnOutTaskFit("RsnFit");
//	tFit->AddFit(new AliRsnOutValue(0,1.000,1.040));
//	tFit->AddFit(new AliRsnOutValue(0,0.997,1.050));
//
//	tNorm->Add(tFit);

	tMgr->ExecuteTask();

	if (verbose) {
		TString json = TBufferJSON::ConvertToJSON(tMgr);
		Printf("%s", json.Data());
		Printf("size=%d", json.Length());
		std::ofstream out("output.json");
		out << json;
		out.close();
	}

//	gROOT->GetListOfBrowsables()->Add(tMgr);
//	new TBrowser;
}

TObject *AddBin(Int_t id, Double_t min, Double_t max) {
	AliRsnOutTaskBin *tBin = new AliRsnOutTaskBin("RsnBin");
	tBin->GetValue()->SetId(0);
	tBin->AddCut(new AliRsnOutValue(id,min,max));

	AliRsnOutTaskNorm *tNorm = new AliRsnOutTaskNorm("RsnNorm");
	tNorm->AddRange(new AliRsnOutValue(0,1.05,1.10));
	tBin->Add(tNorm);

	AliRsnOutTaskFit *tFit = new AliRsnOutTaskFit("RsnFit");
	tFit->AddFit(new AliRsnOutValue(0,1.000,1.040));
	tFit->AddFit(new AliRsnOutValue(0,0.997,1.050));

	tNorm->Add(tFit);

	return tBin;

}
