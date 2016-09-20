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

ClassImp(AliRsnOutTaskFit)

    AliRsnOutTaskFit::AliRsnOutTaskFit(const char *name, const char *title)
    : AliRsnOutTask(name, title), fInput(0), fFitProbTestMin(1e-4),
      fFitProbTestMax(0.6), fFitResult(0), fResult(0) {}

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

  Fit(fInput->GetId(), fInput->GetMin(), fInput->GetMax());
}

void AliRsnOutTaskFit::Fit(Int_t fitId, Double_t fitMin, Double_t fitMax) {

  Printf(GetName());
  if (!fParent->GetOutput())
    return;

  fResult = (TH1 *)fParent->GetOutput()->FindObject("hSignal");
  if (fResult) {
    fResult = (TH1 *)fResult->Clone();

    const Double_t phi_mass = 1.019445;
    const Double_t phi_width = 0.00426;
    const Double_t phi_sigma = 0.001;

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

    fFitResult = fResult->Fit(sigBgFnc, "QN MFC S", "", fitMin, fitMax);
    fFitResult = fResult->Fit(sigBgFnc, "QN MFC S", "", fitMin, fitMax);
    fFitResult = fResult->Fit(sigBgFnc, "QN MFC S", "", fitMin, fitMax);
    fFitResult = fResult->Fit(sigBgFnc, "QN MF S", "", fitMin, fitMax);
    //	fFitResult->Print();
    Double_t par[6];
    sigBgFnc->GetParameters(par);
    bgFnc->SetParameters(&par[4]);

    if ((fFitResult->Prob() < fFitProbTestMin) ||
        (fFitResult->Prob() > fFitProbTestMax)) {
      sigBgFnc->SetLineColor(kRed);
      bgFnc->SetLineColor(kRed);
    }

    fResult->GetListOfFunctions()->Add(sigBgFnc);
    fResult->GetListOfFunctions()->Add(bgFnc);
    fOutput->Add(fResult);
  }
}

void AliRsnOutTaskFit::SetProbTest(Double_t min, Double_t max) {
  fFitProbTestMin = min;
  fFitProbTestMax = max;
}

void AliRsnOutTaskFit::GetYieldBinCounting(Double_t &val, Double_t &err) {

  Double_t min = fResult->FindBin(fInput->GetMin());
  Double_t max = fResult->FindBin(fInput->GetMax());

  Double_t histWidth = fResult->GetXaxis()->GetBinWidth(1);

  val = fResult->IntegralAndError(min, max, err);

  //	TF1 *sigBgFnc = (TF1*) fResult->GetListOfFunctions()->At(0);
  TF1 *bgFnc = (TF1 *)fResult->GetListOfFunctions()->At(1);

  Double_t bg = bgFnc->Integral(fInput->GetMin(), fInput->GetMax()) / histWidth;
  // TODO Verify it
  Double_t bgErr =
      bgFnc->IntegralError(fInput->GetMin(), fInput->GetMax(),
                           fFitResult->GetParams(),
                           fFitResult->GetCovarianceMatrix().GetMatrixArray()) /
      histWidth;

  //	Printf("val=%f err=%f bg=%f bgErr=%f",val,err,bg,bgErr);

  val -= bg;
  err = TMath::Sqrt(TMath::Power(err, 2) + TMath::Power(bgErr, 2));
}

void AliRsnOutTaskFit::GetYieldFitFunction(Double_t &val, Double_t &err) {
  Double_t min = fResult->FindBin(fInput->GetMin());
  Double_t max = fResult->FindBin(fInput->GetMax());

  Double_t histWidth = fResult->GetXaxis()->GetBinWidth(1);

  val = fResult->IntegralAndError(min, max, err);

  TF1 *sigBgFnc = (TF1 *)fResult->GetListOfFunctions()->At(0);
  TF1 *bgFnc = (TF1 *)fResult->GetListOfFunctions()->At(1);

  val = sigBgFnc->Integral(fInput->GetMin(), fInput->GetMax()) / histWidth;
  err = sigBgFnc->IntegralError(
            fInput->GetMin(), fInput->GetMax(), fFitResult->GetParams(),
            fFitResult->GetCovarianceMatrix().GetMatrixArray()) /
        histWidth;

  Double_t bg = bgFnc->Integral(fInput->GetMin(), fInput->GetMax()) / histWidth;
  // TODO Verify it
  Double_t bgErr =
      bgFnc->IntegralError(fInput->GetMin(), fInput->GetMax(),
                           fFitResult->GetParams(),
                           fFitResult->GetCovarianceMatrix().GetMatrixArray()) /
      histWidth;

  //	Printf("val=%f err=%f bg=%f bgErr=%f",val,err,bg,bgErr);

  val -= bg;
  err = TMath::Sqrt(TMath::Power(err, 2) + TMath::Power(bgErr, 2));
}
Double_t AliRsnOutTaskFit::GetChi2() { return fFitResult->Chi2(); }

Double_t AliRsnOutTaskFit::GetNdf() { return fFitResult->Ndf(); }

Double_t AliRsnOutTaskFit::GetReducedChi2() {
  return fFitResult->Chi2() / fFitResult->Ndf();
}

Double_t AliRsnOutTaskFit::GetProb() { return fFitResult->Prob(); }
