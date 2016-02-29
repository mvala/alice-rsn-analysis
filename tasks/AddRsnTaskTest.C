#ifndef __CINT__
#include <TString.h>
#include <AliAnalysisManager.h>
#endif

AliAnalysisTaskSE *AddRsnTaskTest() {

	AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();

	// create task
    gROOT->LoadMacro("AliRsnTaskTest.cxx+g");
    AliAnalysisTaskSE* task = new AliRsnTaskTest("RsnTestTask");
    task->SelectCollisionCandidates(AliVEvent::kMB); // if physics selection performed in UserExec(), this line should be commented
    mgr->AddTask(task);


    TString outputFileName = AliAnalysisManager::GetCommonFileName();    // create containers for input/output
    AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();
    AliAnalysisDataContainer *coutput1 = mgr->CreateContainer("rsnTaskTest", TList::Class(), AliAnalysisManager::kOutputContainer, outputFileName.Data());

    // connect input/output
    mgr->ConnectInput(task, 0, cinput);
    mgr->ConnectOutput(task, 1, coutput1);

    return task;
}
