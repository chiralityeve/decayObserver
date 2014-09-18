#include <iostream>
#include <string>
#include "math.h"
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH1.h>
#include <TPaveStats.h>
#include <TGaxis.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TH2.h>
#include <TPad.h>
#include <TF1.h>



//RooFit includes
#include <RooRealVar.h> 
#include <RooDataSet.h> 
#include <RooAbsPdf.h> 
#include <RooAddPdf.h> 
#include <RooProdPdf.h>
#include <RooGaussian.h>
#include <RooChebychev.h>
#include <RooExponential.h>
#include <RooArgusBG.h>
#include <RooCBShape.h>
#include <RooBreitWigner.h>
#include <RooConstVar.h> 
#include <RooPlot.h>
#include <RooHist.h>

using std::cout;
using std::endl;
using namespace RooFit;

int main() {
    
// -----------------------------------------------------------------------
// Get an estimate on the Signalevents for the resonant Bs2Jpsif2-Channel (after preselection and triggerrequirements)
// -----------------------------------------------------------------------
    
    
    
    // Open Data 
    TFile* fileReal = TFile::Open("/afs/cern.ch/work/d/dberning/private/BDT/Cutted/Data_non-resonant_triggered_punzi_unblinded.root");
    if (fileReal == 0) {
        // if we cannot open the file, print an error message and return immediatly
        printf("Error: cannot open RealData");
        return 1;
    }
    // get a pointer to the tree
    TTree *treeReal = dynamic_cast<TTree*>(fileReal->Get("Bs2phimumuTuple/DecayTree"));

    treeReal->SetBranchStatus("*",0);
    treeReal->SetBranchStatus("B0_M",1);                  //Bs_M aktivieren
    treeReal->SetBranchStatus("phi_1020_M",1);            //KK_M aktivieren

    


    //Cuts anwenden (auf Jpsi-Massenfenster)
    //std::string cuts = "J_psi_1S_M > 3047 && J_psi_1S_M < 3147";

    //---------------------
    //Let the FITTING BEGIN
    //--------------------
    
    //Lade Daten aus Tree
    RooRealVar Bs_M("B0_M", "Mass B_{s}", 5150, 5700, "MeV");                   //Erstelle Variable Masse für gewünschte Range
    RooRealVar KK_M("phi_1020_M", "KK mass", 1300, 1800, "MeV");
    

    RooArgSet VarSet(Bs_M, KK_M);
    
    RooDataSet dataSet("Datensatz_Masse", "Datensatz_Masse", treeReal, VarSet);     //Lade gewünschte Daten (Bplus_M) aus dem Tree
    RooDataSet* ReducedDataSet = &dataSet;

    //Fiting Parameter definieren


    //Resonant Signal (resonant = f2)
    RooRealVar B_CB1_res_mean("B_CB1_res_mean", "", 5369.96, "MeV"),
               B_CB1_res_sigma("B_CB1_res_sigma", "", 17.4881, "MeV"),
               B_CB1_res_alpha("B_CB1_res_alpha", "", 1.31874),
               B_CB1_res_n("B_CB1_res_n", "", 6.92291);
    
    //RooRealVar mean is the same as B_CB1
    RooRealVar B_CB2_res_sigma("B_CB2_res_sigma", "", 68.9154, "MeV");
               //same alpha
               //same n
    RooRealVar B_DCB_coefficient("B_DCB_coefficient", "", 0.936966);
    
    RooCBShape B_CB1_res("B_CB1_res", "", Bs_M, B_CB1_res_mean, B_CB1_res_sigma, B_CB1_res_alpha, B_CB1_res_n);
    RooCBShape B_CB2_res("B_CB2_res", "", Bs_M, B_CB1_res_mean, B_CB2_res_sigma, B_CB1_res_alpha, B_CB1_res_n);
    RooAddPdf  B_DCB_res("B_DCB_res", "", B_CB1_res, B_CB2_res, B_DCB_coefficient);

    RooRealVar KK_BreitWigner_res_mean("KK_BreitWigner_res_mean", "", 1521.55, "MeV"),
               KK_BreitWigner_res_sigma("KK_BreitWigner_res_sigma", "", 46.7051, "MeV");
    RooGaussian KK_BreitWigner_res("KK_BreitWigner_res", "", KK_M, KK_BreitWigner_res_mean, KK_BreitWigner_res_sigma);

    RooProdPdf Res_sig("Res_sig", "", RooArgSet(B_DCB_res,KK_BreitWigner_res));
    RooRealVar Res_sig_yield("Res_sig_yield", "", 12, 0, 50);




    //Non-resonant Signal (non-res = KK)
    //for Bs-mass use same DCB as for the resonant channel

    RooRealVar KK_Exponential_nonres_lambda("KK_Exponential_nonres_lambda", "", -0.00114470);
              
    RooExponential KK_Exponential_nonres("KK_Exponential_nonres", "", KK_M, KK_Exponential_nonres_lambda);

    RooProdPdf Nonres_sig("Nonres_sig", "", RooArgSet(B_DCB_res, KK_Exponential_nonres));

    //Fixed yieldratio from controlchannel
    RooFormulaVar Nonres_sig_yield("Nonres_sig_yield", "Res_sig_yield*1.548444379", RooArgList(Res_sig_yield));
    //Non-fixed yield for non-resonant signal
    //RooRealVar Nonres_sig_yield("Nonres_sig_yield", "", 24, 0, 100);




    //Combinatorial Background
    RooRealVar B_BKG_lambda("B_BKG_lambda", "", -0.00410129, -0.1, 0.0);    //free
    //RooRealVar B_BKG_lambda("B_BKG_lambda", "", -0.00410129);    //fixed
    RooExponential B_BKG("B_BKG", "", Bs_M, B_BKG_lambda);

    RooRealVar KK_BKG_lambda("KK_BKG_lambda", "", -0.00257055, -0.1, 0.0);  //free  
    //RooRealVar KK_BKG_lambda("KK_BKG_lambda", "", -0.00257055);  //fixed
    RooExponential KK_BKG("KK_BKG", "", KK_M, KK_BKG_lambda);

    RooProdPdf BKG("BKG", "", RooArgSet(B_BKG, KK_BKG));
    RooRealVar BKG_yield("BKG_yield", "", 50, 0.0, 400);




    //Add them all together
    RooAddPdf totalPdf("totalPdf", "sum of all", /*Shapes*/RooArgList(Res_sig, Nonres_sig, BKG), /*yields*/RooArgList(Res_sig_yield, Nonres_sig_yield, BKG_yield));


    //Fit it
    totalPdf.fitTo(*ReducedDataSet, Extended());

    
    //----------------------
    //Plotte den ganzen Spaß
    //----------------------
    
    //Ordner
    std::string ordner2 = "free_yields";
    std::string ordner1 = "open_ratio_free_BKG";
    std::string ordner = "fixed_ratio_free_BKG";
    
    //Open TFile to save Plots
    TFile* f = new TFile(("../plots/2D-Fits/Non-Res/"+ordner+"/Non-Resonant_selection_punzi.root").c_str(), "RECREATE");
    
    //CreateRooPlot object with Mass on the (x) axis
    RooPlot* MassFrame_Bs = Bs_M.frame(Bins(50), Name("Masse"), Title("2D-Fit of B_{s}- and KK-mass | Projection on B_{s}-mass"));
    RooPlot* MassFrame_KK = KK_M.frame(Bins(50), Name("Masse"), Title("2D-Fit of B_{s}- and KK-mass | Projection on KK-mass"));

    //Create Pullplots
    RooPlot* Bs_FramePull = Bs_M.frame(RooFit::Bins(50), RooFit::Name("Bs_M_Pull"), RooFit::Title(""));
    
    RooPlot* KK_FramePull = KK_M.frame(RooFit::Bins(50), RooFit::Name("KK_M_Pull"), RooFit::Title(""));

    
    //Plot data
    ReducedDataSet->plotOn(MassFrame_Bs, MarkerSize(0.9));
    ReducedDataSet->plotOn(MassFrame_KK, MarkerSize(0.9));

    //Plot just the background
    //totalPdf.plotOn(DMassFrame, Components(background), LineColor(kGreen) );



    //Plot resonant Signal
    totalPdf.plotOn(MassFrame_Bs, Components(Res_sig), LineColor(kRed), LineStyle(2), LineWidth(2), Name("plot_res_sig"));
    totalPdf.plotOn(MassFrame_KK, Components(Res_sig), LineColor(kRed), LineStyle(2), LineWidth(2));

    //Plot nonresonant Signal
    totalPdf.plotOn(MassFrame_Bs, Components(Nonres_sig), LineColor(kOrange-3), LineStyle(2), LineWidth(2), Name("plot_nonres_sig"));
    totalPdf.plotOn(MassFrame_KK, Components(Nonres_sig), LineColor(kOrange-3), LineStyle(2), LineWidth(2));

    //Plot combinatorial
    totalPdf.plotOn(MassFrame_Bs, Components(BKG), LineColor(kGreen+2), LineStyle(2), LineWidth(2), Name("plot_comb_bkg"));
    totalPdf.plotOn(MassFrame_KK, Components(BKG), LineColor(kGreen+2), LineStyle(2), LineWidth(2));

    //Plot total PDF
    totalPdf.plotOn(MassFrame_Bs, LineColor(kBlue), Name("plot_total_pdf"), LineWidth(2));
    //totalPdf.paramOn(MassFrame_Bs, Format("NELU", AutoPrecision(2)), Layout(0.12, 0.57, 0.9));
    totalPdf.plotOn(MassFrame_KK, LineColor(kBlue), LineWidth(2));

    //Plot legend
    TLegend* legend = new TLegend(0.70, 0.70, 0.89, 0.89);
    legend -> SetFillColor(kWhite);
    legend -> SetBorderSize(0);


    //Create Canvas and put Plot on the screen
    //Bs
    RooHist* Bs_HisPull = MassFrame_Bs->pullHist();
    Bs_FramePull -> addPlotable(Bs_HisPull, "P");
    Bs_FramePull -> GetYaxis()->SetRangeUser(-5,5);

    TCanvas* MassCanvas_Bs = new TCanvas("MassCanvas_Bs", "Projection on B_{s}-mass", 200, 10, 1000, 600);
    TPad* Bs_Toppad = new TPad("Bs_upperPad", "Bs_upperPad", 0., 0.2, 1., 1.);
    TPad* Bs_Bottompad = new TPad("Bs_bottomPad", "Bs_bottomPad", 0.,0., 1., .2);
    TF1* pFuncDummy = new TF1("dummyfunc", "0", 0., 6000.);
    
    Bs_Toppad -> Draw();
    Bs_Bottompad -> Draw();

    Bs_Toppad -> cd();
    MassFrame_Bs -> Draw();
    legend->AddEntry(MassFrame_Bs->findObject("plot_res_sig"), "B_{s} #rightarrow f_{2}' #mu^{+} #mu^{-}", "lp");
    legend->AddEntry(MassFrame_Bs->findObject("plot_nonres_sig"), "B_{s} #rightarrow K^{+} K^{-} #mu^{+} #mu^{-}", "lp");
    legend->AddEntry(MassFrame_Bs->findObject("plot_comb_bkg"), "Comb. BKG", "lp");
    legend->AddEntry(MassFrame_Bs->findObject("plot_total_pdf"), "Total Pdf", "lp");
    legend -> Draw();
    
    
    
    Bs_Bottompad -> cd();
    Bs_Bottompad->SetTopMargin(0.5*Bs_Bottompad->GetBottomMargin());
    Bs_Bottompad->SetTicks(1, 1);
    Bs_FramePull->SetTitle("");
    Bs_FramePull->GetYaxis()->SetNdivisions(205, true);	
    Bs_FramePull->GetYaxis()->SetLabelSize(MassFrame_Bs->GetYaxis()->GetLabelSize()*Bs_Toppad->GetAbsHNDC()/Bs_Bottompad->GetAbsHNDC());
    Bs_FramePull->GetXaxis()->SetLabelSize(0.);
    Bs_FramePull->GetXaxis()->SetTitleSize(0.);
    Bs_FramePull->Draw();
    pFuncDummy->Draw("same");

    MassFrame_Bs -> Write();    //Write to TFile
    
    //KK
    RooHist* KK_HisPull = MassFrame_KK->pullHist();
    KK_FramePull -> addPlotable(KK_HisPull, "P");
    KK_FramePull -> GetYaxis()->SetRangeUser(-5,5);
    
    TCanvas* MassCanvas_KK = new TCanvas("MassCanvas_KK", "Projection on KK-mass", 200, 10, 1000, 600);
    TPad* KK_Toppad = new TPad("KK_upperPad", "KK_upperPad", 0., 0.2, 1., 1.);
    TPad* KK_Bottompad = new TPad("KK_bottomPad", "KK_bottomPad", 0.,0., 1., .2);
    
    
    KK_Toppad -> Draw();
    KK_Bottompad -> Draw();

    KK_Toppad -> cd();

    MassFrame_KK -> Draw();
    legend -> Draw();
    
    

    KK_Bottompad -> cd();
    KK_Bottompad->SetTopMargin(0.5*KK_Bottompad->GetBottomMargin());
    KK_Bottompad->SetTicks(1, 1);
    KK_FramePull->SetTitle("");
    KK_FramePull->GetYaxis()->SetNdivisions(205, true);	
    KK_FramePull->GetYaxis()->SetLabelSize(MassFrame_KK->GetYaxis()->GetLabelSize()*KK_Toppad->GetAbsHNDC()/KK_Bottompad->GetAbsHNDC());
    KK_FramePull->GetXaxis()->SetLabelSize(0.);
    KK_FramePull->GetXaxis()->SetTitleSize(0.);
    KK_FramePull->Draw();
    pFuncDummy->Draw("same");
    
    MassFrame_KK -> Write();


    //Save plots
    MassCanvas_Bs -> SaveAs(("../plots/2D-Fits/Non-Res/"+ordner+"/Projection_Bs-mass.png").c_str());
    Bs_Toppad -> SetLogy(1); 
    MassCanvas_Bs -> SaveAs(("../plots/2D-Fits/Non-Res/"+ordner+"/Projection_Bs-mass_logy.png").c_str());
    Bs_Toppad -> SetLogy(0);
    MassCanvas_KK -> SaveAs(("../plots/2D-Fits/Non-Res/"+ordner+"/Projection_KK-mass.png").c_str());



    //Do also a 2D plot
    TCanvas* MassCanvas_2D = new TCanvas("MassCanvas_2D", "Total Pdf after 2D-Fit", 200, 10, 1000, 600);

    TH2D* fithist = (TH2D*)totalPdf.createHistogram("fithist", Bs_M, Binning(50), YVar(KK_M, Binning(50))); 
    //totalPdf.fillHistogram(fithist, RooArgList(Bs_M, KK_M));
    fithist -> SetLineColor(kRed);

    TH2D* datahist = (TH2D*)ReducedDataSet->createHistogram("datahist", Bs_M, Binning(50), YVar(KK_M, Binning(50))); 

    //ReducedDataSet->fillHistogram(datahist, RooArgList(Bs_M, KK_M));

    datahist -> Draw("LEGO2");
    datahist -> Write();
    MassCanvas_2D -> SaveAs(("../plots/2D-Fits/Non-Res/"+ordner+"/Data_gen_2Dhistogram.png").c_str());

    fithist -> Draw("SURF,same");
    fithist -> Write();
    MassCanvas_2D -> SaveAs(("../plots/2D-Fits/Non-Res/"+ordner+"/2D-Fit_overlaid.png").c_str());

    fithist -> Draw("SURF");
    fithist -> Write();
    MassCanvas_2D -> SaveAs(("../plots/2D-Fits/Non-Res/"+ordner+"/PDF_gen_2Dhistogram.png").c_str());





  
    
    f -> Close();



    return 0;

}
