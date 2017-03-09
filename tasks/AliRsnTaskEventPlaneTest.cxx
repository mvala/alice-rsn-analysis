/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

/* AliRsnTaskEventPlaneTest.cxx
 *
 * Template task producing a P_t spectrum and pseudorapidity distribution.
 * Includes explanations of physics and primary track selections
 *
 * Instructions for adding histograms can be found below, starting with NEW
 * HISTO
 *
 * Based on tutorial example from offline pages
 * Edited by Arvinder Palaha
 */
#include "AliRsnTaskEventPlaneTest.h"

#include "Riostream.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TList.h"
#include "TTree.h"

#include "AliAODEvent.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisTaskSE.h"
#include "AliESDEvent.h"
#include "AliESDInputHandler.h"
#include "AliESDtrackCuts.h"
#include "AliMCEvent.h"
#include "AliStack.h"

#include "AliAnalysisTaskFlowVectorCorrections.h"
#include "AliQnCorrectionsManager.h"
#include "AliQnCorrectionsQnVector.h"

ClassImp(AliRsnTaskEventPlaneTest)

    //________________________________________________________________________
    AliRsnTaskEventPlaneTest::AliRsnTaskEventPlaneTest()
    : AliAnalysisTaskSE(), fOutput(0), fHistEpAngle(0), fFlowQnVectorMgr(0)

{
  // Dummy constructor ALWAYS needed for I/O.
}

//________________________________________________________________________
AliRsnTaskEventPlaneTest::AliRsnTaskEventPlaneTest(const char *name)
    : AliAnalysisTaskSE(name), fOutput(0), fHistEpAngle(0), fFlowQnVectorMgr(0)

{
  // Constructor
  // Define input and output slots here (never in the dummy constructor)
  // Input slot #0 works with a TChain - it is connected to the default input
  // container
  // Output slot #1 writes into a TH1 container
  DefineOutput(1, TList::Class()); // for output list
}

//________________________________________________________________________
AliRsnTaskEventPlaneTest::~AliRsnTaskEventPlaneTest() {
  // Destructor. Clean-up the output list, but not the histograms that are put
  // inside
  // (the list is owner and will clean-up these histograms). Protect in PROOF
  // case.
  if (fOutput && !AliAnalysisManager::GetAnalysisManager()->IsProofMode()) {
    delete fOutput;
  }
}

//________________________________________________________________________
void AliRsnTaskEventPlaneTest::UserCreateOutputObjects() {
  // Create histograms
  // Called once (on the worker node)

  fOutput = new TList();
  fOutput->SetOwner(); // IMPORTANT!

  fHistEpAngle = new TH1D("fHistEpAngle", "EP angle", 360, -180, 180);
  fOutput->Add(fHistEpAngle);

  AliAnalysisTaskFlowVectorCorrections *flowQnVectorTask =
      dynamic_cast<AliAnalysisTaskFlowVectorCorrections *>(
          AliAnalysisManager::GetAnalysisManager()->GetTask(
              "FlowQnVectorCorrections"));
  if (flowQnVectorTask) {
    AliInfo("Using Flow Qn vector corrections framework task ...");
    fFlowQnVectorMgr = flowQnVectorTask->GetAliQnCorrectionsManager();
  }

  // NEW HISTO added to fOutput here
  PostData(1, fOutput); // Post data for ALL output slots >0 here, to get at
                        // least an empty histogram
}

//________________________________________________________________________
void AliRsnTaskEventPlaneTest::UserExec(Option_t *) {
  // Main loop
  // Called for each event

  if (fFlowQnVectorMgr) {
    TList *qnlist = fFlowQnVectorMgr->GetQnVectorList();
    if (!qnlist) {
      Printf("Error: qnlist is null !!!");
      return;
    }

    const AliQnCorrectionsQnVector *qnVect;
    qnVect = GetQnVectorFromList(qnlist, "VZEROA", "latest", "latest");
    if (!qnVect) {
      Printf("Error: qnVect is null !!!");
      return;
    }

    Printf("qnX=%f qnY=%f", qnVect->Qx(1), qnVect->Qy(1));
    Double_t epAngle = TMath::ATan2(qnVect->Qy(1), qnVect->Qx(1)) / 1.;
    Printf("epAngle=%f epAngleDeg=%f", epAngle, epAngle * TMath::RadToDeg());
    fHistEpAngle->Fill(epAngle * TMath::RadToDeg());
  } else {
    Printf("Error: fFlowQnVectorMgr is null !!!");
  }
  // NEW HISTO should be filled before this point, as PostData puts the
  // information for this iteration of the UserExec in the container
  PostData(1, fOutput);
}

//________________________________________________________________________
void AliRsnTaskEventPlaneTest::Terminate(Option_t *) {
  // Draw result to screen, or perform fitting, normalizations
  // Called once at the end of the query
  fOutput = dynamic_cast<TList *>(GetOutputData(1));
  if (!fOutput) {
    Printf("ERROR: could not retrieve TList fOutput");
    return;
  }

  fHistEpAngle = dynamic_cast<TH1D *>(fOutput->FindObject("fHistEpAngle"));
  if (!fHistEpAngle) {
    Printf("ERROR: could not retrieve fHistEpAngle");
    return;
  }
  fHistEpAngle->DrawCopy("E");
}

const AliQnCorrectionsQnVector *AliRsnTaskEventPlaneTest::GetQnVectorFromList(
    const TList *list, const char *subdetector, const char *expectedstep,
    const char *altstep) const {

  AliQnCorrectionsQnVector *theQnVector = NULL;

  TList *pQvecList = dynamic_cast<TList *>(list->FindObject(subdetector));
  if (pQvecList != NULL) {
    /* the detector is present */
    if (TString(expectedstep).EqualTo("latest"))
      theQnVector = (AliQnCorrectionsQnVector *)pQvecList->First();
    else
      theQnVector =
          (AliQnCorrectionsQnVector *)pQvecList->FindObject(expectedstep);

    if (theQnVector == NULL) {
      /* the Qn vector for the expected step was not there */
      if (TString(altstep).EqualTo("latest"))
        theQnVector = (AliQnCorrectionsQnVector *)pQvecList->First();
      else
        theQnVector =
            (AliQnCorrectionsQnVector *)pQvecList->FindObject(altstep);
    }
  }
  if (theQnVector != NULL) {
    /* check the Qn vector quality */
    if (!(theQnVector->IsGoodQuality()) || !(theQnVector->GetN() != 0))
      /* not good quality, discarded */
      theQnVector = NULL;
  }
  return theQnVector;
}
