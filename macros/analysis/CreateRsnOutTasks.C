#if defined(__MAKECINT__) || defined(__MAKECLING__)
#include <TSystem.h>
#include <TROOT.h>
#include <TFile.h>
#include <TList.h>
#include <TTask.h>
#include <TString.h>
#include <TObjArray.h>
#include <TBufferJSON.h>
#include <AliRsnOutTask.h>
#include <AliRsnOutTaskInput.h>
#include <AliRsnOutTaskBin.h>
#include <AliRsnOutTaskNorm.h>
#include <AliRsnOutTaskFit.h>
#endif

AliRsnOutTaskBin *AddBin(Int_t id, Double_t min, Double_t max,TList *norms, TList *fits);

void CreateRsnOutTasks(TString names="pt:ctj:ctt") {

	TFile *f = TFile::Open("RsnOutMgr.root","RECREATE");

	AliRsnOutTaskMgr *tMgr = 0;
	TObjArray *a = names.Tokenize(":");
	TObjString *s;
	TString sname;
	for (Int_t i = 0; i < a->GetEntries(); ++i) {
		s = (TObjString*) a->At(i);
		sname = s->GetString();
		sname.ToLower();
		if (!sname.CompareTo("pt")) tMgr = CreateRsnOutMgr(sname,1,6,50,1);
		else if (!sname.CompareTo("ctj")) tMgr = CreateRsnOutMgr(sname,3,1,21,1);
		else if (!sname.CompareTo("ctt")) tMgr = CreateRsnOutMgr(sname,4,1,21,1);
		else if (!sname.CompareTo("test")) tMgr = CreateRsnOutMgr(sname,1,6,15,5);

		if (tMgr) tMgr->Write();
	}

	f->Close();


}
AliRsnOutTaskMgr * CreateRsnOutMgr(TString name,Int_t cutAxis=1,Int_t binStart=6,Int_t binEnd = 50,Int_t binStep = 1) {

	AliRsnOutTaskMgr *tMgr = new AliRsnOutTaskMgr(name);

	AliRsnOutTaskInput *tInputMC = new AliRsnOutTaskInput("mc");
	tInputMC->SetFileName("root://alieos.saske.sk///eos/alike.saske.sk/alice/alike/PWGLF/LF_pp_MC/376_20160308-1500/merge_runlist_X/AnalysisResults.root");
	tInputMC->SetListName("RsnOut_tpc3s");
	tInputMC->SetSigBgName("Unlike");
	tInputMC->SetBgName("Mixing");
	tInputMC->SetMCRecName("Trues");
	tInputMC->SetMCGenName("Mother");
	tInputMC->SetEfficiencyOnly(kTRUE);
	tMgr->Add(tInputMC);

	AliRsnOutTaskInput *tInputData = new AliRsnOutTaskInput("data");
	tInputData->SetFileName("root://alieos.saske.sk///eos/alike.saske.sk/alice/alike/PWGLF/LF_pp/389_20160307-1141/merge_runlist_4/AnalysisResults.root");
	tInputData->SetListName("RsnOut_tpc3s");
	tInputData->SetSigBgName("Unlike");
	tInputData->SetBgName("Mixing");
	tInputData->AddMC(tInputMC);
	tMgr->Add(tInputData);

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
		tBin = AddBin(cutAxis,i,i+binStep-1,norms,fits);
		tInputData->Add(tBin);
		tBin = AddBin(cutAxis,i,i+binStep-1,norms,fits);
		tInputMC->Add(tBin);
	}

	return tMgr;
}

AliRsnOutTaskBin *AddBin(Int_t id, Double_t min, Double_t max,TList *norms, TList *fits) {
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
