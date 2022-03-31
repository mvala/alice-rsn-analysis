#ifndef AliRsnOutTaskFit_cxx
#define AliRsnOutTaskFit_cxx
#include <TFitResultPtr.h>
#include <AliRsnOutTask.h>
#include <AliRsnOutValue.h>

class AliRsnOutTaskFit : public AliRsnOutTask {

public:
  enum { kVoightPol1 = 0, kVoightPol2, kVoightPol3, kBWPol1, kBWPol2, kBWPol3 };

  AliRsnOutTaskFit(const char *name = "fit", const char *title = "");
  virtual ~AliRsnOutTaskFit();

  static Double_t Pol1(double *m, double *par);
  static Double_t Pol2(double *m, double *par);
  static Double_t Pol3(double *m, double *par);
  static Double_t BWPol1(double *m, double *par);
  static Double_t BWPol2(double *m, double *par);
  static Double_t BWPol3(double *m, double *par);
  static Double_t VoigtPol1(double *m, double *par);
  static Double_t VoigtPol2(double *m, double *par);
  static Double_t VoigtPol3(double *m, double *par);

  virtual void Exec(Option_t *option);
  void Fit(Int_t fitId, Double_t fitMin, Double_t fitMax);
  void SetFit(AliRsnOutValue *fit);
  AliRsnOutValue *GetFit() const { return fInput; }

  void GetYieldBinCounting(Double_t &val, Double_t &err);
  void GetYieldFitFunction(Double_t &val, Double_t &err);
  Double_t GetChi2();
  Double_t GetNdf();
  Double_t GetReducedChi2();
  Double_t GetProb();

  void SetMass(Double_t m) { fMass = m; }
  void SetWidth(Double_t w) { fWidth = w; }
  void SetSigma(Double_t s) { fSigma = s; }

  Double_t GetMass() const { return fMass; }
  Double_t GetWidth() const { return fWidth; }
  Double_t GetSigma() const { return fSigma; }

  void SetProbTest(Double_t min, Double_t max);

private:
  AliRsnOutValue *fInput;
  Double_t fFitProbTestMin;
  Double_t fFitProbTestMax;
  TFitResultPtr fFitResult;
  Int_t fNUmberOfFits;
  Double_t fIntegralEps;

  Double_t fMass;
  Double_t fWidth;
  Double_t fSigma;

  TH1 *fResult;    //!
  TH1 *fResultPar; //!

  ClassDef(AliRsnOutTaskFit, 1)
};

#endif
