#include <TH1.h>
#include <AliRsnOutValue.h>
#include "AliRsnOutTaskNorm.h"

ClassImp(AliRsnOutTaskNorm)

AliRsnOutTaskNorm::AliRsnOutTaskNorm(const char *name, const char *title) : AliRsnOutTask(name,title), fRanges(0) {
}

AliRsnOutTaskNorm::~AliRsnOutTaskNorm() {
}

void AliRsnOutTaskNorm::Exec(Option_t* /*option*/) {

	TH1::AddDirectory(kFALSE);

	Printf("%s",GetName());

	TH1 *hSigBgNorm = (TH1*) fParent->GetOutput()->FindObject("hSignalBg")->Clone();
	hSigBgNorm->SetName("hSignalBg");
	TH1 *hBgNorm = (TH1*) fParent->GetOutput()->FindObject("hBg")->Clone();
	hBgNorm->SetName("hBgNorm");

	Double_t scale = 0.0;

	TIter next(fRanges);
	AliRsnOutValue *v;
	while ((v = (AliRsnOutValue *)next())) {
		Double_t s;
		s = hSigBgNorm->Integral(hSigBgNorm->FindBin((Int_t)v->GetMin()), hSigBgNorm->FindBin((Int_t)v->GetMax()));
		s /= hBgNorm->Integral(hBgNorm->FindBin((Int_t)v->GetMin()), hBgNorm->FindBin((Int_t)v->GetMax()));

		Printf("s=%f",s);
		scale += s;
	}

	scale /= fRanges->GetEntries();
	Printf("scale=%f",scale);
	Double_t correction = 0.9;
	hBgNorm->Scale(scale * correction);


	TH1 *hSig = (TH1*) hSigBgNorm->Clone();
	hSig->SetName("hSignal");
	hSig->Add(hBgNorm, -1);

	fOutput->Add(hSigBgNorm);
	fOutput->Add(hBgNorm);
	fOutput->Add(hSig);
	fOutput->Print();

	Printf("%p",fOutput);
}

void AliRsnOutTaskNorm::AddRange(AliRsnOutValue* range) {
	if (!range) return;
	if (!fRanges) fRanges = new TList();
	fRanges->Add(range);
}
