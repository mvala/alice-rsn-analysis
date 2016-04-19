#include <TH1.h>
#include <TGraphAsymmErrors.h>
#include <TFolder.h>
#include <THnSparse.h>

#include "AliRsnOutTaskBin.h"
#include "AliRsnOutTaskNorm.h"
#include "AliRsnOutTaskFit.h"
#include "AliRsnOutTaskInput.h"

#include "AliRsnOutTaskResult.h"

ClassImp(AliRsnOutTaskResult)

AliRsnOutTaskResult::AliRsnOutTaskResult(const char *name, const char *title) : AliRsnOutTask(name,title), fData(0), fMC(0) {
}

AliRsnOutTaskResult::~AliRsnOutTaskResult() {
}

void AliRsnOutTaskResult::Exec(Option_t* /*option*/) {

	AliRsnOutTaskInput* tInputData = dynamic_cast<AliRsnOutTaskInput *>(fData);
	if (!tInputData) return;

	Printf("%s",GetName());

	if (!fOutput) {
		fOutput = new TList();
		fOutput->SetOwner();
	}

	Long64_t nEvents = tInputData->GetNEvents();

	const Int_t nAxis = 4;
	Int_t bins[nAxis];
	Double_t mins[nAxis];
	Double_t maxs[nAxis];

	Int_t nResultsBins = 10;

	AliRsnOutTaskBin *tBin;
	AliRsnOutTaskNorm *tNorm;
	AliRsnOutTaskFit *tFit;
	AliRsnOutValue *v;

	bins[0] = tInputData->GetListOfTasks()->GetEntries();
	mins[0] = 0;
	maxs[0] = tInputData->GetListOfTasks()->GetEntries();
	tBin = (AliRsnOutTaskBin *) tInputData->GetListOfTasks()->At(0);
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

	THnSparseD *s = new THnSparseD("sparse","Results Sparse",nAxis,bins,mins,maxs);
	const Int_t nVariableBins = tInputData->GetListOfTasks()->GetEntries();
	Double_t varBins[nVariableBins+1];

	Int_t iBin;
	for (iBin = 0; iBin < tInputData->GetListOfTasks()->GetEntries(); ++iBin) {
		tBin = (AliRsnOutTaskBin *) tInputData->GetListOfTasks()->At(iBin);
		v = (AliRsnOutValue*)tBin->GetValue();
		varBins[iBin] = v->GetMin();
		if (iBin == tInputData->GetListOfTasks()->GetEntries()-1)
			varBins[iBin+1] = v->GetMax();

	}
	s->GetAxis(0)->Set(nVariableBins, varBins);
	s->GetAxis(0)->SetName("bin");
	s->GetAxis(1)->SetName("norm");
	s->GetAxis(2)->SetName("fit");
	s->GetAxis(3)->SetName("value");

	fOutput->Add(s);

	AliRsnOutTaskInput *tInputMC = 0;
	TGraphAsymmErrors *mcEff = 0;
	if (fMC) {
		tInputMC = dynamic_cast<AliRsnOutTaskInput *>(fMC);
		if (tInputMC) mcEff = tInputMC->GetMCEfficiency();
	}

	Int_t iSparseBin[nAxis];
	Double_t x,y,ey;
	for (Int_t iBin = 0; iBin < tInputData->GetListOfTasks()->GetEntries(); ++iBin) {
		tBin = (AliRsnOutTaskBin *) tInputData->GetListOfTasks()->At(iBin);
		v = (AliRsnOutValue*)tBin->GetValue();
		x = (v->GetMax()+v->GetMin())/2;
		iSparseBin[0] = s->GetAxis(0)->FindBin(x);
		Double_t valMCEff,errMCEff;
		if (mcEff) {
			Double_t xx;
			mcEff->GetPoint(iBin,xx,valMCEff);
			errMCEff = mcEff->GetErrorY(iBin);
		}
		for (Int_t iNorm = 0; iNorm < tBin->GetListOfTasks()->GetEntries(); ++iNorm) {
			tNorm = (AliRsnOutTaskNorm *) tBin->GetListOfTasks()->At(iNorm);
			iSparseBin[1] = iNorm+1;
			for (Int_t iFit = 0; iFit < tNorm->GetListOfTasks()->GetEntries(); ++iFit) {
				tFit = (AliRsnOutTaskFit *) tNorm->GetListOfTasks()->At(iFit);
				iSparseBin[2] = iFit+1;

				if (mcEff) {
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
				if (mcEff)  {
					// Corrected spectra Bin Counting
					iSparseBin[3] = 4;
					s->SetBinContent(iSparseBin,y/valMCEff/nEvents);
					s->SetBinError(iSparseBin,y/valMCEff*TMath::Sqrt(TMath::Power(ey/y,2)+TMath::Power(errMCEff/valMCEff,2))/nEvents);
				}

				// Raw Fit Function
				iSparseBin[3] = 2;
				tFit->GetYieldFitFunction(y,ey);
				s->SetBinContent(iSparseBin,y);
				s->SetBinError(iSparseBin,ey);
				if (mcEff)  {
					// Corrected spectra Fit Function
					iSparseBin[3] = 5;
					s->SetBinContent(iSparseBin,y/valMCEff/nEvents);
					s->SetBinError(iSparseBin,y/valMCEff*TMath::Sqrt(TMath::Power(ey/y,2)+TMath::Power(errMCEff/valMCEff,2))/nEvents);
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

	TFolder *dRoot = new TFolder("hist","Final Results");
	fOutput->Add(dRoot);
	TFolder *dNorm, *dFit;
	TH1D *h;
	tBin = (AliRsnOutTaskBin *) tInputData->GetListOfTasks()->At(0);
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
			s->GetAxis(3)->SetRange(1,1);h = s->Projection(0);h->SetName("hRawBC");h->GetYaxis()->SetRangeUser(0,h->GetMaximum()*1.1);dFit->Add(h);
			s->GetAxis(3)->SetRange(2,2);h = s->Projection(0);h->SetName("hRawFF");h->GetYaxis()->SetRangeUser(0,h->GetMaximum()*1.1);dFit->Add(h);
			if (mcEff) {
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

void AliRsnOutTaskResult::SetData(AliRsnOutTask* data) {
	if (!data) return;
	fData = data;
	fName = TString::Format("%s_%s",fData?fData->GetName():"no_data",fMC?fMC->GetName():"no_eff");
	fTitle = TString::Format("%s (%s)",fData?fData->GetTitle():"no_data",fMC?fMC->GetTitle():"No Eff");
}

void AliRsnOutTaskResult::SetMC(AliRsnOutTask* mc) {
	if (!mc) return;
	fMC = mc;
	fName = TString::Format("%s_%s",fData?fData->GetName():"no_data",fMC?fMC->GetName():"no_eff");
	fTitle = TString::Format("%s (%s)",fData?fData->GetTitle():"no_data",fMC?fMC->GetTitle():"No Eff");
}
