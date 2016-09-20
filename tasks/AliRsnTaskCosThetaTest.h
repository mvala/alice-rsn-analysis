/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
/* AliRsnTaskCosThetaTest.h
 *
 * Template task producing a P_t spectrum and pseudorapidity distribution.
 * Includes explanations of physics and primary track selections
 *
 * Based on tutorial example from offline pages
 * Edited by Arvinder Palaha
 */
#ifndef AliRsnTaskCosThetaTest_H
#define AliRsnTaskCosThetaTest_H

#include "AliAnalysisTaskSE.h"

class TH1F;
class TList;
class AliESDtrackCuts;

class AliRsnTaskCosThetaTest : public AliAnalysisTaskSE {
public:
  AliRsnTaskCosThetaTest();
  AliRsnTaskCosThetaTest(const char *name);
  virtual ~AliRsnTaskCosThetaTest();

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

  AliRsnTaskCosThetaTest(const AliRsnTaskCosThetaTest &); // not implemented
  AliRsnTaskCosThetaTest &
  operator=(const AliRsnTaskCosThetaTest &); // not implemented

  ClassDef(AliRsnTaskCosThetaTest, 1); // example of analysis
};

#endif
