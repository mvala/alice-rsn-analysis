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

ClassImp(AliRsnTaskEventPlaneTest)

    //________________________________________________________________________
    AliRsnTaskEventPlaneTest::AliRsnTaskEventPlaneTest() // All data members
                                                         // should be
                                                         // initialised
                                                         // here
    : AliAnalysisTaskSE(),
      fOutput(0)
// The last in the above list should not have a comma after it
{
  // Dummy constructor ALWAYS needed for I/O.
}

//________________________________________________________________________
AliRsnTaskEventPlaneTest::AliRsnTaskEventPlaneTest(
    const char *name) // All data members should be initialised here
    : AliAnalysisTaskSE(name),
      fOutput(0)
// The last in the above list should not have a comma after it
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
  delete fTrackCuts;
}

//________________________________________________________________________
void AliRsnTaskEventPlaneTest::UserCreateOutputObjects() {
  // Create histograms
  // Called once (on the worker node)

  fOutput = new TList();
  fOutput->SetOwner(); // IMPORTANT!

  // NEW HISTO added to fOutput here
  PostData(1, fOutput); // Post data for ALL output slots >0 here, to get at
                        // least an empty histogram
}

//________________________________________________________________________
void AliRsnTaskEventPlaneTest::UserExec(Option_t *) {
  // Main loop
  // Called for each event

  // Create pointer to reconstructed event
  AliVEvent *event = InputEvent();
  if (!event) {
    Printf("ERROR: Could not retrieve event");
    return;
  }

  AliESDEvent *esd = dynamic_cast<AliESDEvent *>(event);
  if (esd) {
    Printf("nTracks=%d", esd->GetNumberOfTracks());
    AliESDtrack *esdTrack = 0;
    for (Int_t iTrack = 0; iTrack < esd->GetNumberOfTracks(); iTrack++) {
      esdTrack = (AliESDtrack *)esd->GetTrack(iTrack);
      fHistPt->Fill(esdTrack->Pt());
    }
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
}
