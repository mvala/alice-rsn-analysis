#include <TFile.h>
#include <TList.h>
#include <TH1.h>
#include <TStyle.h>
#include <TROOT.h>
#include <THnSparse.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TFolder.h>
#include "AliRsnOutTaskBin.h"
#include "AliRsnOutTaskNorm.h"
#include "AliRsnOutTaskFit.h"
#include "AliRsnOutTaskInput.h"

ClassImp(AliRsnOutTaskInput)

AliRsnOutTaskInput::AliRsnOutTaskInput(const char *name, const char *title) :
		AliRsnOutTask(name, title), fFileName(), fListName(), fSigBgName(), fBgName(), fMCRecName(), fMCGenName(), fIsEffOnly(kFALSE), fMCEff(0), fNEvents(0), fListOfMC(0), fFile(
				0), fList(0), fSigBg(0), fBg(0), fMCRec(0), fMCGen(0) {
}

AliRsnOutTaskInput::~AliRsnOutTaskInput() {
	Clear();
}

void AliRsnOutTaskInput::Exec(Option_t* /*option*/) {

	if (!fFile) {
		fFile = TFile::Open(fFileName.Data());
		if (!fFile)
			return;
		fList = (TList *) fFile->Get(fListName.Data());
		if (!fList)
			return;

		TList *extra = (TList*) fFile->Get(TString::Format("%s_extra",fListName.Data()).Data());
		TH1 *hEvents = (TH1*) extra->FindObject("hEventStat");
		fNEvents = hEvents->GetBinContent(4);

		if (!fIsEffOnly) {
			fSigBg = (THnSparse *) fList->FindObject(fSigBgName.Data());
			if (!fSigBg)
				return;
			fBg = (THnSparse *) fList->FindObject(fBgName.Data());
			if (!fBg)
				return;
		}
		fMCRec = (THnSparse *) fList->FindObject(fMCRecName.Data());
		fMCGen = (THnSparse *) fList->FindObject(fMCGenName.Data());

	}
}

void AliRsnOutTaskInput::ExecPost(Option_t* /*option*/) {

	GetResults();

	Clear();
}

void AliRsnOutTaskInput::Clear(Option_t* /*opt*/) {
	if (fList) SafeDelete(fList);
	if (fFile) SafeDelete(fFile);
	fSigBg = 0;
	fBg = 0;
	fMCRec = 0;
	fMCGen = 0;
}

void AliRsnOutTaskInput::GetResults() {

	if (!fOutput) {
		fOutput = new TList();
		fOutput->SetOwner();
	}

	const Int_t nAxis = 4;
	Int_t bins[nAxis];
	Double_t mins[nAxis];
	Double_t maxs[nAxis];

	Int_t nResultsBins = 10;

	AliRsnOutTaskBin *tBin;
	AliRsnOutTaskNorm *tNorm;
	AliRsnOutTaskFit *tFit;
	AliRsnOutValue *v;

	bins[0] = fTasks->GetEntries();
	mins[0] = 0;
	maxs[0] = fTasks->GetEntries();
	tBin = (AliRsnOutTaskBin *) fTasks->At(0);
	bins[1] = tBin->GetListOfTasks()->GetEntries();
	mins[1] = 0;
	maxs[1] = tBin->GetListOfTasks()->GetEntries();
	tNorm = (AliRsnOutTaskNorm *) tBin->GetListOfTasks()->At(0);
	bins[2] = tNorm->GetListOfTasks()->GetEntries();
	mins[2] = 0;
	maxs[2] = tNorm->GetListOfTasks()->GetEntries();
	tFit = (AliRsnOutTaskFit *) tNorm->GetListOfTasks()->At(0);

	bins[3] = nResultsBins;
	mins[3] = 0;
	maxs[3] = nResultsBins;

	THnSparseD *s = new THnSparseD("resultSparse","Results Sparse",nAxis,bins,mins,maxs);
	const Int_t nVariableBins = fTasks->GetEntries();
	Double_t varBins[nVariableBins+1];

	Int_t iBin;
	for (iBin = 0; iBin < fTasks->GetEntries(); ++iBin) {
		tBin = (AliRsnOutTaskBin *) fTasks->At(iBin);
		v = (AliRsnOutValue*)tBin->GetValue();
		varBins[iBin] = v->GetMin();
		if (iBin == fTasks->GetEntries()-1)
			varBins[iBin+1] = v->GetMax();

	}
	s->GetAxis(0)->Set(nVariableBins, varBins);
	s->GetAxis(0)->SetName("bin");
	s->GetAxis(1)->SetName("norm");
	s->GetAxis(2)->SetName("fit");
	s->GetAxis(3)->SetName("value");

//	Printf("nbins=%d min=%f max=%f",s->GetAxis(0)->GetNbins(),s->GetAxis(0)->GetXmin(),s->GetAxis(0)->GetXmax());
//	Printf("nbins=%d min=%f max=%f",s->GetAxis(1)->GetNbins(),s->GetAxis(1)->GetXmin(),s->GetAxis(1)->GetXmax());
//	Printf("nbins=%d min=%f max=%f",s->GetAxis(2)->GetNbins(),s->GetAxis(2)->GetXmin(),s->GetAxis(2)->GetXmax());
//	Printf("nbins=%d min=%f max=%f",s->GetAxis(3)->GetNbins(),s->GetAxis(3)->GetXmin(),s->GetAxis(3)->GetXmax());

	fOutput->Add(s);

	TEfficiency *fMCEfficiency = 0;

	AliRsnOutTaskInput *tInputMC = 0;

	if (fListOfMC) {
		tInputMC = (AliRsnOutTaskInput *)fListOfMC->At(0);
		fMCEff = tInputMC->GetMCEfficiency();
	}

	Int_t iSparseBin[nAxis];
	Double_t x,y,ey;
	for (Int_t iBin = 0; iBin < fTasks->GetEntries(); ++iBin) {
		tBin = (AliRsnOutTaskBin *) fTasks->At(iBin);
		v = (AliRsnOutValue*)tBin->GetValue();
		x = (v->GetMax()+v->GetMin())/2;
		iSparseBin[0] = s->GetAxis(0)->FindBin(x);
		Double_t valMCEff,errMCEff;
		TH1 *mcGen = (TH1*) tBin->GetOutput()->FindObject("hSignalMCGen");
		TH1 *mcRec = (TH1*) tBin->GetOutput()->FindObject("hSignalMCRec");
		if (mcGen&&mcRec) {
			if (!fMCEfficiency) fMCEfficiency = new TEfficiency("eff","MC Efficiency;x;#epsilon",nVariableBins, varBins);
			fMCEfficiency->SetTotalEvents(iBin+1,mcGen->Integral());
			fMCEfficiency->SetPassedEvents(iBin+1,mcRec->Integral());
			valMCEff = fMCEfficiency->GetEfficiency(iSparseBin[0]);
			errMCEff = fMCEfficiency->GetEfficiencyErrorUp(iSparseBin[0]);
		} else if (fMCEff) {
			Double_t xx;
			fMCEff->GetPoint(iBin,xx,valMCEff);
			errMCEff = fMCEff->GetErrorY(iBin);
		}
//		Printf("valMCEff=%f errMCEff=%f",valMCEff,errMCEff);
		for (Int_t iNorm = 0; iNorm < tBin->GetListOfTasks()->GetEntries(); ++iNorm) {
			tNorm = (AliRsnOutTaskNorm *) tBin->GetListOfTasks()->At(iNorm);
			iSparseBin[1] = iNorm+1;
			for (Int_t iFit = 0; iFit < tNorm->GetListOfTasks()->GetEntries(); ++iFit) {
				tFit = (AliRsnOutTaskFit *) tNorm->GetListOfTasks()->At(iFit);
				iSparseBin[2] = iFit+1;

				if (!fIsEffOnly) {
					if ((mcGen&&mcRec)||fMCEff) {
						// MC eff
						iSparseBin[3] = 3;
						s->SetBinContent(iSparseBin,valMCEff);
						s->SetBinError(iSparseBin,errMCEff);
					}
					// Raw Bin Counting
					iSparseBin[3] = 1;
					tFit->GetYieldBinCounting(y,ey);
					s->SetBinContent(iSparseBin,y);
					s->SetBinError(iSparseBin,ey);

					if ((mcGen&&mcRec)||fMCEff)  {
						// Corrected spectra Bin Counting
						iSparseBin[3] = 4;
						s->SetBinContent(iSparseBin,y/valMCEff/fNEvents);
						s->SetBinError(iSparseBin,y/valMCEff*TMath::Sqrt(TMath::Power(ey/y,2)+TMath::Power(errMCEff/valMCEff,2))/fNEvents);
					}
					// Raw Fit Function
					iSparseBin[3] = 2;
					tFit->GetYieldFitFunction(y,ey);
					s->SetBinContent(iSparseBin,y);
					s->SetBinError(iSparseBin,ey);

					if ((mcGen&&mcRec)||fMCEff)  {
						// Corrected spectra Fit Function
						iSparseBin[3] = 5;
						s->SetBinContent(iSparseBin,y/valMCEff/fNEvents);
						s->SetBinError(iSparseBin,y/valMCEff*TMath::Sqrt(TMath::Power(ey/y,2)+TMath::Power(errMCEff/valMCEff,2))/fNEvents);
					}

					// Chi2
					iSparseBin[3] = 6;
					s->SetBinContent(iSparseBin,tFit->GetChi2());
					s->SetBinError(iSparseBin,0);

					// Ndf
					iSparseBin[3] = 7;
					s->SetBinContent(iSparseBin,tFit->GetNdf());
					s->SetBinError(iSparseBin,0);

					// Reduced Chi2 = Chi2/Ndf
					iSparseBin[3] = 8;
					s->SetBinContent(iSparseBin,tFit->GetReducedChi2());
					s->SetBinError(iSparseBin,0);

					// Prob
					iSparseBin[3] = 9;
					s->SetBinContent(iSparseBin,tFit->GetProb());
					s->SetBinError(iSparseBin,0);
				}
			}
		}
	}

	if (fMCEfficiency) {
		fMCEff = fMCEfficiency->CreateGraph();
		fOutput->Add(fMCEff);
	}

	TFolder *dRoot = new TFolder("results","Results");
	fOutput->Add(dRoot);
	TFolder *dNorm, *dFit;
	TH1D *h;
	tBin = (AliRsnOutTaskBin *) fTasks->At(0);
	if (!tBin) return;
	for (Int_t iNorm = 0; iNorm < tBin->GetListOfTasks()->GetEntries(); ++iNorm) {
		tNorm = (AliRsnOutTaskNorm *) tBin->GetListOfTasks()->At(iNorm);
		if (!tNorm) continue;
		dNorm = dRoot->AddFolder(tNorm->GetName(),tNorm->GetTitle());
		s->GetAxis(1)->SetRange(iNorm+1,iNorm+1);
		for (Int_t iFit = 0; iFit < tNorm->GetListOfTasks()->GetEntries(); ++iFit) {
			tFit = (AliRsnOutTaskFit *) tNorm->GetListOfTasks()->At(iFit);
			if (!tFit) continue;
			dFit = dNorm->AddFolder(tFit->GetName(),tFit->GetTitle());
			s->GetAxis(2)->SetRange(iFit+1,iFit+1);
			if (!fIsEffOnly) {
				s->GetAxis(3)->SetRange(1,1);h = s->Projection(0);h->SetName("hRawBC");h->GetYaxis()->SetRangeUser(0,h->GetMaximum()*1.1);dFit->Add(h);
				s->GetAxis(3)->SetRange(2,2);h = s->Projection(0);h->SetName("hRawFF");h->GetYaxis()->SetRangeUser(0,h->GetMaximum()*1.1);dFit->Add(h);
			}
			if (!fIsEffOnly) {
				if (fMCEff) {
					s->GetAxis(3)->SetRange(3,3);h = s->Projection(0);h->SetName("hMCEff");h->GetYaxis()->SetRangeUser(0,h->GetMaximum()*1.1);dFit->Add(h);
					s->GetAxis(3)->SetRange(4,4);h = s->Projection(0);h->SetName("hCorrBC");h->GetYaxis()->SetRangeUser(0,h->GetMaximum()*1.1);dFit->Add(h);
					s->GetAxis(3)->SetRange(5,5);h = s->Projection(0);h->SetName("hCorrFF");h->GetYaxis()->SetRangeUser(0,h->GetMaximum()*1.1);dFit->Add(h);
				}
				s->GetAxis(3)->SetRange(6,6);h = s->Projection(0);h->SetName("hChi2");h->GetYaxis()->SetRangeUser(0,h->GetMaximum()*1.1);dFit->Add(h);
				s->GetAxis(3)->SetRange(7,7);h = s->Projection(0);h->SetName("hNdf");h->GetYaxis()->SetRangeUser(0,h->GetMaximum()*1.1);dFit->Add(h);
				s->GetAxis(3)->SetRange(8,8);h = s->Projection(0);h->SetName("hReducedChi2");h->GetYaxis()->SetRangeUser(0,h->GetMaximum()*1.1);dFit->Add(h);
				s->GetAxis(3)->SetRange(9,9);h = s->Projection(0);h->SetName("hProb");h->GetYaxis()->SetRangeUser(0,h->GetMaximum()*1.1);dFit->Add(h);
			}
		}
	}
}

void AliRsnOutTaskInput::AddMC(TObject* mc) {
	if (!mc) return;
	if (!fListOfMC) fListOfMC = new TList();
	fListOfMC->Add(mc);
}
