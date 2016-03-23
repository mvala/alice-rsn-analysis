#ifndef AliRsnOutTaskFit_cxx
#define AliRsnOutTaskFit_cxx

#include <AliRsnOutTask.h>

class AliRsnOutTaskFit : public AliRsnOutTask {

public:
	enum {
		kVoightPol1 = 0, kVoightPol2 = 1, kVoightCheb1 = 2, kVoightCheb2 = 3
	};

  AliRsnOutTaskFit(const char *name="rsn", const char *title="");
  virtual ~AliRsnOutTaskFit();

  virtual void Exec(Option_t *option);

  void Fit(Int_t id,Int_t fitId, Double_t fitMin, Double_t fitMax);

  void AddFit(AliRsnOutValue *fit);
  TList *GetFit() const { return fFits; }

private:

  TList *fFits;
  TList *fFitResults;

  ClassDef(AliRsnOutTaskFit, 1)

};

#endif

