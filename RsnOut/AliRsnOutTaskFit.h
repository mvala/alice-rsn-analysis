#ifndef AliRsnOutTaskFit_cxx
#define AliRsnOutTaskFit_cxx

#include <AliRsnOutTask.h>

class AliRsnOutTaskFit : public AliRsnOutTask {

public:
	enum {
		kVoightPol1 = 0, kVoightPol2, kVoightPol3, kVoightCheb1, kVoightCheb2
	};

  AliRsnOutTaskFit(const char *name="fit", const char *title="");
  virtual ~AliRsnOutTaskFit();

  static Double_t VoigtPol1(double *m, double *par);
  static Double_t VoigtPol2(double *m, double *par);
  static Double_t VoigtPol3(double *m, double *par);


  virtual void Exec(Option_t *option);
  void Fit(Int_t fitId, Double_t fitMin, Double_t fitMax);
  void SetFit(AliRsnOutValue *fit) { fInput = fit;}
  AliRsnOutValue *GetFit() const { return fInput; }

private:

  AliRsnOutValue *fInput;

  ClassDef(AliRsnOutTaskFit, 1)

};

#endif

