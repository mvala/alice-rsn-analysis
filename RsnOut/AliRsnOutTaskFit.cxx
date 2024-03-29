#include <Riostream.h>
#include <TBufferJSON.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TH1.h>
#include <TMath.h>
#include <TROOT.h>
#include <TStyle.h>

#include <AliRsnOutValue.h>

#include "AliRsnOutTaskFit.h"

ClassImp(AliRsnOutTaskFit);

AliRsnOutTaskFit::AliRsnOutTaskFit(const char *name, const char *title)
    : AliRsnOutTask(name, title), fInput(0), fFitProbTestMin(1e-4),
      fFitProbTestMax(0.6), fFitResult(0), fNUmberOfFits(3),
      fIntegralEps(1.e-4), fMass(1.019445), fWidth(0.00426), fSigma(0.001),
      fResult(0), fResultPar(0) {}

AliRsnOutTaskFit::~AliRsnOutTaskFit() {}

Double_t AliRsnOutTaskFit::Pol1(double *x, double *par) {
  return par[0] + x[0] * par[1];
}

Double_t AliRsnOutTaskFit::Pol2(double *x, double *par) {
  return par[0] + x[0] * par[1] + x[0] * x[0] * par[2];
}

Double_t AliRsnOutTaskFit::Pol3(double *x, double *par) {
  return par[0] + x[0] * par[1] + x[0] * x[0] * par[2] +
         x[0] * x[0] * x[0] * par[3];
}

Double_t AliRsnOutTaskFit::VoigtPol1(double *x, double *par) {
  return par[0] * TMath::Voigt(x[0] - par[1], par[3], par[2]) +
         Pol1(x, &par[4]);
}

Double_t AliRsnOutTaskFit::VoigtPol2(double *x, double *par) {
  return par[0] * TMath::Voigt(x[0] - par[1], par[3], par[2]) +
         Pol2(x, &par[4]);
}

Double_t AliRsnOutTaskFit::VoigtPol3(double *x, double *par) {
  return par[0] * TMath::Voigt(x[0] - par[1], par[3], par[2]) +
         Pol3(x, &par[4]);
}

Double_t AliRsnOutTaskFit::BWPol1(double *x, double *par) {
  return par[0] * TMath::BreitWigner(x[0],par[1], par[2]) + Pol1(x, &par[3]);
}

Double_t AliRsnOutTaskFit::BWPol2(double *x, double *par) {
  return par[0] * TMath::BreitWigner(x[0],par[1], par[2]) + Pol2(x, &par[3]);
}

Double_t AliRsnOutTaskFit::BWPol3(double *x, double *par) {
  return par[0] * TMath::BreitWigner(x[0],par[1], par[2]) + Pol3(x, &par[3]);
}

void AliRsnOutTaskFit::SetFit(AliRsnOutValue *fit) {
  fInput = fit;
  if (fInput) {
    fName = TString::Format("fit_%d[%.3f,%.3f]", fInput->GetId(),
                            fInput->GetMin(), fInput->GetMax());
    fTitle = TString::Format("Fit %d[%.3f,%.3f]", fInput->GetId(),
                             fInput->GetMin(), fInput->GetMax());
  } else {
    fName = "fit";
    fTitle = "Fit";
  }
}

void AliRsnOutTaskFit::Exec(Option_t * /*option*/) {

  // Printf(GetName());
  if (!fParent->GetOutput())
    return;

  if (fResult) {
    SafeDelete(fResult);
  }

  if (fResultPar) {
    SafeDelete(fResultPar);
  }

  Fit(fInput->GetId(), fInput->GetMin(), fInput->GetMax());
}

void AliRsnOutTaskFit::Fit(Int_t fitId, Double_t fitMin, Double_t fitMax) {

  fResult = (TH1 *)fParent->GetOutput()->FindObject("hSignal");
  if (fResult) {

    fResult = (TH1 *)fResult->Clone();
    Double_t p0p =
        fResult->Integral(fResult->FindBin(fitMin), fResult->FindBin(fitMax)) *
        fResult->GetBinWidth(fResult->FindBin(fitMin));

    // Printf("AliRsnOutTaskFit inside: mass=%f width=%f sigma=%f", fMass,
    // fWidth, fSigma);
    TF1 *sigBgFnc = 0;
    TF1 *bgFnc = 0;
    switch (fitId) {
    case kVoightPol1:
      sigBgFnc =
          new TF1("VoightPol1", AliRsnOutTaskFit::VoigtPol1, fitMin, fitMax, 6);
      sigBgFnc->SetParNames("yield", "mass", "width", "sigma", "p0", "p1", "p2",
                            "p3");

      sigBgFnc->SetParameters(p0p, fMass, fWidth, fSigma, 0.0, 0.0, 0.0, 0.0);
      bgFnc = new TF1("Pol1", Pol1, fitMin, fitMax, 2);
      break;
    case kVoightPol2:
      sigBgFnc =
          new TF1("VoightPol2", AliRsnOutTaskFit::VoigtPol2, fitMin, fitMax, 7);
      sigBgFnc->SetParNames("yield", "mass", "width", "sigma", "p0", "p1", "p2",
                            "p3");

      sigBgFnc->SetParameters(p0p, fMass, fWidth, fSigma, 0.0, 0.0, 0.0, 0.0);
      bgFnc = new TF1("Pol2", Pol2, fitMin, fitMax, 3);
      break;
    case kVoightPol3:
      sigBgFnc =
          new TF1("VoightPol3", AliRsnOutTaskFit::VoigtPol3, fitMin, fitMax, 8);
      sigBgFnc->SetParNames("yield", "mass", "width", "sigma", "p0", "p1", "p2",
                            "p3");

      sigBgFnc->SetParameters(p0p, fMass, fWidth, fSigma, 0.0, 0.0, 0.0, 0.0);
      bgFnc = new TF1("Pol3", Pol3, fitMin, fitMax, 4);
      break;
    case kBWPol1:
      sigBgFnc =
          new TF1("BWPol1", AliRsnOutTaskFit::BWPol1, fitMin, fitMax, 5);
      sigBgFnc->SetParNames("yield", "mass", "width", "p0", "p1", "p2", "p3");

      sigBgFnc->SetParameters(p0p, fMass, fWidth, 0.0, 0.0, 0.0, 0.0);
      bgFnc = new TF1("Pol1", Pol1, fitMin, fitMax, 2);
      break;
    case kBWPol2:
      sigBgFnc =
          new TF1("BWPol2", AliRsnOutTaskFit::BWPol2, fitMin, fitMax, 6);
      sigBgFnc->SetParNames("yield", "mass", "width", "p0", "p1", "p2", "p3");
      sigBgFnc->SetParameters(p0p, fMass, fWidth, 0.0, 0.0, 0.0, 0.0);
      bgFnc = new TF1("Pol2", Pol2, fitMin, fitMax, 3);
      break;
    case kBWPol3:
      sigBgFnc =
          new TF1("kBWPol3", AliRsnOutTaskFit::BWPol3, fitMin, fitMax, 7);
      sigBgFnc->SetParNames("yield", "mass", "width", "p0", "p1", "p2", "p3");
      sigBgFnc->SetParameters(p0p, fMass, fWidth, 0.0, 0.0, 0.0, 0.0);
      bgFnc = new TF1("Pol3", Pol3, fitMin, fitMax, 4);
      break;
    }
    if (!sigBgFnc || !bgFnc)
      return;

 sigBgFnc->FixParameter(3, fSigma);
 // sigBgFnc->FixParameter(2, fWidth);
  //sigBgFnc->FixParameter(1, fMass);

    for (Int_t i = 0; i < fNUmberOfFits; i++) {
      fFitResult = fResult->Fit(sigBgFnc, "QN MFC", "", fitMin, fitMax);
    }

    fFitResult = fResult->Fit(sigBgFnc, "QN MF S", "", fitMin, fitMax);
    // Printf("fFitResult->IsValid() = %d", (Int_t)fFitResult->IsValid());
    Double_t par[6];
    sigBgFnc->GetParameters(par);
    const Double_t *parErr = sigBgFnc->GetParErrors();
    bgFnc->SetParameters(&par[4]);

    if ((fFitResult->Prob() < fFitProbTestMin) ||
        (fFitResult->Prob() > fFitProbTestMax)) {
      sigBgFnc->SetLineColor(kRed);
      bgFnc->SetLineColor(kRed);
    }

    fResult->GetListOfFunctions()->Add(sigBgFnc);
    fResult->GetListOfFunctions()->Add(bgFnc);
    fOutput->Add(fResult);

    Double_t val, err;
    const Int_t nBins = 6 + sigBgFnc->GetNpar();
    fResultPar = new TH1D("fResultPar", "Result parameters", nBins, 0, nBins);

    Int_t iBin = 1;
    GetYieldBinCounting(val, err);
    fResultPar->SetBinContent(iBin, val);
    fResultPar->SetBinError(iBin, err);
    fResultPar->GetXaxis()->SetBinLabel(iBin, "IntBC");
    iBin++;

    GetYieldFitFunction(val, err);
    fResultPar->SetBinContent(iBin, val);
    fResultPar->SetBinError(iBin, err);
    fResultPar->GetXaxis()->SetBinLabel(iBin, "IntFF");
    iBin++;

    fResultPar->SetBinContent(iBin, GetChi2());
    fResultPar->SetBinError(iBin, 0);
    fResultPar->GetXaxis()->SetBinLabel(iBin, "Chi2");
    iBin++;

    fResultPar->SetBinContent(iBin, GetNdf());
    fResultPar->SetBinError(iBin, 0);
    fResultPar->GetXaxis()->SetBinLabel(iBin, "Ndf");
    iBin++;

    fResultPar->SetBinContent(iBin, GetReducedChi2());
    fResultPar->SetBinError(iBin, 0);
    fResultPar->GetXaxis()->SetBinLabel(iBin, "ReducedChi2");
    iBin++;

    fResultPar->SetBinContent(iBin, GetProb());
    fResultPar->SetBinError(iBin, 0);
    fResultPar->GetXaxis()->SetBinLabel(iBin, "Prob");
    iBin++;

    for (Int_t i = 0; i < sigBgFnc->GetNpar(); i++) {
      fResultPar->SetBinContent(iBin, par[i]);
      fResultPar->SetBinError(iBin, parErr[i]);
      fResultPar->GetXaxis()->SetBinLabel(iBin, sigBgFnc->GetParName(i));
      iBin++;
    }

    fOutput->Add(fResultPar);
  }
}

void AliRsnOutTaskFit::SetProbTest(Double_t min, Double_t max) {
  fFitProbTestMin = min;
  fFitProbTestMax = max;
}

void AliRsnOutTaskFit::GetYieldBinCounting(Double_t &val, Double_t &err) {

  if (!fResult)
    fResult = (TH1 *)fOutput->FindObject("hSignal");

  Double_t bin_min = fResult->FindBin(fInput->GetMin());
  Double_t bin_max = fResult->FindBin(fInput->GetMax());
  Double_t min = fInput->GetMin();
  Double_t max = fInput->GetMax();

  Double_t histWidth = fResult->GetXaxis()->GetBinWidth(1);

  val = fResult->IntegralAndError(bin_min, bin_max, err);

  TF1 *bgFnc = (TF1 *)fResult->GetListOfFunctions()->At(1);

  Double_t bg = bgFnc->Integral(min, max, fIntegralEps);

  // TODO Verify it
  Double_t bgErr = bgFnc->IntegralError(
      min, max, fFitResult->GetParams(),
      fFitResult->GetCovarianceMatrix().GetMatrixArray(), fIntegralEps);

  bg /= histWidth;
  bgErr /= histWidth;

  val -= bg;
  err = TMath::Sqrt(TMath::Power(err, 2) + TMath::Power(bgErr, 2));
}

void AliRsnOutTaskFit::GetYieldFitFunction(Double_t &val, Double_t &err) {

  if (!fResult)
    fResult = (TH1 *)fOutput->FindObject("hSignal");

  Double_t min = fInput->GetMin();
  Double_t max = fInput->GetMax();

  Double_t histWidth = fResult->GetXaxis()->GetBinWidth(1);

  TF1 *sigBgFnc = (TF1 *)fResult->GetListOfFunctions()->At(0);
  TF1 *bgFnc = (TF1 *)fResult->GetListOfFunctions()->At(1);

  val = sigBgFnc->Integral(min, max, fIntegralEps);
  err = sigBgFnc->IntegralError(
      min, max, fFitResult->GetParams(),
      fFitResult->GetCovarianceMatrix().GetMatrixArray(), fIntegralEps);

  val /= histWidth;
  err /= histWidth;

  Double_t bg = bgFnc->Integral(min, max, fIntegralEps);

  // TODO Verify it
  Double_t bgErr = bgFnc->IntegralError(
      min, max, fFitResult->GetParams(),
      fFitResult->GetCovarianceMatrix().GetMatrixArray(), fIntegralEps);

  bg /= histWidth;
  bgErr /= histWidth;

  val -= bg;
  err = TMath::Sqrt(TMath::Power(err, 2) + TMath::Power(bgErr, 2));
}
Double_t AliRsnOutTaskFit::GetChi2() { return fFitResult->Chi2(); }

Double_t AliRsnOutTaskFit::GetNdf() { return fFitResult->Ndf(); }

Double_t AliRsnOutTaskFit::GetReducedChi2() {
  return fFitResult->Chi2() / fFitResult->Ndf();
}

Double_t AliRsnOutTaskFit::GetProb() { return fFitResult->Prob(); }
