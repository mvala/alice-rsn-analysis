#ifndef AliRsnOutTaskInput_cxx
#define AliRsnOutTaskInput_cxx
#include <TEfficiency.h>
#include <AliRsnOutTask.h>

class THnSparse;
class TH1;
class TFile;
class TList;
class AliRsnOutTaskInput : public AliRsnOutTask {

public:

  AliRsnOutTaskInput(const char *name="input", const char *title="");
  virtual ~AliRsnOutTaskInput();

  virtual void Exec(Option_t *option);
  virtual void ExecPost(Option_t *option);

  void SetFileName(TString filename) { fFileName = filename; fTitle = filename; }
  void SetListName(TString listname) { fListName = listname; }
  void SetSigBgName(TString sigbgname) { fSigBgName = sigbgname; }
  void SetBgName(TString bgname) { fBgName = bgname; }
  void SetMCRecName(TString mcrecname) { fMCRecName = mcrecname; }
  void SetMCGenName(TString mcgenname) { fMCGenName = mcgenname; }
  void SetEfficiencyOnly(Bool_t onlyEff) { fIsEffOnly = onlyEff;}

  Bool_t IsEfficiencyOnly() const { return fIsEffOnly; }
  THnSparse *GetSigBg() const { return fSigBg; }
  THnSparse *GetBg() const { return fBg; }
  THnSparse *GetMCRec() const { return fMCRec; }
  THnSparse *GetMCGen() const { return fMCGen; }

  TGraphAsymmErrors *GetMCEfficiency() const { return fMCEff; }
  TList *GetListOfMC() const { return fListOfMC; }
  void AddMC(TObject*mc);

  virtual void 	Clear (Option_t *opt="");

  void GetResults();

private:

  TString fFileName;
  TString fListName;
  TString fSigBgName;
  TString fBgName;
  TString fMCRecName;
  TString fMCGenName;
  Bool_t  fIsEffOnly;
  TGraphAsymmErrors *fMCEff;
  Long64_t fNEvents;

  TList *fListOfMC;

  TFile *fFile;//!
  TList *fList;//!
  THnSparse *fSigBg;//!
  THnSparse *fBg;//!
  THnSparse *fMCRec;//!
  THnSparse *fMCGen;//!

  ClassDef(AliRsnOutTaskInput, 1)

};

#endif

