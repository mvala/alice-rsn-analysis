/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
/* AliRsnTaskEventPlaneTest.h
 *
 * Template task producing a P_t spectrum and pseudorapidity distribution.
 * Includes explanations of physics and primary track selections
 *
 * Based on tutorial example from offline pages
 * Edited by Arvinder Palaha
 */
#ifndef AliRsnTaskEventPlaneTest_H
#define AliRsnTaskEventPlaneTest_H

#include "AliAnalysisTaskSE.h"

class TH1F;
class TList;
class AliESDtrackCuts;
class AliQnCorrectionsQnVector;
class AliQnCorrectionsManager;

class AliRsnTaskEventPlaneTest : public AliAnalysisTaskSE {
public:
  AliRsnTaskEventPlaneTest();
  AliRsnTaskEventPlaneTest(const char *name);
  virtual ~AliRsnTaskEventPlaneTest();

  virtual void UserCreateOutputObjects();
  virtual void UserExec(Option_t *option);
  virtual void Terminate(Option_t *);

  const AliQnCorrectionsQnVector *
  GetQnVectorFromList(const TList *list, const char *subdetector,
                      const char *expectedstep, const char *altstep) const;

private:
  TList *fOutput; // Output list
  TH1D *fHistEpAngle;

  AliQnCorrectionsManager *fFlowQnVectorMgr;

  AliRsnTaskEventPlaneTest(const AliRsnTaskEventPlaneTest &); // not implemented
  AliRsnTaskEventPlaneTest &
  operator=(const AliRsnTaskEventPlaneTest &); // not implemented

  ClassDef(AliRsnTaskEventPlaneTest, 1); // example of analysis
};

#endif
