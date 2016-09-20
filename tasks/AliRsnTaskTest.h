/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
/* AliRsnTaskTest.h
 *
 * Template task producing a P_t spectrum and pseudorapidity distribution.
 * Includes explanations of physics and primary track selections
 *
 * Based on tutorial example from offline pages
 * Edited by Arvinder Palaha
 */
#ifndef ALIRSNTASKTEST_H
#define ALIRSNTASKTEST_H

#include "AliAnalysisTaskSE.h"

class TH1F;
class TList;
class AliESDtrackCuts;

class AliRsnTaskTest : public AliAnalysisTaskSE {
public:
  AliRsnTaskTest();
  AliRsnTaskTest(const char *name);
  virtual ~AliRsnTaskTest();

  virtual void UserCreateOutputObjects();
  virtual void UserExec(Option_t *option);
  virtual void Terminate(Option_t *);

private:
  TList *fOutput;              // Output list
  AliESDtrackCuts *fTrackCuts; // Track cuts
  TH1F *fHistPt;               // Pt spectrum

  AliRsnTaskTest(const AliRsnTaskTest &);            // not implemented
  AliRsnTaskTest &operator=(const AliRsnTaskTest &); // not implemented

  ClassDef(AliRsnTaskTest, 1); // example of analysis
};

#endif
