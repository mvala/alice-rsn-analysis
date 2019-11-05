#ifndef AliRsnOutTaskFit_cxx
#define AliRsnOutTaskFit_cxx
#include <TFitResultPtr.h>

#include <AliRsnOutTask.h>

class AliRsnOutTaskFit : public AliRsnOutTask {

public:
  enum {
    kVoightPol1 = 0,
    kVoightPol2,
    kVoightPol3,
    kVoightCheb1,
    kVoightCheb2
  };

  AliRsnOutTaskFit(const char *name = "fit", const char *title = "");
  virtual ~AliRsnOutTaskFit();

  static Double_t Pol1(double *m, double *par);
  static Double_t Pol2(double *m, double *par);
  static Double_t Pol3(double *m, double *par);
  static Double_t BWPol1(double *x, double *par);
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

  void SetProbTest(Double_t min, Double_t max);

private:
  AliRsnOutValue *fInput;
  Double_t fFitProbTestMin;
  Double_t fFitProbTestMax;
  TFitResultPtr fFitResult;
  Int_t fNUmberOfFits;
  Double_t fIntegralEps;

  TH1 *fResult;    //!
  TH1 *fResultPar; //!

  ClassDef(AliRsnOutTaskFit, 1)
};

#endif
