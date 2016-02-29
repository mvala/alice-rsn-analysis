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

/* AliRsnTaskTest.cxx
 *
 * Template task producing a P_t spectrum and pseudorapidity distribution.
 * Includes explanations of physics and primary track selections
 *
 * Instructions for adding histograms can be found below, starting with NEW HISTO
 *
 * Based on tutorial example from offline pages
 * Edited by Arvinder Palaha
 */
#include "AliRsnTaskTest.h"

#include "Riostream.h"
#include "TChain.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TList.h"

#include "AliAnalysisTaskSE.h"
#include "AliAnalysisManager.h"
#include "AliStack.h"
#include "AliESDtrackCuts.h"
#include "AliESDEvent.h"
#include "AliESDInputHandler.h"
#include "AliAODEvent.h"
#include "AliMCEvent.h"

ClassImp(AliRsnTaskTest)

//________________________________________________________________________
AliRsnTaskTest::AliRsnTaskTest() // All data members should be initialised here
   :AliAnalysisTaskSE(),
    fOutput(0),
    fTrackCuts(0),
    fHistPt(0),
	fHistCosThetaJ(0),
	fHistCosThetaT(0),
	fHistCosThetaH(0),
	fHistCosThetaA(0) // The last in the above list should not have a comma after it
{
    // Dummy constructor ALWAYS needed for I/O.
}

//________________________________________________________________________
AliRsnTaskTest::AliRsnTaskTest(const char *name) // All data members should be initialised here
   :AliAnalysisTaskSE(name),
    fOutput(0),
    fTrackCuts(0),
    fHistPt(0),
	fHistCosThetaJ(0),
	fHistCosThetaT(0),
	fHistCosThetaH(0),
	fHistCosThetaA(0) // The last in the above list should not have a comma after it
{
    // Constructor
    // Define input and output slots here (never in the dummy constructor)
    // Input slot #0 works with a TChain - it is connected to the default input container
    // Output slot #1 writes into a TH1 container
    DefineOutput(1, TList::Class());                                            // for output list
}

//________________________________________________________________________
AliRsnTaskTest::~AliRsnTaskTest()
{
    // Destructor. Clean-up the output list, but not the histograms that are put inside
    // (the list is owner and will clean-up these histograms). Protect in PROOF case.
    if (fOutput && !AliAnalysisManager::GetAnalysisManager()->IsProofMode()) {
        delete fOutput;
    }
    delete fTrackCuts;
}

//________________________________________________________________________
void AliRsnTaskTest::UserCreateOutputObjects()
{
    // Create histograms
    // Called once (on the worker node)
        
    fOutput = new TList();
    fOutput->SetOwner();  // IMPORTANT!

    fTrackCuts = AliESDtrackCuts::GetStandardITSTPCTrackCuts2010(kTRUE);
    // === Primary Track Selection ===
    //
    // The definition of a primary track is taken from the ALICE Twiki
    // page https://twiki.cern.ch/twiki/bin/view/ALICE/SelectionOfPrimaryTracksForPpDataAnalysis
    // using the following parameters for a standard dN/dPt analysis:
    //  track quality cuts:
    //          esdTrackCuts->SetMinNClustersTPC(70);
    //          esdTrackCuts->SetMaxChi2PerClusterTPC(4);
    //          esdTrackCuts->SetAcceptKinkDaughters(kFALSE);
    //          esdTrackCuts->SetRequireTPCRefit(kTRUE);
    //          esdTrackCuts->SetRequireITSRefit(kTRUE);
    //          esdTrackCuts->SetClusterRequirementITS(AliESDtrackCuts::kSPD,
    //                                                                                     AliESDtrackCuts::kAny);
    //  dca:
    //          if(selPrimaries) {
    //                  // 7*(0.0026+0.0050/pt^1.01)
    //                  esdTrackCuts->SetMaxDCAToVertexXYPtDep("0.0182+0.0350/pt^1.01");
    //          }
    //          esdTrackCuts->SetMaxDCAToVertexZ(2);
    //          esdTrackCuts->SetDCAToVertex2D(kFALSE);
    //          esdTrackCuts->SetRequireSigmaToVertex(kFALSE);
    //
    // The Primary Track Selection is implemented here by creating an
    // AliESDtrackCuts object, with kTRUE argument to choose primary tracks.
    //
    // By default, it is set to the above conditions which are suitable for
    // a standard inclusive dN/dPt analysis. For others, such as identified
    // dN/dPt or strangeness as well as others, follow the above link for
    // the specific changes to include in the selection.

    // To change cuts after selecting some default set, one can use
    // esdtrackcuts->SetMinNClustersTPC(70) for example

    // Create histograms
    Int_t ptbins = 15;
    Float_t ptlow = 0.1, ptup = 3.1;
    fHistPt = new TH1F("fHistPt", "P_{T}", ptbins, ptlow, ptup);
    fHistPt->GetXaxis()->SetTitle("P_{T} (GeV/c)");
    fHistPt->GetYaxis()->SetTitle("dN/dP_{T} (c/GeV)");
    fHistPt->SetMarkerStyle(kFullCircle);

    fHistCosThetaJ = new TH1F("fHistCosThetaJ", "cos(#theta) Jackson frame", 15, -1, 1);
    fHistCosThetaJ->GetXaxis()->SetTitle("cos(#theta)");
    fHistCosThetaJ->GetYaxis()->SetTitle("N");
    fHistCosThetaJ->SetMarkerStyle(kFullCircle);

    fHistCosThetaT = new TH1F("fHistCosThetaT", "cos(#theta) Transverse frame", 15, -1, 1);
    fHistCosThetaT->GetXaxis()->SetTitle("cos(#theta)");
    fHistCosThetaT->GetYaxis()->SetTitle("N");
    fHistCosThetaT->SetMarkerStyle(kFullCircle);

    fHistCosThetaH = new TH1F("fHistCosThetaH", "cos(#theta) Helicity frame", 15, -1, 1);
    fHistCosThetaH->GetXaxis()->SetTitle("cos(#theta)");
    fHistCosThetaH->GetYaxis()->SetTitle("N");
    fHistCosThetaH->SetMarkerStyle(kFullCircle);

    fHistCosThetaA = new TH1F("fHistCosThetaA", "cos(#theta) Adair Frame", 15, -1, 1);
    fHistCosThetaA->GetXaxis()->SetTitle("cos(#theta)");
    fHistCosThetaA->GetYaxis()->SetTitle("N");
    fHistCosThetaA->SetMarkerStyle(kFullCircle);


    // NEW HISTO should be defined here, with a sensible name,
        
    fOutput->Add(fHistPt);
    fOutput->Add(fHistCosThetaJ);
    fOutput->Add(fHistCosThetaT);
    fOutput->Add(fHistCosThetaH);
    fOutput->Add(fHistCosThetaA);
    // NEW HISTO added to fOutput here
    PostData(1, fOutput); // Post data for ALL output slots >0 here, to get at least an empty histogram
}

//________________________________________________________________________
void AliRsnTaskTest::UserExec(Option_t *) 
{
    // Main loop
    // Called for each event
        
        
    // Create pointer to reconstructed event
	AliVEvent *event = InputEvent();
	if (!event) {
		Printf("ERROR: Could not retrieve event");
		return;
	}

	AliESDEvent* esd = dynamic_cast<AliESDEvent*>(event);
	if (esd) {

		AliMCEvent *mcEvent = MCEvent();
		if (!mcEvent) {
			Printf("ERROR: Could not retrieve MC event");
			return;
		}
		Printf("MC particles: %d", mcEvent->GetNumberOfTracks());

		// Track loop for reconstructed event
		Int_t ntracks = mcEvent->GetNumberOfTracks();
		for (Int_t i = 0; i < ntracks; i++) {
			AliMCParticle* esdMcTrack = (AliMCParticle*)mcEvent->GetTrack(i); // pointer to reconstructed to track
			if (!esdMcTrack) {
				AliError(Form("ERROR: Could not retrieve esdtrack %d", i));
				continue;
			}
			if (esdMcTrack->PdgCode() == 333) {

				AliMCParticle* k1 = (AliMCParticle*)mcEvent->GetTrack(esdMcTrack->GetFirstDaughter());
				AliMCParticle* k2 = (AliMCParticle*)mcEvent->GetTrack(esdMcTrack->GetLastDaughter());
				if ((TMath::Abs(k1->PdgCode())) == 321 && (TMath::Abs(k2->PdgCode()) == 321)) {
				Printf("E=%f pdg=%d", esdMcTrack->E(),esdMcTrack->PdgCode());


				TLorentzVector k1_lab(k1->Px(),k1->Py(),k1->Pz(),k1->E());
				TLorentzVector k2_lab(k2->Px(),k2->Py(),k2->Pz(),k2->E());
				TLorentzVector phi_lab = k1_lab + k2_lab;

				Printf("  K1 : pt=%f pdg=%d", k1->Pt(),k1->PdgCode());
				Printf("  K2 : pt=%f pdg=%d", k2->Pt(),k2->PdgCode());

				Printf("  TLorentzVector K1 (Lab): ");
				k1_lab.Print();
				Printf("  TLorentzVector K2 (Lab): ");
				k2_lab.Print();
				Printf("  TLorentzVector Phi (Lab): ");
				phi_lab.Print();


				TLorentzVector k1_phi = k1_lab;
				TLorentzVector k2_phi = k2_lab;
				TLorentzVector phi_phi = phi_lab;


				k1_phi.Boost(-phi_lab.BoostVector());
				k2_phi.Boost(-phi_lab.BoostVector());
//				phi_phi.Boost(-phi_lab.BoostVector());

				Printf("  TLorentzVector K1 (Phi): ");
				k1_phi.Print();
				Printf("  TLorentzVector K2 (Phi): ");
				k2_phi.Print();
//				Printf("  TLorentzVector Phi (Phi): ");
//				phi_phi.Print();

//				Double_t cosTheta = phi_lab.Angle(k1_phi.Vect());
//				Double_t cosTheta = phi_lab.Dot(k1_phi) / phi_lab.Mag();
				Double_t cosTheta = TMath::Cos(k1_phi.Theta());
				Printf("  cos(theta) : %f", cosTheta);

				fHistPt->Fill(esdMcTrack->Pt());
				fHistCosThetaJ->Fill(cosTheta);
				}
			}
		}
	}
	// NEW HISTO should be filled before this point, as PostData puts the
	// information for this iteration of the UserExec in the container
	PostData(1, fOutput);
}


//________________________________________________________________________
void AliRsnTaskTest::Terminate(Option_t *) 
{
    // Draw result to screen, or perform fitting, normalizations
    // Called once at the end of the query
    fOutput = dynamic_cast<TList*> (GetOutputData(1));
    if(!fOutput) { Printf("ERROR: could not retrieve TList fOutput"); return; }
        
    fHistPt = dynamic_cast<TH1F*> (fOutput->FindObject("fHistPt"));
    if (!fHistPt) { Printf("ERROR: could not retrieve fHistPt"); return;}

    fHistCosThetaJ = dynamic_cast<TH1F*> (fOutput->FindObject("fHistCosThetaJ"));
    if (!fHistCosThetaJ) { Printf("ERROR: could not retrieve fHistCosThetaJ"); return;}
    fHistCosThetaT = dynamic_cast<TH1F*> (fOutput->FindObject("fHistCosThetaT"));
    if (!fHistCosThetaT) { Printf("ERROR: could not retrieve fHistCosThetaT"); return;}
    fHistCosThetaH = dynamic_cast<TH1F*> (fOutput->FindObject("fHistCosThetaH"));
    if (!fHistCosThetaH) { Printf("ERROR: could not retrieve fHistCosThetaH"); return;}
    fHistCosThetaA = dynamic_cast<TH1F*> (fOutput->FindObject("fHistCosThetaA"));
    if (!fHistCosThetaA) { Printf("ERROR: could not retrieve fHistCosThetaA"); return;}

    // Get the physics selection histograms with the selection statistics
    //AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
    //AliESDInputHandler *inputH = dynamic_cast<AliESDInputHandler*>(mgr->GetInputEventHandler());
    //TH2F *histStat = (TH2F*)inputH->GetStatistics();
   
   
    // NEW HISTO should be retrieved from the TList container in the above way,
    // so it is available to draw on a canvas such as below

    TCanvas *c = new TCanvas("AliRsnTaskTest","P_{T}",10,10,1020,510);
    c->Divide(2,1);
    c->cd(1)->SetLogy();
    fHistPt->DrawCopy("E");
    c->cd(2);
    fHistCosThetaJ->DrawCopy("E");
}
