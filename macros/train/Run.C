#ifndef __CINT__
#include <Riostream.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TString.h>
#include <TStopwatch.h>
#include <AliAnalysisManager.h>
#include <AliESDInputHandler.h>
#include <AliMCEventHandler.h>
#include <AliAODInputHandler.h>
#include <AliAODHandler.h>
#include <AliPhysicsSelectionTask.h>
#include <AliAnalysisTaskPIDResponse.h>
#endif
Bool_t Run(TString anSrc = "grid" /*or "local" or "proof"*/,
        TString anMode = "terminate" /*or "full" or "test"*/,
        TString input="aod" /*or "esd"*/,
        Bool_t useMC=kFALSE /*or kTRUE*/,
		TString intputData="input.txt",
        Long64_t nEvents = kMaxLong64,
        Long64_t nSkip = 0,
		const char * aliphyiscsVersion="vAN-20160216-1",
		const char * alirootVersion="v5-08-00-2",
		const char * rootVersion="v5-34-30-alice-12") {

	// make all options lower case
	anSrc.ToLower();
	anMode.ToLower();
	input.ToLower();

	gROOT->ProcessLine(Form(".include %s/include",gSystem->ExpandPathName("$ALICE_ROOT")));
	gROOT->ProcessLine(Form(".include %s/include",gSystem->ExpandPathName("$ALICE_PHYSICS")));

	// load libs
	if (LoadLibsBase()) return kFALSE;

	// Create Analysis Manager
	AliAnalysisManager *mgr = new AliAnalysisManager("AliRsnAM","AliRsn Analysis Manager");

	// Create input handlers
	Bool_t useAODOut = kFALSE;
	CreateInputHandlers(input,useMC,useAODOut);

	// Seting Analysis Plugin
	gROOT->LoadMacro("AnalysisConfig.C");
	AliAnalysisGrid *analysisPlugin = AnalysisConfig(anMode.Data(),aliphyiscsVersion,alirootVersion,rootVersion);
	if (!analysisPlugin) {
		Printf("Error : analysisPlugin is null !!!");
		return kFALSE;
	}
	if (!intputData.IsNull()) {
		if (!anSrc.CompareTo("proof") && !anMode.CompareTo("full")) {
			analysisPlugin->SetProofDataSet(intputData.Data());
			Printf(Form("Using DataSet %s ...", intputData.Data()));
		} else {
			analysisPlugin->SetFileForTestMode(intputData.Data());
			Printf(Form("Using Test file %s ...", intputData.Data()));
		}
	}

	if (!anSrc.CompareTo("local") && !anMode.CompareTo("full"))
		analysisPlugin->SetNtestFiles(kMaxInt);

	mgr->SetGridHandler(analysisPlugin);

    if (!input.CompareTo("esd")) {
        Printf("Adding PysicsSelection task ...");
		gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
		AliPhysicsSelectionTask *physSelTask = AddTaskPhysicsSelection(useMC);
		if(!physSelTask) { Printf("no physSelTask"); return kFALSE; }
    }

    Printf("Adding PIDResponse task ...");
    gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C");
    AliAnalysisTaskPIDResponse *pidResponseTask = AddTaskPIDResponse(useMC);
	if(!pidResponseTask) { Printf("no pidResponseTask"); return kFALSE; }

	TString wagons = gSystem->GetFromPipe("ls -C *.wag");
	if (wagons.IsNull()) return kFALSE;
	TObjArray *listWagons = wagons.Tokenize(" ");
	TObjString *strObj;
	for (Int_t i=0;i<listWagons->GetEntries();i++) {
		strObj = (TObjString *)listWagons->At(i);
		Printf("Adding wagon %s ...",strObj->GetString().Data());
		if (!AddVagon(strObj->GetString().Data())) return kFALSE;
	}

	TStopwatch timer;
	timer.Start();
	// runs analysis
	if (!RunAnalysisManager(anSrc, anMode.Data(), nEvents, nSkip)) {
		Printf("Error : RunAnalysisManager failed !!!");
		return kFALSE;
	}

	timer.Stop();
	timer.Print();

	Printf("Run.C is successfully done");

	return kTRUE;
}

Int_t LoadLibsBase() {

    Int_t num = 0;
    if (gSystem->Load("libTree") < 0) {num++; return num;}
    if (gSystem->Load("libGeom") < 0) {num++; return num;}
    if (gSystem->Load("libVMC") < 0) {num++; return num;}
    if (gSystem->Load("libMinuit") < 0) {num++; return num;}
    if (gSystem->Load("libPhysics") < 0) {num++; return num;}
    if (gSystem->Load("libSTEERBase") < 0) {num++; return num;}
    if (gSystem->Load("libESD") < 0) {num++; return num;}
    if (gSystem->Load("libAOD") < 0) {num++; return num;}
    if (gSystem->Load("libANALYSIS") < 0) {num++; return num;}
    if (gSystem->Load("libOADB") < 0) {num++; return num;}
    if (gSystem->Load("libANALYSISalice") < 0) {num++; return num;}
    return 0;
}

Bool_t CreateInputHandlers(TString input,Bool_t useMC,Bool_t useAODOut=kFALSE) {

	AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
	if (!mgr) {
		Printf("Error [CreateInputHandlers] : mgr is null !!!");
		return kFALSE;
	}

	if (!input.CompareTo("esd")) {
		AliESDInputHandler *esdHandler = new AliESDInputHandler();
		esdHandler->SetNeedField(kTRUE);
		mgr->SetInputEventHandler(esdHandler);
		if (useMC)
			mgr->SetMCtruthEventHandler(new AliMCEventHandler());
	} else if (!input.CompareTo("aod")) {
		mgr->SetInputEventHandler(new AliAODInputHandler());
	}

	if (useAODOut) {
		AliAODHandler *aodHandler = new AliAODHandler();
		aodHandler->SetOutputFileName("AliAOD.root");
		mgr->SetOutputEventHandler(aodHandler);
	}

   return kTRUE;
}

Bool_t RunAnalysisManager(TString anSrc = "proof", TString anMode = "test", Long64_t nEvents = 1e10, Long64_t nSkip = 0) {

   AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();

   if (!mgr) { Printf("Error [RunAnalysisManager] : mgr is null !!!"); return kFALSE; }

   // Run analysis
   if (!mgr->InitAnalysis()) return kFALSE;
   mgr->PrintStatus();
   //   mgr->SetDebugLevel(2);
   //   mgr->SetNSysInfo(100);

   if ((!anSrc.CompareTo("proof")) || (!anSrc.CompareTo("local"))) {
      mgr->StartAnalysis(anSrc.Data(), nEvents, nSkip);
   } else {
      mgr->StartAnalysis(anSrc.Data());
   }

   return kTRUE;
}

Bool_t SetupPar(TString par) {

	par.ReplaceAll(".par","");
	if (par.IsNull())
		return kFALSE;
	if (gSystem->AccessPathName(Form("%s.par", par.Data()))) return kFALSE;

	gSystem->Exec(Form("tar xzf %s.par", par.Data()));

	TString ocwd = gSystem->WorkingDirectory();
	if (!gSystem->ChangeDirectory(par.Data()))
		return kFALSE;
    Printf("%s",gSystem->pwd());
	// check for BUILD.sh and execute
	if (!gSystem->AccessPathName("PROOF-INF/BUILD.sh")) {
		printf("*******************************\n");
		printf("*** Building PAR archive    ***\n");
		printf("*******************************\n");
		if (gSystem->Exec("PROOF-INF/BUILD.sh")) {
			Error("runProcess", "Cannot Build the PAR Archive! - Abort!");
			return kFALSE;
		}
	}

	// check for SETUP.C and execute
	if (!gSystem->AccessPathName("PROOF-INF/SETUP.C")) {
		printf("*******************************\n");
		printf("*** Setup PAR archive       ***\n");
		printf("*******************************\n");
		gROOT->Macro("PROOF-INF/SETUP.C");
	}
	if (!gSystem->ChangeDirectory(ocwd.Data()))
		return kFALSE;

	return kTRUE;
}

Bool_t AddVagon(const char *fname) {

	ifstream input(fname);
	TString macroStr;
	TString macroFun;
	TString argsStr;
	if (input.is_open()) {
		string line;
		TString lineStr;
		while (getline(input, line)) {
			lineStr = line;
			if (lineStr.IsNull()) continue;
			if (lineStr.BeginsWith("#")) continue;
			if (lineStr.BeginsWith("Libs=")) {
				lineStr.ReplaceAll("Libs=","");
				TObjArray *libArray = lineStr.Tokenize(",");
				TObjString *strObj;
				TString strlib;
				TString strpar;
				for (Int_t i=0;i<libArray->GetEntries();i++) {
					strObj = (TObjString *)libArray->At(i);
					strlib = strObj->GetString();
					strpar = strlib;
					strpar.ReplaceAll("lib","");
					strpar.ReplaceAll(".so","");
					strpar.Append(".par");
					Printf("Loading %s ...",strObj->GetString().Data());

					if (!gSystem->AccessPathName(strpar.Data())) {
						if (!SetupPar(strpar)) return kFALSE;
					} else {
						if (gSystem->Load(strObj->GetString().Data())<0) return kFALSE;
					}
				}
			}
			else if (lineStr.BeginsWith("Macro=")) {
				lineStr.ReplaceAll("Macro=","");
				macroStr = lineStr;
				macroFun = lineStr;
				macroFun = gSystem->BaseName(lineStr.Data());
				macroFun.ReplaceAll(".C","");

			}
			else if (lineStr.BeginsWith("Arguments=")) {
				lineStr.ReplaceAll("Arguments=","");
				argsStr = TString::Format("%s(%s)",macroFun.Data(),lineStr.Data());
			}
		}
		input.close();
	}

	Printf("Loading macro '%s' ...",macroStr.Data());
	gROOT->LoadMacro(macroStr.Data());
	Printf("Running function '%s' ...",argsStr.Data());
	gROOT->ProcessLine(argsStr.Data());
	return kTRUE;
}
