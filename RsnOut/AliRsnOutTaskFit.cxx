#include <TROOT.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TF1.h>
#include <TMath.h>
#include <TStyle.h>
#include <TFitResult.h>

#include <Riostream.h>
#include <TBufferJSON.h>

#include <AliRsnOutValue.h>

#include "AliRsnOutTaskFit.h"

ClassImp(AliRsnOutTaskFit)

AliRsnOutTaskFit::AliRsnOutTaskFit(const char *name, const char *title) : AliRsnOutTask(name,title),fInput(0) {

}

AliRsnOutTaskFit::~AliRsnOutTaskFit() {
}

Double_t AliRsnOutTaskFit::VoigtPol1(double *m, double *par)
{
  Double_t val = 0.0;
  double x     = m[0];
  val = TMath::Voigt(x - par[1], par[3], par[2]);
  return par[0]*val + par[4] + x*par[5];
}

Double_t AliRsnOutTaskFit::VoigtPol2(double *m, double *par)
{
  Double_t val = 0.0;
  double x     = m[0];
  val = TMath::Voigt(x - par[1], par[3], par[2]);
  return par[0]*val + par[4] + x*par[5] + x*x*par[6];
}

Double_t AliRsnOutTaskFit::VoigtPol3(double *m, double *par)
{
  Double_t val = 0.0;
  double x     = m[0];
  val = TMath::Voigt(x - par[1], par[3], par[2]);
  return par[0]*val + par[4] + x*par[5] + x*x*par[6] + x*x*x*par[7];
}

void AliRsnOutTaskFit::Exec(Option_t* /*option*/) {

	if (!fInput) return;

	fName = TString::Format("fit_%d[%.3f,%.3f]",fInput->GetId(),fInput->GetMin(),fInput->GetMax());
	fTitle = TString::Format("Fit %d[%.3f,%.3f]",fInput->GetId(),fInput->GetMin(),fInput->GetMax());

	Fit(fInput->GetId(),fInput->GetMin(),fInput->GetMax());
}

void AliRsnOutTaskFit::Fit(Int_t fitId, Double_t fitMin, Double_t fitMax) {

	if (!fParent->GetOutput()) return;

	TH1 *hSig = (TH1*)fParent->GetOutput()->FindObject("hSignal")->Clone();

	if (!hSig) return;

	  const Double_t phi_mass  = 1.019445;
	  const Double_t phi_width = 0.00426;
	  const Double_t phi_sigma = 0.001;

	TF1 *func = 0;
	if (fitId == 0)
		func = new TF1("func1", AliRsnOutTaskFit::VoigtPol1, fitMin, fitMax, 6);
	if (fitId == 1)
		func = new TF1("func2", AliRsnOutTaskFit::VoigtPol2, fitMin, fitMax, 7);
	if (fitId == 2)
		func = new TF1("func3", AliRsnOutTaskFit::VoigtPol3, fitMin, fitMax, 8);

	if (!func)
		return;

	func->SetNpx(200);

//	  // find init parameters
//	  // first fit exclude intervals for polynomials
//	  TF1 *pol1r = new TF1("pol1r", rpol1, func1->GetXmin(), func1->GetXmax(), 2);
//	  TF1 *pol2r = new TF1("pol2r", rpol2, func2->GetXmin(), func2->GetXmax(), 3);
//	  TF1 *pol3r = new TF1("pol3r", rpol3, func3->GetXmin(), func3->GetXmax(), 4);
//	  fstart = 0.99;
//	  fstop  = histo->GetXaxis()->GetXmax();
//	  //  fstart = phi_mass -  7*phi_width;
//	  //  fstop  = phi_mass + 10*phi_width;
//	  //  fstart = fmin;
//	  //  fstop  = fmax;
//	  histo->Fit(pol1r, "QN FC", "", fstart, fstop);
//	  histo->Fit(pol2r, "QN FC", "", fstart, fstop);
//	  histo->Fit(pol3r, "QN FC", "", fstart, fstop);
//	  TF1::RejectPoint(kFALSE);


	  Double_t p0p = hSig->Integral(hSig->FindBin(fitMin), hSig->FindBin(fitMax))*hSig->GetBinWidth(hSig->FindBin(fitMin));
	  func->SetParameters(p0p, phi_mass, phi_width, phi_sigma, 0.0,0.0,0.0,0.0);
//	  func2->SetParameters(p0p, phi_mass, phi_width, phi_sigma, pol2r->GetParameter(0), pol2r->GetParameter(1),
//	                       pol2r->GetParameter(2));
//	  func3->SetParameters(p0p, phi_mass, phi_width, phi_sigma, pol3r->GetParameter(0), pol3r->GetParameter(1),
//	                       pol3r->GetParameter(2), pol3r->GetParameter(3));

	gROOT->SetBatch();
//	TCanvas *c = new TCanvas("canvas",fTitle.Data());
//	gROOT->GetListOfCanvases()->Remove(c);



//	hSig->Set
	func->SetParNames ("yield","mass","width","sigma","p0","p1","p2","p3");
	func->FixParameter(3,phi_sigma);

	TFitResultPtr r;
//	Double_t chi2Prev = 1e6;
//	Double_t chi2Curr = 0;
//	Double_t chi2Delta = 1e-2;
	do {
		r = hSig->Fit(func,"QN MFC S", "", fitMin, fitMax);
		r = hSig->Fit(func,"QN MFC S", "", fitMin, fitMax);
		r = hSig->Fit(func,"QN MFC S", "", fitMin, fitMax);
		r = hSig->Fit(func,"Q MF S", "", fitMin, fitMax);
		Printf("Status = %d",(Int_t)r);
	} while (((Int_t)r)<0);
	r->Print();
//	TFitResult *fitResult = fitResultPtr.Get();
//	fitResult->Print();
//	hSig->Fit(func1,"QN FC", "", fitMin, fitMax);
//	hSig->Fit(func1,"", "", fitMin, fitMax);
//	hSig->Draw();

	if ((r->Prob()<1e-4)||(r->Prob()>0.5+0.1)) {
		SafeDelete(hSig);
	}

	if (hSig) {
		fOutput->Add(hSig);
	}
	gROOT->SetBatch(kFALSE);

}
