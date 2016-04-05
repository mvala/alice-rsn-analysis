#ifndef AliRsnOutTaskRooFit_cxx
#define AliRsnOutTaskRooFit_cxx

#include <AliRsnOutTask.h>

class AliRsnOutTaskRooFit : public AliRsnOutTask {

public:
	enum {
		kVoightPol1 = 0, kVoightPol2, kVoightPol3, kVoightCheb1, kVoightCheb2
	};

  AliRsnOutTaskRooFit(const char *name="fit", const char *title="");
  virtual ~AliRsnOutTaskRooFit();

  virtual void Exec(Option_t *option);
  void Fit(Int_t fitId, Double_t fitMin, Double_t fitMax);
  void SetFit(AliRsnOutValue *fit) { fInput = fit;}
  AliRsnOutValue *GetFit() const { return fInput; }

private:

  AliRsnOutValue *fInput;
  TList *fFitResults;

  ClassDef(AliRsnOutTaskRooFit, 1)

};

#endif

