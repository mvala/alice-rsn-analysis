#ifndef AliRsnOutTaskInput_cxx
#define AliRsnOutTaskInput_cxx
#include <AliRsnOutTask.h>
#include <TEfficiency.h>

class THnSparse;
class TH1;
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
  Long64_t GetNEvents(Int_t binMin = -1, Int_t binMax = -1) const;
  Bool_t IsEfficiencyOnly() const { return fEffOnly; }

  TGraphAsymmErrors *GetMCEfficiency() const { return fMCEff; }

  virtual void Clear(Option_t *opt = "");

  void CalculateEfficiency();

private:
  TString fFileName;
  TString fListName;
  TString fSigBgName;
  TString fBgName;
  TString fMCRecName;
  TString fMCGenName;
  TGraphAsymmErrors *fMCEff;
  TH1 *fEventStat;
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
