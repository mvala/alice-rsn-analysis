#ifndef __CINT__
#include <AliAnalysisManager.h>
#include <TString.h>
#endif

AliAnalysisTaskSE *AddRsnTaskCosThetaTest() {

  // Taking task from same directory as this macro
  TString macroPathStr = ".";
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  TMap *map = mgr->GetGlobals();
  TNamed *macroPath = map->GetValue("alice-rsn-macro-path");
  if (macroPath) macroPathStr = macroPath->GetTitle();

  // create task
  gROOT->LoadMacro(TString::Format("%s/AliRsnTaskCosThetaTest.cxx+g",macroPathStr.Data()).Data());
  AliAnalysisTaskSE *task = new AliRsnTaskTestCosTheata("RsnCosThetaTestTask");
  task->SelectCollisionCandidates(AliVEvent::kMB); // if physics selection
                                                   // performed in UserExec(),
                                                   // this line should be
                                                   // commented
  mgr->AddTask(task);

  TString outputFileName =
      AliAnalysisManager::GetCommonFileName(); // create containers for
                                               // input/output
  AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();
  AliAnalysisDataContainer *coutput1 = mgr->CreateContainer(
      "rsnTaskCosThetaTest", TList::Class(),
      AliAnalysisManager::kOutputContainer, outputFileName.Data());

  // connect input/output
  mgr->ConnectInput(task, 0, cinput);
  mgr->ConnectOutput(task, 1, coutput1);

  return task;
}
