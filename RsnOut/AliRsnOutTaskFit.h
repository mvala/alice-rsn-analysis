#ifndef AliRsnOutTaskFit_cxx
#define AliRsnOutTaskFit_cxx

#include <AliRsnOutTask.h>

class AliRsnOutTaskFit : public AliRsnOutTask {

public:
	enum {
		kVoightPol1 = 0, kVoightPol2 = 1, kVoightCheb1 = 2, kVoightCheb2 = 3
	};

  AliRsnOutTaskFit(const char *name="fit", const char *title="");
  virtual ~AliRsnOutTaskFit();

  virtual void Exec(Option_t *option);
  void Fit(Int_t fitId, Double_t fitMin, Double_t fitMax);
  void SetFit(AliRsnOutValue *fit) { fInput = fit;}
  AliRsnOutValue *GetFit() const { return fInput; }

private:

  AliRsnOutValue *fInput;
  TList *fFitResults;

  ClassDef(AliRsnOutTaskFit, 1)

};

#endif

