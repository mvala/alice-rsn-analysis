#include <THnSparse.h>
#include <TAxis.h>
#include <TH1.h>

#include "AliRsnOutTaskInput.h"
#include "AliRsnOutTaskBin.h"

ClassImp(AliRsnOutTaskBin)

AliRsnOutTaskBin::AliRsnOutTaskBin(const char *name, const char *title) : AliRsnOutTask(name,title), fValue(), fCuts(0) {
}

AliRsnOutTaskBin::~AliRsnOutTaskBin() {
}

void AliRsnOutTaskBin::Exec(Option_t* /*option*/) {

	AliRsnOutTaskInput *input = dynamic_cast<AliRsnOutTaskInput*> (GetPartent());
	if (!input) return;

	THnSparse *sigBg = input->GetSigBg();
	THnSparse *bg = input->GetBg();

	ApplyCuts(sigBg,bg);

	TH1 *hSigBg = sigBg->Projection(fValue.GetId());
	hSigBg->SetName("hSignalBg");
	fOutput->Add(hSigBg);

	TH1 *hBg = bg->Projection(fValue.GetId());
	hBg->SetName("hBg");
	fOutput->Add(hBg);

	fOutput->Print();

}

void AliRsnOutTaskBin::AddCut(AliRsnOutValue* cut) {
	if (!cut) return;
	if (!fCuts) fCuts = new TList();
	fCuts->Add(cut);
}

void AliRsnOutTaskBin::ApplyCuts(THnSparse *sigBg,THnSparse *bg) {

	// TODO reset all cuts with Range(0,0)

	TIter next(fCuts);
	AliRsnOutValue *v;
	fName = "";
	Double_t minVal,maxVal;
	while ((v = (AliRsnOutValue *)next())) {
		sigBg->GetAxis(v->GetId())->SetRange((Int_t)v->GetMin(), (Int_t)v->GetMax());
		bg->GetAxis(v->GetId())->SetRange((Int_t)v->GetMin(), (Int_t)v->GetMax());

		minVal = sigBg->GetAxis(v->GetId())->GetBinLowEdge((Int_t)v->GetMin());
		maxVal = sigBg->GetAxis(v->GetId())->GetBinUpEdge((Int_t)v->GetMax());
		fName += TString::Format("%s[%.2f,%.2f]", sigBg->GetAxis(v->GetId())->GetName(), minVal, maxVal);
	}
}
