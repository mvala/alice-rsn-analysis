#if defined(__MAKECINT__) || defined(__MAKECLING__)
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
#include <TFile.h>
#include <TList.h>
#include <TPaveLabel.h>
#include <TSystem.h>
#endif

using namespace RooFit;
void RsnShow(Int_t cutAxix, Int_t cutBinMin, Int_t cutBinMax,
             const char *filename = "root://alieos.saske.sk///eos/"
                                    "alike.saske.sk/alice/alike/PWGLF/LF_pp/"
                                    "389_20160307-1141/merge_runlist_4/"
                                    "AnalysisResults.root",
             const char *listname = "RsnOut_tpc3s", const char *sb = "Unlike",
             const char *b = "Mixing") {

  TFile::SetCacheFileDir(gSystem->HomeDirectory(), 1, 1);

  TFile *f = TFile::Open(filename);
  if (!f)
    return;
  TList *l = (TList *)f->Get(listname);
  if (!l)
    return;
  //	l->Print();

  THnSparse *sSigBg = (THnSparse *)l->FindObject(sb);
  if (!sSigBg)
    return;
  THnSparse *sBg = (THnSparse *)l->FindObject(b);
  if (!sBg)
    return;

  sSigBg->Print();
  sBg->Print();

  sSigBg->GetAxis(cutAxix)->SetRange(cutBinMin, cutBinMax);
  sBg->GetAxis(cutAxix)->SetRange(cutBinMin, cutBinMax);

  Int_t proj_show = 0;

  TH1 *hSigBg = (TH1 *)sSigBg->Projection(proj_show);
  if (!hSigBg)
    return;
  TH1 *hBg = (TH1 *)sBg->Projection(proj_show);
  if (!hBg)
    return;

  TH1 *hBgNorm = (TH1 *)hBg->Clone();

  Double_t normL = 1.05;
  Double_t normR = 1.10;

  Double_t scale = 1.0;
  scale = hSigBg->Integral(hSigBg->FindBin(normL), hSigBg->FindBin(normR));
  scale /= hBg->Integral(hBg->FindBin(normL), hBg->FindBin(normR));

  hBgNorm->Scale(scale * 0.9);

  TH1 *hSig = (TH1 *)hSigBg->Clone();
  hSig->Add(hBgNorm, -1);

  Printf("%d %f %f", hSig->GetMinimumBin(), hSig->GetMinimum(), scale);
  Double_t min = 0.99;
  Double_t max = 1.05;

  TString cutAxixName = sSigBg->GetAxis(cutAxix)->GetName();
  Double_t cutMinVal = sSigBg->GetAxis(cutAxix)->GetBinLowEdge(cutBinMin);
  Double_t cutMaxVal = sSigBg->GetAxis(cutAxix)->GetBinUpEdge(cutBinMax);

  cutAxixName += TString::Format(" [%.2f,%.2f]", cutMinVal, cutMaxVal);

  RooRealVar x("x", "x", min, max);
  RooDataHist data("data", "dataset with x", x, hSig);

  RooRealVar meanV("mean", "mean Voigtian", 1.019, 1.000, 1.050);
  RooRealVar widthV("width", "width Voigtian", 0.0045, 0.003, 0.006);
  RooRealVar sigmaV("sigma", "sigma Voigtian", 0.001 /*, 0.000,0.002*/);
  RooVoigtian sig("voigtian", "Voigtian", x, meanV, widthV, sigmaV);

  RooRealVar c0("c0", "coefficient #0", 1.0, -1., 1.);
  RooRealVar c1("c1", "coefficient #1", 0.1, -1., 1.);
  RooRealVar c2("c2", "coefficient #2", -0.1, -1., 1.);
  RooPolynomial bkg("pol", "background p.d.f.", x, RooArgList(c0, c1));
  //	RooPolynomial bkg("pol", "background p.d.f.", x, RooArgList(c0, c1,
  // c2));
  //	RooChebychev bkg("pol","background p.d.f.",x,RooArgList(c0,c1)) ;
  //	RooChebychev bkg("pol","background p.d.f.",x,RooArgList(c0,c1,c2)) ;

  //	RooRealVar fsig("fsig","signal fraction",0.5,0.,1.) ;
  //	RooAddPdf model("model","model",RooArgList(sig,bkg),fsig) ;

  // --- Construct signal+background PDF ---
  RooRealVar nsig("nsig", "#signal events", 200, 0., 1e8);
  RooRealVar nbkg("nbkg", "#background events", 800, 0., 1e8);
  RooAddPdf model("model", "v+p", RooArgList(sig, bkg), RooArgList(nsig, nbkg));

  Double_t minFit = min;
  Double_t maxFit = max;
  minFit = 1.000;
  maxFit = 1.040;
  minFit = 0.997;
  maxFit = 1.050;

  RooFitResult *fitResutl = 0;
  fitResutl =
      model.fitTo(data, Save(), SumW2Error(kTRUE), Range(minFit, maxFit));

  TCanvas *c = new TCanvas();
  RooPlot *frame = x.frame(Title(cutAxixName.Data()));
  data.plotOn(frame, Name("data"));
  model.plotOn(frame, Name("model"));
  //	model.plotOn(frame, Components(sig), LineColor(kGreen));
  model.plotOn(frame, Components(bkg), LineStyle(kDashed));

  //	TPaveLabel *t1 = new TPaveLabel(0.7,0.6,0.9,0.68, Form("#chi^{2} = %f",
  // frame->chiSquare("model","data",5)),"brNDC");
  //	frame->addObject(t1);
  //	model.paramOn(frame, Layout(0.55));
  frame->Draw();

  Printf("chi^2 = %f", frame->chiSquare("model", "data", 5));
  Printf("nsig = %f +/- %f", nsig.getVal(), nsig.getError());
  Printf("nbkg = %f +/- %f", nbkg.getVal(), nbkg.getError());
  Printf("mean = %e +/- %e", meanV.getVal(), meanV.getError());
  Printf("width = %e +/- %e", widthV.getVal(), widthV.getError());
  Printf("sigma = %e +/- %e", sigmaV.getVal(), sigmaV.getError());
  Printf("c0 = %e +/- %e", c0.getVal(), c0.getError());
  Printf("c1 = %e +/- %e", c1.getVal(), c1.getError());
  Printf("c2 = %e +/- %e", c2.getVal(), c2.getError());
}
