#include <RooAddPdf.h>
#include <RooBreitWigner.h>
#include <RooChebychev.h>
#include <RooDataHist.h>
#include <RooFFTConvPdf.h>
#include <RooFitResult.h>
#include <RooGaussian.h>
#include <RooNumConvPdf.h>
#include <RooPlot.h>
#include <RooPolynomial.h>
#include <RooRealVar.h>
#include <RooVoigtian.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TH1.h>
#include <TMath.h>
#include <TROOT.h>
#include <TStyle.h>

#include <Riostream.h>
#include <TBufferJSON.h>

#include <AliRsnOutValue.h>

#include "AliRsnOutTaskRooFit.h"

using namespace RooFit;

ClassImp(AliRsnOutTaskRooFit);

AliRsnOutTaskRooFit::AliRsnOutTaskRooFit(const char *name, const char *title)
    : AliRsnOutTask(name, title), fInput(0), fFitResults(0) {
  RooMsgService::instance().getStream(0).active = kFALSE;
  RooMsgService::instance().getStream(1).active = kFALSE;
}

AliRsnOutTaskRooFit::~AliRsnOutTaskRooFit() {}

void AliRsnOutTaskRooFit::Exec(Option_t * /*option*/) {

  if (!fInput)
    return;

  fName = TString::Format("fit_%d[%.3f,%.3f]", fInput->GetId(),
                          fInput->GetMin(), fInput->GetMax());
  fTitle = TString::Format("Fit %d[%.3f,%.3f]", fInput->GetId(),
                           fInput->GetMin(), fInput->GetMax());

  Fit(fInput->GetId(), fInput->GetMin(), fInput->GetMax());
}

void AliRsnOutTaskRooFit::Fit(Int_t fitId, Double_t fitMin, Double_t fitMax) {

  if (!fParent->GetOutput())
    return;

  TH1 *hSig = (TH1 *)fParent->GetOutput()->FindObject("hSignal")->Clone();

  if (!hSig)
    return;

  if (fitId < 0)
    return;

  Double_t min = hSig->GetXaxis()->GetXmin();
  Double_t max = hSig->GetXaxis()->GetXmax();
  min = 0.99;
  //	max = 1.05;
  RooRealVar x("x", "x", min, max);
  RooDataHist data("data", "dataset with x", x, hSig);

  //	if (kVoightPol1)
  RooRealVar meanV("mean", "mean Voigtian", 1.019, 1.000, 1.050);
  RooRealVar widthV("width", "width Voigtian", 0.0045, 0.003, 0.006);
  RooRealVar sigmaV("sigma", "sigma Voigtian", 0.001 /*, 0.000,0.002*/);
  RooVoigtian sig("voigtian", "Voigtian", x, meanV, widthV, sigmaV);

  RooRealVar c0("c0", "coefficient #0", 1.0, -10., 10.);
  RooRealVar c1("c1", "coefficient #1", 0.1, -10., 10.);
  RooRealVar c2("c2", "coefficient #2", -0.1, -10., 10.);
  RooPolynomial bkg("pol", "background p.d.f.", x, RooArgList(c0, c1));
  //	RooPolynomial bkg("pol", "background p.d.f.", x, RooArgList(c0, c1,
  // c2));
  //	RooChebychev bkg("pol","background p.d.f.",x,RooArgList(c0,c1)) ;
  //	RooChebychev bkg("pol","background p.d.f.",x,RooArgList(c0,c1,c2)) ;

  RooRealVar fsig("fsig", "signal fraction", 0.5, 0., 1.);
  RooAddPdf model("model", "model", RooArgList(sig, bkg), fsig);

  // --- Construct signal+background PDF ---
  //	RooRealVar nsig("nsig", "#signal events", 200, 0., 1e8);
  //	RooRealVar nbkg("nbkg", "#background events", 800, 0., 1e8);
  //	RooAddPdf model("model", "v+p", RooArgList(sig, bkg),
  //			RooArgList(nsig, nbkg));

  Double_t minFit = min;
  Double_t maxFit = max;
  minFit = fitMin;
  maxFit = fitMax;
  //	minFit = 0.997;
  //	maxFit = 1.050;

  RooFitResult *fitResult = 0;

  fitResult = model.chi2FitTo(data, Save(), SumW2Error(kTRUE),
                              Range(minFit, maxFit), PrintLevel(-1));
  //	fitResult = model.fitTo(data, Save(),SumW2Error(kTRUE), Range(minFit,
  // maxFit),PrintLevel(-1));
  fitResult->Print();
  if (!fitResult)
    return;

  gROOT->SetBatch();
  TCanvas *c = new TCanvas("canvas", fTitle.Data());
  gROOT->GetListOfCanvases()->Remove(c);

  RooPlot *frame = x.frame(Title(TString::Format("%s", GetTitle()).Data()));
  data.plotOn(frame, Name("data"));
  model.plotOn(frame, Name("model"));
  //	model.plotOn(frame, Name("signal"),Components(sig), LineColor(kGreen));
  model.plotOn(frame, Name("background"), Components(bkg), LineStyle(kDashed));

  //	TPaveLabel *t1 = new TPaveLabel(0.7,0.6,0.9,0.68, Form("#chi^{2} = %f",
  // frame->chiSquare("model","data",5)),"brNDC");
  //	frame->addObject(t1);
  frame->Draw();
  //	model.paramOn(frame, Layout(0.55));

  fOutput->Add(c);

  //	TString json = TBufferJSON::ConvertToJSON(c);
  //	std::ofstream out(TString::Format("%s.json",GetName()).Data());
  //	out << json;
  //	out.close();

  if (!fFitResults)
    fFitResults = new TList();
  fFitResults->Add(fitResult);

  Printf("chi2=%f", frame->chiSquare("model", "data", 5));

  gROOT->SetBatch(kFALSE);
}
