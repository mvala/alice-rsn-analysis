
enum ERsnCollisionType { kPP = 0, kPbPb = 1 };
void SetEventCuts(AliRsnMiniAnalysisTask *task, Double_t vzCut,
                  Bool_t rejectPileUp);
void SetEventHistograms(AliRsnMiniAnalysisTask *task);
void SetPairCuts(AliRsnMiniAnalysisTask *task, Double_t minY, Double_t maxY);
void SetRsnMixing(AliRsnMiniAnalysisTask *task, Int_t n, Float_t vz, Float_t m);

AliRsnMiniAnalysisTask *
AddTaskRsnPhiSpinPol(TString name = "PhiKK_SP", Bool_t isMC = kFALSE,
                     UInt_t triggerMask = AliVEvent::kMB,
                     ERsnCollisionType collisionType = kPP,
                     TString outputFileName = "") {

  // ==== START RSN CONFIG =========
  Bool_t rejectPileUp = kTRUE;
  Double_t vzCut = 10.0; // cm, default cut on vtx z

  // Rsn Mixing
  Int_t nmix = 10;
  Float_t maxDiffVzMix = 1.;
  Float_t maxDiffMultMix = 10.;

  if (collisionType == kPP || isMC)
    rejectPileUp = kFALSE;

  // Pair rapidity
  Double_t minYlab = -0.5;
  Double_t maxYlab = 0.5;

  // ====== END RSN CONFIG =========

  // Retrieve global Analysis Manager
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    ::Error("AddTaskRsnPhiSpinPol", "No analysis manager to connect to.");
    return NULL;
  }

  // Creating Rsn Task
  AliRsnMiniAnalysisTask *task = new AliRsnMiniAnalysisTask(name.Data(), isMC);

  // Setting trigger
  task->SelectCollisionCandidates(triggerMask);

  // Setting multiplicity or centrality setting
  if (collisionType == kPP)
    task->UseMultiplicity("QUALITY");
  else
    task->UseCentrality("V0M");

  // Setting event cuts
  SetEventCuts(task, isMC, collisionType, vzCut, rejectPileUp);
  SetEventHistograms(task);

  // Setting pair cuts
  SetPairCuts(task, minYlab, maxYlab);

  // set rsn mixing
  SetRsnMixing(task, nmix, maxDiffVzMix, maxDiffMultMix);

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

void SetEventCuts(AliRsnMiniAnalysisTask *task, Bool_t isMC,
                  ERsnCollisionType collisionType, Double_t vzCut,
                  Bool_t rejectPileUp) {

  // cut on primary vertex:
  // - 2nd argument --> |Vz| range
  // - 3rd argument --> minimum required number of contributors to vtx
  // - 4th argument --> tells if TPC stand-alone vertexes must be accepted
  AliRsnCutPrimaryVertex *cutVertex = 0;
  cutVertex = new AliRsnCutPrimaryVertex("cutEVertex", vtxZcut, 0, kFALSE);

  AliRsnCutEventUtils *cutEventUtils = 0;
  cutEventUtils = new AliRsnCutEventUtils("cutEUtils", kTRUE, rejectPileUp);

  if ((collisionType == kPP) && (!isMC) && cutVertex) {
    cutVertex->SetCheckPileUp(rejectPileUp); // set the check for pileup
    ::Info("AddTaskRsnPhiSpinPol",
           Form(":::::::::::::::::: Pile-up rejection mode: %s",
                (rejectPileUp) ? "ON" : "OFF"));
  }

  // define and fill cut set for event cut
  AliRsnCutSet *eventCuts = new AliRsnCutSet("eventCuts", AliRsnTarget::kEvent);

  eventCuts->AddCut(cutEventUtils);
  eventCuts->AddCut(cutVertex);
  TString cutScheme =
      TString::Format("%s&%s", cutEventUtils->GetName(), cutVertex->GetName());
  eventCuts->SetCutScheme(cutScheme.Data());

  task->SetEventCuts(eventCuts);
}

void SetEventHistograms(AliRsnMiniAnalysisTask *task) {
  // -- EVENT-ONLY COMPUTATIONS
  // -------------------------------------------------------------------

  // vertex
  Int_t vtxID = task->CreateValue(AliRsnMiniValue::kVz, kFALSE);
  AliRsnMiniOutput *outVtx = task->CreateOutput("eventVtx", "HIST", "EVENT");
  outVtx->AddAxis(vtxID, 240, -12.0, 12.0);

  // multiplicity or centrality
  Int_t multID = task->CreateValue(AliRsnMiniValue::kMult, kFALSE);
  AliRsnMiniOutput *outMult = task->CreateOutput("eventMult", "HIST", "EVENT");
  if (isPP && !MultBins)
    outMult->AddAxis(multID, 400, 0.5, 400.5);
  else
    outMult->AddAxis(multID, 110, 0., 110.);

  TH2F *hvz = new TH2F("hVzVsCent", "", 110, 0., 110., 240, -12.0, 12.0);
  task->SetEventQAHist(
      "vz", hvz); // plugs this histogram into the fHAEventVz data member

  TH2F *hmc = new TH2F("MultiVsCent", "", 110, 0., 110., 400, 0.5, 400.5);
  hmc->GetYaxis()->SetTitle("QUALITY");
  task->SetEventQAHist(
      "multicent",
      hmc); // plugs this histogram into the fHAEventMultiCent data member
}

void SetPairCuts(AliRsnMiniAnalysisTask *task, Double_t minY, Double_t maxY) {

  // -- PAIR CUTS (common to all resonances)
  // ------------------------------------------------------

  AliRsnCutMiniPair *cutY =
      new AliRsnCutMiniPair("cutRapidity", AliRsnCutMiniPair::kRapidityRange);
  cutY->SetRangeD(minY, maxY);
  AliRsnCutSet *cutsPair = new AliRsnCutSet("pairCuts", AliRsnTarget::kMother);
  cutsPair->AddCut(cutY);
  cutsPair->SetCutScheme(cutY->GetName());
}

void SetRsnMixing(AliRsnMiniAnalysisTask *task, Int_t n, Float_t vz,
                  Float_t m) {
  if (!n)
    return;

  // set event mixing options
  task->UseContinuousMix();
  // task->UseBinnedMix();

  task->SetNMix(n);
  task->SetMaxDiffVz(vz);
  task->SetMaxDiffMult(m);
  ::Info("AddTaskRsnPhiSpinPol",
         Form("Event mixing configuration: \n events to mix = %i \n max diff. "
              "vtxZ = cm %5.3f \n max diff multi = %5.3f",
              n, vz, m));
}