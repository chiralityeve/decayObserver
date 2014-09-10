#include <iostream>
#include <string>
#include "math.h"
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TPaveStats.h>
#include <TGaxis.h>
#include <TLegend.h>
#include <TStyle.h>



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
#include <RooConstVar.h> 
#include <RooPlot.h> 

using std::cout;
using std::endl;
using namespace RooFit;

int main() {
    
// -----------------------------------------------------------------------
// Get an estimate on the Signalevents for the resonant Bs2Jpsif2-Channel (after preselection and triggerrequirements)
// -----------------------------------------------------------------------
    
    
    
    // Open Data 
    TFile* fileReal = TFile::Open("/afs/cern.ch/work/d/dberning/private/BDT/Cutted/Data_resonant_triggered_punzi.root");
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
    RooRealVar B_CB_res_mean("B_CB_res_mean", "", 5370, 5340, 5400, "MeV"),
               B_CB_res_sigma("B_CB_res_sigma", "", 18, 10, 30, "MeV"),
               B_CB_res_alpha("B_CB_res_alpha", "", 1.4, 1.0, 2.0),
               B_CB_res_n("B_CB_res_n", "", 3, 2, 4);
    RooCBShape B_CB_res("B_CB_res", "", Bs_M, B_CB_res_mean, B_CB_res_sigma, B_CB_res_alpha, B_CB_res_n);

    RooRealVar KK_Gauss_res_mean("KK_Gauss_res_mean", "", 1520, 1500, 1550, "MeV"),
               KK_Gauss_res_sigma("B_CB_res_sigma", "", 18, 10, 30, "MeV");
    RooGaussian KK_Gauss_res("KK_Gauss_res", "", KK_M, KK_Gauss_res_mean, KK_Gauss_res_sigma);

    RooProdPdf Res_sig("Res_sig", "", RooArgSet(B_CB_res,KK_Gauss_res));
    RooRealVar Res_sig_yield("Res_sig_yield", "", 2300, 1800, 2800);

    //Non-resonant Signal (non-res = KK)
    RooCBShape B_CB_nonres("B_CB_nonres", "", Bs_M, B_CB_res_mean, B_CB_res_sigma, B_CB_res_alpha, B_CB_res_n); //same parameters as for the resonant decay

    RooRealVar KK_Linear_nonres_a0("KK_Linear_nonres_a0", "", 420.0, 0.0, 1000.0),
               KK_Linear_nonres_a1("KK_Lineare_nonres_a1", "", -0.2, -1, 0.0);
    RooChebychev KK_Linear_nonres("KK_Linear_nonres", "", KK_M, RooArgList(KK_Linear_nonres_a1, KK_Linear_nonres_a0));

    RooProdPdf Nonres_sig("Nonres_sig", "", RooArgSet(B_CB_nonres, KK_Linear_nonres));
    RooRealVar Nonres_sig_yield("Nonres_sig_yield", "", 4000, 1000, 6000);

    //Combinatorial Background
    RooRealVar B_BKG_lambda("B_BKG_lambda", "", -0.001, -0.1, 0.0);
    RooExponential B_BKG("B_BKG", "", Bs_M, B_BKG_lambda);

    RooRealVar KK_BKG_lambda("KK_BKG_lambda", "", -0.002, -0.1, 0.0);
    RooExponential KK_BKG("KK_BKG", "", KK_M, KK_BKG_lambda);

    RooProdPdf BKG("BKG", "", RooArgSet(B_BKG, KK_BKG));
    RooRealVar BKG_yield("BKG_yield", "", 700.0, 0.0, 1000.0);

    //Add them all together
    RooAddPdf totalPdf("totalPdf", "sum of all", /*Shapes*/RooArgList(Res_sig, Nonres_sig, BKG), /*yields*/RooArgList(Res_sig_yield, Nonres_sig_yield, BKG_yield));


    //Fit it
    totalPdf.fitTo(*ReducedDataSet, Extended());

    
    //----------------------
    //Plotte den ganzen Spaß
    //----------------------
    
    //Open TFile to save Plots
    TFile* f = new TFile("../plots/2D-Fits/Resonant_selection_punzi.root", "RECREATE");
    
    //CreateRooPlot object with Mass on the (x) axis
    RooPlot* MassFrame_Bs = Bs_M.frame(Bins(50), Name("Masse"), Title("Projection on B_{s}-mass"));
    RooPlot* MassFrame_KK = KK_M.frame(Bins(50), Name("Masse"), Title("Projection on KK-mass"));
    
    //Plot data
    ReducedDataSet->plotOn(MassFrame_Bs, MarkerSize(0.9));
    ReducedDataSet->plotOn(MassFrame_KK, MarkerSize(0.9));

    //Plot just the background
    //totalPdf.plotOn(DMassFrame, Components(background), LineColor(kGreen) );

    //Plot total PDF
    totalPdf.plotOn(MassFrame_Bs, LineColor(kBlue));
    totalPdf.plotOn(MassFrame_KK, LineColor(kBlue));

    //Create Canvas and put Plot on the screen
    TCanvas* MassCanvas_Bs = new TCanvas("MassCanvas_Bs", "Projection on B_{s}-mass", 200, 10, 1000, 600);
    MassFrame_Bs -> Draw();
    MassFrame_Bs -> Write();    //Write to TFile

    TCanvas* MassCanvas_KK = new TCanvas("MassCanvas_KK", "Projection on KK-mass", 200, 10, 1000, 600);
    MassFrame_KK -> Draw();
    MassFrame_KK -> Write();

    //Save plots
    MassCanvas_Bs -> SaveAs("../plots/2D-Fits/Projection_Bs-mass.png");
    MassCanvas_KK -> SaveAs("../plots/2D-Fits/Projection_KK-mass.png");

 
    /*
    //Display fit parameters
    totalPdf.paramOn(DMassFrame, Format("NELU", AutoPrecision(2)), Layout(0.12, 0.57, 0.9));
    DMassFrame -> Write();
    DMassFrame -> Draw();
    */
  
    
    f -> Close();



    return 0;

}
