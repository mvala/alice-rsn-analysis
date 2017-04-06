AliRsnMiniAnalysisTask *AddTaskRsnPhiSpinPol(TString name = "PhiKK_SP",
                                             Bool_t isMC,
                                             TString outputFileName = "") {

  // Retrieve global Analysis Manager
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    ::Error("AddTaskPhiPP13TeV_PID", "No analysis manager to connect to.");
    return NULL;
  }

  // Creating Rsn Task
  AliRsnMiniAnalysisTask *task = new AliRsnMiniAnalysisTask(name.Data(), isMC);

  // ==== START RSN CONFIG =========

  
  // ====== END RSN CONFIG =========

  // Contecting input container (ESD or AOD) from Analysis Manager
  mgr->ConnectInput(task, 0, mgr->GetCommonInputContainer());

  // Setting ouptut filename
  if (outputFileName.IsNull())
    outputFileName = AliAnalysisManager::GetCommonFileName();

  TString outListName = "RsnOutput";
  // Creating output container
  AliAnalysisDataContainer *output = mgr->CreateContainer(
      outListName.Data(), TList::Class(), AliAnalysisManager::kOutputContainer,
      outputFileName.Data());

  // Connecting output
  mgr->ConnectOutput(task, 1, output);

  // returning task
  return task;
}