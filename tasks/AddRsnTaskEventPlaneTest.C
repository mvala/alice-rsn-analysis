#ifndef __CINT__
#include <AliAnalysisManager.h>
#include <TString.h>
#endif

AliAnalysisTaskSE *AddRsnTaskEventPlaneTest() {

  // Taking task from same directory as this macro
  TString macroPathStr = ".";
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  TMap *map = mgr->GetGlobals();
  TNamed *macroPath = map->GetValue("alice-rsn-macro-path");
  if (macroPath) macroPathStr = macroPath->GetTitle();

  // create task
  gROOT->LoadMacro(TString::Format("%s/AliRsnTaskEventPlaneTest.cxx+g",macroPathStr.Data()).Data());
  AliAnalysisTaskSE *task = new AliRsnTaskEventPlaneTest("RsnTestEventPlaneTask");
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
      "rsnTaskEventPlaneTest", TList::Class(), AliAnalysisManager::kOutputContainer,
      outputFileName.Data());

  // connect input/output
  mgr->ConnectInput(task, 0, cinput);
  mgr->ConnectOutput(task, 1, coutput1);

  return task;
}
