#ifndef AliRsnOutTaskResult_cxx
#define AliRsnOutTaskResult_cxx

#include <AliRsnOutTask.h>

class THnSparse;
class TFolder;

class AliRsnOutTaskResult : public AliRsnOutTask {

public:
  AliRsnOutTaskResult(const char *name = "results", const char *title = "");
  virtual ~AliRsnOutTaskResult();

  virtual void Exec(Option_t *option);

  void SetData(AliRsnOutTask *data);
  void SetMC(AliRsnOutTask *mc);

  void ProcessBinMgrElement(AliRsnOutTaskBin *bme, AliRsnOutTaskBin *bmeMC = 0);

  THnSparse *CreateSparse(AliRsnOutTaskBin *bme, TFolder *folder);
  void FillSparse(AliRsnOutTask *task, AliRsnOutTask *taskMC, THnSparse *s,
                  Int_t *sparseBin, Int_t index);
  void FolderFromSparse(AliRsnOutTask *task, THnSparse *s, Int_t index,
                        TFolder *folder, Int_t nBins, Int_t projX, Int_t projY);

  void GetBinsFromTask(AliRsnOutTask *t, Double_t *varBins);
  Double_t GetErrorDivide(Double_t val1, Double_t err1, Double_t val2,
                          Double_t err2);

private:
  AliRsnOutTask *fData;
  AliRsnOutTask *fMC;
  ClassDef(AliRsnOutTaskResult, 1)
};

#endif
