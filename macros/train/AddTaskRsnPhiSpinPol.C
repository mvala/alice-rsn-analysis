AliRsnMiniAnalysisTask *AddTaskRsnPhiSpinPol(TString name = "PhiKK_SP",
                                             Bool_t isMC) {

  // Retrieve global Analysis Manager
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    ::Error("AddTaskPhiPP13TeV_PID", "No analysis manager to connect to.");
    return NULL;
  }

  AliRsnMiniAnalysisTask *task = new AliRsnMiniAnalysisTask(name.Data(), isMC);

  return task;
}