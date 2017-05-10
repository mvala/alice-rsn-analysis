
enum ERsnCollisionType { kPP = 0, kPbPb = 1 };
void SetEventCuts(AliRsnMiniAnalysisTask *task, Double_t vzCut,
                  Bool_t rejectPileUp);
void SetEventHistograms(AliRsnMiniAnalysisTask *task,ERsnCollisionType collisionType);
AliRsnCutSet *SetPairCuts(AliRsnMiniAnalysisTask *task, Double_t minY, Double_t maxY);
void SetRsnMixing(AliRsnMiniAnalysisTask *task, Int_t n, Float_t vz, Float_t m);
void SetRsnOutput(AliRsnMiniAnalysisTask *task, AliRsnCutSet *cutsPair,
                  ERsnCollisionType collisionType, Bool_t isMC = kFALSE,
                  AliRsnCutSetDaughterParticle::ERsnDaughterCutSet qualityCut =
                      AliRsnCutSetDaughterParticle::kQualityStd2010,
                  Double_t nSigmaKaon = 3.0 ,TString polarizationOpt="");
AliAnalysisTask *AddTaskRsnPhiSpinPol(
    TString name = "PhiKK_SP", ERsnCollisionType collisionType = ERsnCollisionType::kPP,
    Bool_t isMC = kFALSE, UInt_t triggerMask = AliVEvent::kMB,
    AliRsnCutSetDaughterParticle::ERsnDaughterCutSet qualityCut =
        AliRsnCutSetDaughterParticle::kQualityStd2010,
    Double_t nSigmaKaon = 3.0, TString polarizationOpt="", TString outputFileName = "") {

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
  SetEventHistograms(task,collisionType);

  // Setting pair cuts
  AliRsnCutSet *pairCut = SetPairCuts(task, minYlab, maxYlab);

  // Setting rsn mixing
  SetRsnMixing(task, nmix, maxDiffVzMix, maxDiffMultMix);

  // Configuring rsn output
  SetRsnOutput(task, pairCut, collisionType, isMC, qualityCut, nSigmaKaon, polarizationOpt);

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
  cutVertex = new AliRsnCutPrimaryVertex("cutEVertex", vzCut, 0, kFALSE);

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

void SetEventHistograms(AliRsnMiniAnalysisTask *task,ERsnCollisionType collisionType) {
  // -- EVENT-ONLY COMPUTATIONS
  // -------------------------------------------------------------------

  // vertex
  Int_t vtxID = task->CreateValue(AliRsnMiniValue::kVz, kFALSE);
  AliRsnMiniOutput *outVtx = task->CreateOutput("eventVtx", "HIST", "EVENT");
  outVtx->AddAxis(vtxID, 240, -12.0, 12.0);

  // multiplicity or centrality
  Int_t multID = task->CreateValue(AliRsnMiniValue::kMult, kFALSE);
  AliRsnMiniOutput *outMult = task->CreateOutput("eventMult", "HIST", "EVENT");
  if(collisionType==kPP)
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

AliRsnCutSet *SetPairCuts(AliRsnMiniAnalysisTask *task, Double_t minY, Double_t maxY) {

  // -- PAIR CUTS (common to all resonances)
  // ------------------------------------------------------

  AliRsnCutMiniPair *cutY =
      new AliRsnCutMiniPair("cutRapidity", AliRsnCutMiniPair::kRapidityRange);
  cutY->SetRangeD(minY, maxY);
  AliRsnCutSet *cutsPair = new AliRsnCutSet("pairCuts", AliRsnTarget::kMother);
  cutsPair->AddCut(cutY);
  cutsPair->SetCutScheme(cutY->GetName());
  return cutsPair;
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
void SetRsnOutput(AliRsnMiniAnalysisTask *task, AliRsnCutSet *cutsPair,
                  ERsnCollisionType collisionType, Bool_t isMC,
                  AliRsnCutSetDaughterParticle::ERsnDaughterCutSet qualityCut,
                  Double_t nSigmaKaon ,TString polarizationOpt) {

  Int_t aodFilterBit = -1; // ESD
  Bool_t useCrossedRows = kTRUE;

  Bool_t isMcTrueOnly = kFALSE;
  Bool_t useMixLS = kFALSE;

  AliRsnCutSetDaughterParticle *cutSetKaon = 0;
  cutSetKaon = new AliRsnCutSetDaughterParticle(
      TString::Format("cutK%i_%2.1fsigma", qualityCut, nSigmaKaon).Data(),
      qualityCut, AliPID::kKaon, nSigmaKaon, aodFilterBit, useCrossedRows);

  Int_t iCutK = task->AddTrackCuts(cutSetKaon);

  // -- Values ------------------------------------------------------------------------------------
  /* invariant mass   */ Int_t imID   = task->CreateValue(AliRsnMiniValue::kInvMass,kFALSE);
  /* mother mass      */ Int_t mmID   = task->CreateValue(AliRsnMiniValue::kInvMassMother,kFALSE);
  /* IM difference    */ Int_t diffID = task->CreateValue(AliRsnMiniValue::kInvMassDiff,kTRUE);
  /* transv. momentum */ Int_t ptID   = task->CreateValue(AliRsnMiniValue::kPt,kFALSE);
  /* centrality       */ Int_t centID = task->CreateValue(AliRsnMiniValue::kMult,kFALSE);
  /* pseudorapidity   */ Int_t etaID  = task->CreateValue(AliRsnMiniValue::kEta,kFALSE);
  /* rapidity         */ Int_t yID    = task->CreateValue(AliRsnMiniValue::kY,kFALSE);
  /* 1st daughter pt  */ Int_t fdpt   = task->CreateValue(AliRsnMiniValue::kFirstDaughterPt,kFALSE);
  /* 2nd daughter pt  */ Int_t sdpt   = task->CreateValue(AliRsnMiniValue::kSecondDaughterPt,kFALSE);
  /* 1st daughter p   */ Int_t fdp    = task->CreateValue(AliRsnMiniValue::kFirstDaughterP,kFALSE);
  /* 2nd daughter p   */ Int_t sdp    = task->CreateValue(AliRsnMiniValue::kSecondDaughterP,kFALSE);
  /* cos(theta) J     */ Int_t ctjID  = task->CreateValue(AliRsnMiniValue::kCosThetaJackson,kFALSE);
  /* cos(theta) J (MC)*/ Int_t ctjmID  = task->CreateValue(AliRsnMiniValue::kCosThetaJackson,kTRUE);
  /* cos(theta) T     */ Int_t cttID  = task->CreateValue(AliRsnMiniValue::kCosThetaTransversity,kFALSE);
  /* cos(theta) T (MC)*/ Int_t cttmID  = task->CreateValue(AliRsnMiniValue::kCosThetaTransversity,kTRUE);
  /* cos(theta) E     */ Int_t cteID  = task->CreateValue(AliRsnMiniValue::kCosThetaToEventPlane,kFALSE);
  /* cos(theta) E (MC)*/ Int_t ctemID  = task->CreateValue(AliRsnMiniValue::kCosThetaToEventPlane,kTRUE);

  // -- Create all needed outputs -----------------------------------------------------------------
  // use an array for more compact writing, which are different on mixing and charges
  // [0] = unlike
  // [1] = mixing
  // [2] = like ++
  // [3] = like --

  Bool_t  use    [11]={!isMcTrueOnly,!isMcTrueOnly,!isMcTrueOnly,!isMcTrueOnly, isMC,isMC,isMC,isMC,isMC, useMixLS,useMixLS};
  Int_t   useIM  [11]={ 1      ,  1     , 1      ,  1     ,  1     ,  1        ,  2      , 2           ,0       , 1        , 1        };
  TString name   [11]={"Unlike","Mixing","LikePP","LikeMM","Trues" ,"TruesFine","TruesMM","TruesFineMM","Res"   ,"MixingPP","MixingMM"};
  TString comp   [11]={"PAIR"  , "MIX"  ,"PAIR"  ,"PAIR"  , "TRUE" , "TRUE"    ,"TRUE"   ,"TRUE"       ,"TRUE"  ,"MIX"     ,"MIX"     };
  TString output [11]={"SPARSE","SPARSE","SPARSE","SPARSE","SPARSE","SPARSE"   ,"SPARSE" ,"SPARSE"     ,"SPARSE","SPARSE"  ,"SPARSE"  };
  Int_t   pdgCode[11]={333     , 333    ,333     ,333     , 333    , 333       ,333      ,333          ,333     , 333      ,333       };
  Char_t  charge1[11]={'+'     , '+'    ,'+'     ,'-'     , '+'    , '+'       ,'+'      , '+'         ,'+'     ,'+'       ,'-'       };
  Char_t  charge2[11]={'-'     , '-'    ,'+'     ,'-'     , '-'    , '-'       ,'-'      , '-'         ,'-'     ,'+'       ,'-'       };

  TString suffix = "";
  for(Int_t i=0;i<9;i++){
      if(!use[i]) continue;
      AliRsnMiniOutput* out=task->CreateOutput(TString::Format("phi_%s%s",name[i].Data(),suffix.Data()).Data(),output[i].Data(),comp[i].Data());
      out->SetCutID(0,iCutK);
      out->SetCutID(1,iCutK);
      out->SetDaughter(0,AliRsnDaughter::kKaon);
      out->SetDaughter(1,AliRsnDaughter::kKaon);
      out->SetCharge(0,charge1[i]);
      out->SetCharge(1,charge2[i]);
      out->SetMotherPDG(pdgCode[i]);
      out->SetMotherMass(1.019461);
      out->SetPairCuts(cutsPair);

      //axis X: invmass (or resolution)
      if(useIM[i]==1) out->AddAxis(imID,215,0.985,1.2);
      else if(useIM[i]==2) out->AddAxis(mmID,75,0.985,1.06);
      else out->AddAxis(diffID,200,-0.02,0.02);

      //axis Y: transverse momentum of pair as default
      out->AddAxis(ptID,200,0.,20.);//default use mother pt

      // axis Z: centrality-multiplicity
      if(collisionType!=kPP) out->AddAxis(centID,100,0.,100.);
      else out->AddAxis(centID,161,-0.5,160.5);
      // axis W: pseudorapidity
      // out->AddAxis(etaID, 20, -1.0, 1.0);
      // axis J: rapidity
      // out->AddAxis(yID, 10, -0.5, 0.5);

      if (polarizationOpt.Contains("J")) out->AddAxis(ctjID,20,-1.,1);
      if (polarizationOpt.Contains("T")) out->AddAxis(cttID,20,-1.,1);
      if (polarizationOpt.Contains("E")) out->AddAxis(cteID,20,-1.,1);
    }

    if(isMC){   
      //get mothers for phi PDG = 333
      AliRsnMiniOutput* outm=task->CreateOutput(Form("phi_Mother%s", suffix),"SPARSE","MOTHER");
      outm->SetDaughter(0,AliRsnDaughter::kKaon);
      outm->SetDaughter(1,AliRsnDaughter::kKaon);
      outm->SetMotherPDG(333);
      outm->SetMotherMass(1.019461);
      outm->SetPairCuts(cutsPair);
      outm->AddAxis(imID,215,0.985,1.2);
      outm->AddAxis(ptID,200,0.,20.);
      if(collisionType!=kPP) outm->AddAxis(centID,100,0.,100.);
      else outm->AddAxis(centID,161,-0.5,160.5);
      if (polarizationOpt.Contains("J")) outm->AddAxis(ctjmID,20,-1.,1.);
      if (polarizationOpt.Contains("T")) outm->AddAxis(cttmID,20,-1.,1.);
      if (polarizationOpt.Contains("E")) outm->AddAxis(ctemID,20,-1.,1.);

      AliRsnMiniOutput* outmf=task->CreateOutput(Form("phi_MotherFine%s", suffix),"SPARSE","MOTHER");
      outmf->SetDaughter(0,AliRsnDaughter::kKaon);
      outmf->SetDaughter(1,AliRsnDaughter::kKaon);
      outmf->SetMotherPDG(333);
      outmf->SetMotherMass(1.019461);
      outmf->SetPairCuts(cutsPair);
      outmf->AddAxis(imID,215,0.985,1.2);
      outmf->AddAxis(ptID,300,0.,3.);//fine binning for efficiency weighting
      if(collisionType!=kPP) outmf->AddAxis(centID,100,0.,100.);
      else outmf->AddAxis(centID,161,-0.5,160.5);
      if (polarizationOpt.Contains("J")) outmf->AddAxis(ctjmID,20,-1.,1.);
      if (polarizationOpt.Contains("T")) outmf->AddAxis(cttmID,20,-1.,1.);
      if (polarizationOpt.Contains("E")) outmf->AddAxis(ctemID,20,-1.,1.);


      //get phase space of the decay from mothers
      AliRsnMiniOutput* outps=task->CreateOutput(Form("phi_phaseSpace%s", suffix),"HIST","TRUE");
      outps->SetDaughter(0,AliRsnDaughter::kKaon);
      outps->SetDaughter(1,AliRsnDaughter::kKaon);
      outps->SetCutID(0,iCutK);
      outps->SetCutID(1,iCutK);
      outps->SetMotherPDG(333);
      outps->SetMotherMass(1.019461);
      outps->SetPairCuts(cutsPair);
      outps->AddAxis(fdpt,100,0.,10.);
      outps->AddAxis(sdpt,100,0.,10.);
      outps->AddAxis(ptID,200,0.,20.);

      AliRsnMiniOutput* outpsf=task->CreateOutput(Form("phi_phaseSpaceFine%s", suffix),"HIST","TRUE");
      outpsf->SetDaughter(0,AliRsnDaughter::kKaon);
      outpsf->SetDaughter(1,AliRsnDaughter::kKaon);
      outpsf->SetCutID(0,iCutK);
      outpsf->SetCutID(1,iCutK);
      outpsf->SetMotherPDG(333);
      outpsf->SetMotherMass(1.019461);
      outpsf->SetPairCuts(cutsPair);
      outpsf->AddAxis(fdpt,30,0.,3.);
      outpsf->AddAxis(sdpt,30,0.,3.);
      outpsf->AddAxis(ptID,300,0.,3.);

      //get reflections
      if(checkReflex){
        AliRsnMiniOutput* outreflex=task->CreateOutput(Form("phi_reflex%s", suffix),"SPARSE","TRUE");
        outreflex->SetDaughter(0,AliRsnDaughter::kKaon);
        outreflex->SetDaughter(1,AliRsnDaughter::kKaon);
        outreflex->SetCutID(0,iCutK);
        outreflex->SetCutID(1,iCutK);
        outreflex->SetMotherPDG(333);
        outreflex->SetMotherMass(1.019461);
        outreflex->SetPairCuts(cutsPair);
        outreflex->AddAxis(imID,215,0.985,1.2);
        outreflex->AddAxis(ptID,200,0.,20.);
        if(collisionType!=kPP) outreflex->AddAxis(centID,100,0.,100.);
        else outreflex->AddAxis(centID,400,0.5,400.5);
        if (polarizationOpt.Contains("J")) outreflex->AddAxis(ctjID,20,-1.,1.);
        if (polarizationOpt.Contains("T")) outreflex->AddAxis(cttID,20,-1.,1.);
        if (polarizationOpt.Contains("E")) outreflex->AddAxis(cteID,20,-1.,1.);
      }//end reflections
    }//end MC

}