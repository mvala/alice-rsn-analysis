/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
/* AliRsnTaskTestCosTheta.h
 *
 * Template task producing a P_t spectrum and pseudorapidity distribution.
 * Includes explanations of physics and primary track selections
 *
 * Based on tutorial example from offline pages
 * Edited by Arvinder Palaha
 */
#ifndef AliRsnTaskTestCosTheta_H
#define AliRsnTaskTestCosTheta_H

#include "AliAnalysisTaskSE.h"

class TH1F;
class TList;
class AliESDtrackCuts;

class AliRsnTaskTestCosTheta : public AliAnalysisTaskSE {
public:
  AliRsnTaskTestCosTheta();
  AliRsnTaskTestCosTheta(const char *name);
  virtual ~AliRsnTaskTestCosTheta();

  virtual void UserCreateOutputObjects();
  virtual void UserExec(Option_t *option);
  virtual void Terminate(Option_t *);

private:
  TList *fOutput;              // Output list
  AliESDtrackCuts *fTrackCuts; // Track cuts
  TH1F *fHistPt;               // Pt spectrum
  TH1F *fHistCosThetaJ;        // Cos(theta) Jackson frame
  TH1F *fHistCosThetaT;        // Cos(theta) Transverse frame
  TH1F *fHistCosThetaH;        // Cos(theta) Helicity frame
  TH1F *fHistCosThetaA;        // Cos(theta) Adair frame

  AliRsnTaskTestCosTheta(const AliRsnTaskTestCosTheta &); // not implemented
  AliRsnTaskTestCosTheta &
  operator=(const AliRsnTaskTestCosTheta &); // not implemented

  ClassDef(AliRsnTaskTestCosTheta, 1); // example of analysis
};

#endif
