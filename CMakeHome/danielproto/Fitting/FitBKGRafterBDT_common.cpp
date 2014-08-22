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
#include <RooGaussian.h> 
#include <RooExponential.h>
#include <RooArgusBG.h> 
#include <RooConstVar.h> 
#include <RooPlot.h> 

using std::cout;
using std::endl;
using namespace RooFit;

int main() {
    
// -----------------------------------------------------------------------
// Get an estimate on the Signalevents for the resonant Bs2Jpsif2-Channel
// -----------------------------------------------------------------------
    
    
    
    // Open Data 
    TFile* fileReal = TFile::Open("/afs/cern.ch/work/d/dberning/private/BDT/Cutted/Data_non-resonant_triggered_common.root");
    if (fileReal == 0) {
        // if we cannot open the file, print an error message and return immediatly
        printf("Error: cannot open RealData");
        return 1;
    }
    // get a pointer to the tree
    TTree *treeReal = dynamic_cast<TTree*>(fileReal->Get("Bs2phimumuTuple/DecayTree"));

    treeReal->SetBranchStatus("*",0);
    treeReal->SetBranchStatus("B0_M",1);                  //Bs_M aktivieren
    

    


    //Cuts anwenden
    //std::string punzicut = "TMVAResponse > -2";

    //---------------------
    //Let the FITTING BEGIN
    //--------------------
    
    //Lade Daten aus Tree
    RooRealVar Bs_M("B0_M", "Masse B_{s}", 5200, 5550, "MeV");                   //Erstelle Variable Masse für gewünschte Range
    
    
    
    

    RooArgSet VarSet(Bs_M);
    
    RooDataSet dataSet("Datensatz_Masse", "Datensatz_Masse", treeReal, VarSet);     //Lade gewünschte Daten (Bplus_M) aus dem Tree
    RooDataSet* ReducedDataSet = &dataSet;

    //Fiting Parameter definieren
    //Background
    RooRealVar background_parameter("bkgrd_param", "Parameter für Background e Fkt", -0.0015, -1, 0.0, "1/MeV");           //Definiere Parameter der e-Fkt
    RooExponential background("bkgrd", "Exponentialfkt für Background", Bs_M, background_parameter);

    RooRealVar background_yield("bkgrd_yield", "Yield des Background", 100, 0, 1000);

    //Füge PDFs zu Listen hinzu
    RooArgList shapes;
    RooArgList yields;
    
    
    shapes.add(background);
    yields.add(background_yield);

    RooAddPdf totalPdf("totalPdf", "Summe aus Signal und Background", shapes, yields);

    Bs_M.setRange("fitrange1", 5200, 5291);  
    Bs_M.setRange("fitrange2", 5441, 5550);
    Bs_M.setRange("signalrange", 5316.3, 5416.3);
    Bs_M.setRange("extrapolationrange", 5291, 5441);

    totalPdf.fitTo(*ReducedDataSet, Extended(), Range("fitrange1,fitrange2") );

    //----------------------
    //Plotte den ganzen Spaß
    //----------------------
 

    //Open TFile to save Plots
    TFile* f = new TFile("../plots/BDTrareDecay_Fits/Bkgrfit_Sigregion_extrap_triggered_Commoncut.root", "RECREATE");
    
    //CreateRooPlot object with Mass on the (x) axis
    RooPlot* DMassFrame = Bs_M.frame(Bins(50), Name("Masse"), Title("Backgroundfit rare Decay after Commoncut (dashed: extrapolation)"));
    
    //Plot histogram of Mass
    ReducedDataSet->plotOn(DMassFrame, MarkerSize(0.9));


    //Plot total PDF
    totalPdf.plotOn(DMassFrame, LineColor(kRed));

    //Plot extrapolated line
    totalPdf.plotOn(DMassFrame, LineColor(kRed), LineStyle(2), LineWidth(1), Range("extrapolationrange"));

    //Plot Signalrange
    totalPdf.plotOn(DMassFrame, LineColor(kGreen), LineStyle(1), Range("signalrange"));


    //Display fit parameters
    totalPdf.paramOn(DMassFrame, Format("NELU", AutoPrecision(2)), Layout(0.5, 1.0, 0.9));

    
    
    //Create Canvas and put Plot on the screen
    TCanvas* DMassCanvas = new TCanvas("DMassCanvas", "Fit of Mass", 200, 10,1000, 600);
    
    DMassFrame->Draw();
    DMassCanvas -> SaveAs("../plots/BDTrareDecay_Fits/Bkgrfit_Sigregion_extrap_triggered_Common.png");

    /*
    DMassFrame -> SetTitle("Backgroundfit (dashed: extrapolation) - logarithmic scale");
    DMassFrame -> SetAxisRange(1000, 3000, "Y");
    DMassCanvas -> SetLogy(1);


    DMassFrame -> Write();
    DMassCanvas->SaveAs("../plots/BDT/Bkgrfit_Sigregion_extrapolation_logy.png");
    */
    

    f -> Close();
    
    RooAbsReal* intOfFunc = totalPdf.createIntegral(Bs_M, NormSet(Bs_M), Range("signalrange"));
    cout << "Normalised Integral (5316.3 - 5416.3): " << intOfFunc -> getVal() << endl;
    double bkgr_yield_in_sig_region = (intOfFunc -> getVal()) * (background_yield.getVal());
    cout << "Background im Signalwindow: " << bkgr_yield_in_sig_region << endl;


    /*
    //Signaleffizienz der verwendeten Cuts ausgeben (berechnet aus MC-Data)
    std::string cutsMC = "B_IPCHI2_OWNPV < 9 && B_ENDVERTEX_CHI2 / B_ENDVERTEX_NDOF < 9 && muplus_IPCHI2_OWNPV > 16 && muminus_IPCHI2_OWNPV > 16";
    cutsMC += " && muplus_PIDK < 0 && muminus_PIDK < 0 && muplus_DLLmu > 0 && muminus_DLLmu > 0 && kaon_DLLK > 5";
    cutsMC += " && jpsi_M < 3146 && jpsi_M > 3046 && B_M > 5180";

    double nevents = treeMC->GetEntries();
    double neventscuts = treeMC->GetEntries(cutsMC.c_str());
    double eff = neventscuts/nevents;

    cout << "Entries ohne Cuts:\t" << nevents << endl;
    cout << "Entries mit Cuts:\t" << neventscuts << endl;
    cout << "=> Effizienz:\t\t" << eff << endl;
    */

    return 0;

}
