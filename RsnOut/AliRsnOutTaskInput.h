#ifndef AliRsnOutTaskInput_cxx
#define AliRsnOutTaskInput_cxx

#include <AliRsnOutTask.h>

class THnSparse;
class TH1;
class AliRsnOutTaskInput : public AliRsnOutTask {

public:

  AliRsnOutTaskInput(const char *name="input", const char *title="");
  virtual ~AliRsnOutTaskInput();

  virtual void Exec(Option_t *option);

  void SetFileName(TString filename) { fFileName = filename; fTitle = filename; }
  void SetListName(TString listname) { fListName = listname; }
  void SetSigBgName(TString sigbgname) { fSigBgName = sigbgname; }
  void SetBgName(TString bgname) { fBgName = bgname; }

  THnSparse *GetSigBg() const { return fSigBg; }
  THnSparse *GetBg() const { return fBg; }
private:

  TString fFileName;
  TString fListName;
  TString fSigBgName;
  TString fBgName;

  THnSparse *fSigBg;//!
  THnSparse *fBg;//!

  ClassDef(AliRsnOutTaskInput, 1)

};

#endif

