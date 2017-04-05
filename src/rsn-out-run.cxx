#include <Riostream.h>
#include <TArrayI.h>
#include <TFile.h>
#include <TFolder.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TSystem.h>
#include <fstream>

#include <AliRsnOutTaskBinMgr.h>
#include <AliRsnOutTaskInput.h>
#include <AliRsnOutTaskMgr.h>
#include <AliRsnOutTaskResult.h>
#include <AliRsnOutValue.h>

#include <json/json.h>

void help(char **argv) {
  std::cout << argv[0] << " <config-file> " << std::endl;
  std::cout << "Example: " << std::endl;
  std::cout << "\t" << argv[0] << " rsn-config.json " << std::endl;
  exit(1);
}

int main(int argc, char **argv) {

  if (argc < 2)
    help(argv);

  Bool_t useLocalCache = kTRUE;
  if (useLocalCache)
    TFile::SetCacheFileDir(gSystem->HomeDirectory(), 1, 1);

  Json::Value root;
  std::cout << "Loading << " << argv[1] << std::endl;

  std::ifstream config_file(argv[1], std::ifstream::binary);
  config_file >> root;

  AliRsnOutTaskMgr *tMgr;
  AliRsnOutTaskInput *tInputData = 0, *tInputMC = 0;
  TString inputFileName = argv[1];
  inputFileName.ReplaceAll(".json", ".root");
  TString outFileName = inputFileName;
  outFileName.ReplaceAll(".root", "-results.root");
  TFile *fileInput = TFile::Open(inputFileName.Data(), "RECREATE");

  Json::Value data = root["data"];
  Json::Value mc = root["mc"];

  // TString mgr_name = data["name"].asString();
  // mgr_name += "_";
  // mgr_name += mc["name"].asString();
  tMgr = new AliRsnOutTaskMgr("mgr");
  tMgr->Print();

  if (!data.empty()) {
    tInputData = new AliRsnOutTaskInput(data["name"].asString().data());
    tInputData->SetFileName(data["file"].asString());
    tInputData->SetListName(data["list"].asString());
    tInputData->SetSigBgName(data["sigbg"].asString());
    tInputData->SetBgName(data["bg"].asString());
    tMgr->Add(tInputData);
  }
  if (!mc.empty()) {
    tInputMC = new AliRsnOutTaskInput(mc["name"].asString().data());
    tInputMC->SetFileName(mc["file"].asString());
    tInputMC->SetListName(mc["list"].asString());
    tInputMC->SetSigBgName(mc["sigbg"].asString());
    tInputMC->SetBgName(mc["bg"].asString());
    tInputMC->SetMCRecName(mc["rec"].asString());
    tInputMC->SetMCGenName(mc["gen"].asString());
    if (mc["effonly"].asBool())
      tInputMC->SetEfficiencyOnly();
    tMgr->Add(tInputMC);
  }
  TList *listBins = new TList();
  Int_t nActive = 0;
  const Json::Value bins = root["bin"];
  for (uint i = 0; i < bins.size(); ++i) {
    // std::cout << bins[i] << std::endl;
    if (!bins[i]["active"].asBool())
      continue;
    nActive++;
    if (nActive > 2)
      continue;
    TString str = bins[i]["varbins"].asString();
    TObjArray *objArr = str.Tokenize(",");
    TArrayI *arr = new TArrayI(objArr->GetEntries());
    TIter next(objArr);
    TObjString *os;
    int c = 0;
    while ((os = (TObjString *)next())) {
      arr->SetAt(os->GetString().Atoi(), c++);
    }
    listBins->Add(new AliRsnOutValue(bins[i]["id"].asInt(), arr));
  }

  TList *norms = new TList();
  const Json::Value norm = root["norm"];
  for (uint i = 0; i < norm.size(); ++i) {
    norms->Add(new AliRsnOutValue(norm[i]["id"].asInt(),
                                  norm[i]["min"].asDouble(),
                                  norm[i]["max"].asDouble()));
  }

  TList *fits = new TList();
  const Json::Value fit = root["fit"];
  for (uint i = 0; i < fit.size(); ++i) {
    fits->Add(new AliRsnOutValue(fit[i]["id"].asInt(), fit[i]["min"].asDouble(),
                                 fit[i]["max"].asDouble()));
  }

  AliRsnOutTaskBinMgr *binMgr = new AliRsnOutTaskBinMgr("binMgr");
  binMgr->GenerateBinTemplate(norms, fits);
  binMgr->SetListOfVartiations(listBins);
  binMgr->Init();

  if (tInputData)
    tInputData->Add((AliRsnOutTaskBinMgr *)binMgr->Clone());

  if (tInputMC)
    tInputMC->Add((AliRsnOutTaskBinMgr *)binMgr->Clone());

  AliRsnOutTask *tResultsAll = new AliRsnOutTask("results", "Results");

  AliRsnOutTaskResult *tResult = new AliRsnOutTaskResult();
  if (tInputData)
    tResult->SetData(tInputData);
  if (tInputMC)
    tResult->SetMC(tInputMC);

  tResultsAll->Add(tResult);
  tMgr->Add(tResultsAll);

  tMgr->UpdateTasks();
  fileInput->cd();
  if (tMgr)
    tMgr->Write();
  fileInput->Close();

  // return 0;
  inputFileName.Prepend("file://");
  fileInput = TFile::Open(inputFileName.Data(), "READ");
  if (!fileInput)
    return 10;

  fileInput->cd();

  tMgr = (AliRsnOutTaskMgr *)fileInput->Get("mgr");
  // tMgr = (AliRsnOutTaskMgr *)fileInput->Get(mgr_name.Data());
  if (!tMgr) {
    Printf("Error getting AliRsnOutTaskMgr from '%s' !!!",
           inputFileName.Data());
    return 11;
  }

  tMgr->ls();
  tMgr->ExecuteTask("");
  TString outFileNameTDir = outFileName;
  outFileNameTDir.ReplaceAll(".root", "-final.root");
  TFile *fOutTDir = TFile::Open(outFileNameTDir.Data(), "RECREATE");
  if (tMgr) {

    TFolder *d = new TFolder("test", "test");
    tMgr->Export(d);
    d->Write();
  }
  fOutTDir->Close();

  TFile *fOut = TFile::Open(outFileName.Data(), "RECREATE");
  if (tMgr) {
    fOut->cd();
    tMgr->Write();
  }
  fOut->Close();

  return 0;
}
