#ifndef AliRsnOutTaskInput_cxx
#define AliRsnOutTaskInput_cxx

#include <AliRsnOutTask.h>
#include <TEfficiency.h>

class THnSparse;
class TH1F;
class TFile;
class TList;
class AliRsnOutTaskInput : public AliRsnOutTask {

public:
  AliRsnOutTaskInput(const char *name = "input", const char *title = "");
  virtual ~AliRsnOutTaskInput();

  virtual void Exec(Option_t *option);
  virtual void ExecPost(Option_t *option);
  virtual void UpdateTask();
  void SetFileName(TString filename) {
    fFileName = filename;
    fTitle = filename;
  }
  void SetListName(TString listname) { fListName = listname; }
  void SetSigBgName(TString sigbgname) { fSigBgName = sigbgname; }
  void SetBgName(TString bgname) { fBgName = bgname; }
  void SetMCRecName(TString mcrecname) { fMCRecName = mcrecname; }
  void SetMCGenName(TString mcgenname) { fMCGenName = mcgenname; }
  void SetEfficiencyOnly(Bool_t effOnly = kTRUE) { fEffOnly = effOnly; }

  THnSparse *GetSigBg() const { return fSigBg; }
  THnSparse *GetBg() const { return fBg; }
  THnSparse *GetMCRec() const { return fMCRec; }
  THnSparse *GetMCGen() const { return fMCGen; }
  Double_t GetNEvents(Double_t min = 1, Double_t max = 0) const;
  Bool_t IsEfficiencyOnly() const { return fEffOnly; }

  virtual void Clear(Option_t *opt = "");

private:
  TString fFileName;
  TString fListName;
  TString fSigBgName;
  TString fBgName;
  TString fMCRecName;
  TString fMCGenName;
  TH1F *fEventStat;
  Bool_t fEffOnly;

  TFile *fFile;      //!
  TList *fList;      //!
  THnSparse *fSigBg; //!
  THnSparse *fBg;    //!
  THnSparse *fMCRec; //!
  THnSparse *fMCGen; //!

  ClassDef(AliRsnOutTaskInput, 1)
};

#endif
