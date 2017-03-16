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
      fIntegralEps(1.e-4), fResult(0), fResultPar(0) {}

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

    const Double_t phi_mass = 1.019445;
    const Double_t phi_width = 0.00426;
    const Double_t phi_sigma = 0.001;
    // const Double_t hist_min = fResult->GetXaxis()->GetXmin();
    // const Double_t hist_max = fResult->GetXaxis()->GetXmax();
    TF1 *sigBgFnc = 0;
    TF1 *bgFnc = 0;
    switch (fitId) {
    case kVoightPol1:
      sigBgFnc =
          new TF1("VoightPol1", AliRsnOutTaskFit::VoigtPol1, fitMin, fitMax, 6);
      bgFnc = new TF1("Pol1", Pol1, fitMin, fitMax, 2);
      break;
    case kVoightPol2:
      sigBgFnc =
          new TF1("VoightPol2", AliRsnOutTaskFit::VoigtPol2, fitMin, fitMax, 7);
      bgFnc = new TF1("Pol2", Pol2, fitMin, fitMax, 3);
      break;
    case kVoightPol3:
      sigBgFnc =
          new TF1("VoightPol3", AliRsnOutTaskFit::VoigtPol3, fitMin, fitMax, 8);
      bgFnc = new TF1("Pol3", Pol3, fitMin, fitMax, 4);
      break;
    }
    if (!sigBgFnc || !bgFnc)
      return;

    Double_t p0p =
        fResult->Integral(fResult->FindBin(fitMin), fResult->FindBin(fitMax)) *
        fResult->GetBinWidth(fResult->FindBin(fitMin));
    sigBgFnc->SetParameters(p0p, phi_mass, phi_width, phi_sigma, 0.0, 0.0, 0.0,
                            0.0);

    sigBgFnc->SetParNames("yield", "mass", "width", "sigma", "p0", "p1", "p2",
                          "p3");
    sigBgFnc->FixParameter(3, phi_sigma);
    for (Int_t i = 0; i < fNUmberOfFits; i++) {
      fFitResult = fResult->Fit(sigBgFnc, "QN MFC", "", fitMin, fitMax);
    }

    fFitResult = fResult->Fit(sigBgFnc, "Q0 MF S", "", fitMin, fitMax);
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
    return;
  // if (!fResult)
  // fResult = (TH1 *)fOutput->FindObject("hSignal");

  Double_t min = fResult->FindBin(fInput->GetMin());
  Double_t max = fResult->FindBin(fInput->GetMax());

  Double_t histWidth = fResult->GetXaxis()->GetBinWidth(1);

  val = fResult->IntegralAndError(min, max, err);
  Printf("min=%.2f max=%.2f histWidth=%f val=%f err=%f", min, max, histWidth,
         val, err);

  //	TF1 *sigBgFnc = (TF1*) fResult->GetListOfFunctions()->At(0);
  TF1 *bgFnc = (TF1 *)fResult->GetListOfFunctions()->At(1);

  Double_t bg =
      bgFnc->Integral(fInput->GetMin(), fInput->GetMax(), fIntegralEps);

  // TODO Verify it
  Double_t bgErr = bgFnc->IntegralError(
      fInput->GetMin(), fInput->GetMax(), fFitResult->GetParams(),
      fFitResult->GetCovarianceMatrix().GetMatrixArray(), fIntegralEps);

  Printf("BC min=%.2f max=%.2f histWidth=%f bg=%f bgErr=%f", min, max,
         histWidth, bg, bgErr);

  bg /= histWidth;
  bgErr /= histWidth;

  Printf("BC min=%.2f max=%.2f histWidth=%f bg=%f bgErr=%f after", min, max,
         histWidth, bg, bgErr);
  // //	Printf("val=%f err=%f bg=%f bgErr=%f",val,err,bg,bgErr);

  val -= bg;
  err = TMath::Sqrt(TMath::Power(err, 2) + TMath::Power(bgErr, 2));
  Printf("BC min=%.2f max=%.2f histWidth=%f val=%f err=%f final", min, max,
         histWidth, val, err);
}

void AliRsnOutTaskFit::GetYieldFitFunction(Double_t &val, Double_t &err) {

  if (!fResult)
    return;
  // if (!fResult)
  // fResult = (TH1 *)fOutput->FindObject("hSignal");

  Double_t min = fResult->FindBin(fInput->GetMin());
  Double_t max = fResult->FindBin(fInput->GetMax());

  Double_t histWidth = fResult->GetXaxis()->GetBinWidth(1);

  val = fResult->IntegralAndError(min, max, err);

  TF1 *sigBgFnc = (TF1 *)fResult->GetListOfFunctions()->At(0);
  TF1 *bgFnc = (TF1 *)fResult->GetListOfFunctions()->At(1);

  val = sigBgFnc->Integral(fInput->GetMin(), fInput->GetMax(), fIntegralEps) /
        histWidth;
  err = sigBgFnc->IntegralError(
            fInput->GetMin(), fInput->GetMax(), fFitResult->GetParams(),
            fFitResult->GetCovarianceMatrix().GetMatrixArray(), fIntegralEps) /
        histWidth;

  Printf("FF min=%.2f max=%.2f histWidth=%f val=%f err=%f", min, max, histWidth,
         val, err);

  Double_t bg =
      bgFnc->Integral(fInput->GetMin(), fInput->GetMax(), fIntegralEps);
  // TODO Verify it
  Double_t bgErr = bgFnc->IntegralError(
      fInput->GetMin(), fInput->GetMax(), fFitResult->GetParams(),
      fFitResult->GetCovarianceMatrix().GetMatrixArray(), fIntegralEps);

  Printf("FF min=%.2f max=%.2f histWidth=%f bg=%f bgErr=%f", min, max,
         histWidth, bg, bgErr);
  bg /= histWidth;
  bgErr /= histWidth;
  // Printf("val=%f err=%f bg=%f bgErr=%f", val, err, bg, bgErr);

  val -= bg;
  err = TMath::Sqrt(TMath::Power(err, 2) + TMath::Power(bgErr, 2));
  Printf("FF min=%.2f max=%.2f histWidth=%f val=%f err=%f final", min, max,
         histWidth, val, err);
}
Double_t AliRsnOutTaskFit::GetChi2() { return fFitResult->Chi2(); }

Double_t AliRsnOutTaskFit::GetNdf() { return fFitResult->Ndf(); }

Double_t AliRsnOutTaskFit::GetReducedChi2() {
  return fFitResult->Chi2() / fFitResult->Ndf();
}

Double_t AliRsnOutTaskFit::GetProb() { return fFitResult->Prob(); }
