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

#include "tree2hist.h"

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
    TFile* fileReal = TFile::Open("/afs/cern.ch/work/d/dberning/private/BDT/Output/Data_preselected_TMVAresponse.root");
    if (fileReal == 0) {
        // if we cannot open the file, print an error message and return immediatly
        printf("Error: cannot open RealData");
        return 1;
    }
    // get a pointer to the tree
    TTree *treeReal = dynamic_cast<TTree*>(fileReal->Get("Bs2phimumuTuple/DecayTree"));

    treeReal->SetBranchStatus("*",0);
    treeReal->SetBranchStatus("B0_M",1);                  //Bs_M aktivieren

    treeReal->SetBranchStatus("J_psi_1S_M",1);       //ab hier: aktiviere Branches für Cuts (nur Cut auf Jpsi-Masse nötig)


    //Cuts anwenden (auf Jpsi-Massenfenster)
    std::string cuts = "J_psi_1S_M > 3047 && J_psi_1S_M < 3147"

    //---------------------
    //Let the FITTING BEGIN
    //--------------------
    
    //Lade Daten aus Tree
    RooRealVar Mass("B0_M", "Masse B_{s}", 5180, 5500, "MeV");                   //Erstelle Variable Masse für gewünschte Range

    RooRealVar B_IPCHI2("J_psi_1S_M", "J_psi_1S_M", 2000, 5000);         //Variablen, auf die Cuts angewendet werden..

    RooArgSet VarSet(Mass, B_IPCHI2 
    
    RooDataSet dataSet("Datensatz_Masse", "Datensatz_Masse", treeReal, VarSet);     //Lade gewünschte Daten (Bplus_M) aus dem Tree
    RooDataSet* ReducedDataSet = (RooDataSet*)dataSet.reduce(cuts.c_str());

    //Fiting Parameter definieren
    //Signal
    RooRealVar peak_gaussian_mean("gaus_mean", "Mittelwert für Gaussian des Signals", 5280, 5250, 5320, "MeV");        //Fitting parameter
    RooRealVar peak_gaussian_sigma("gaus_sigma", "Sigma für Gaussian des Signals", 19, 0, 40, "MeV");
    RooGaussian peak_gaussian("peak_gaussian", "Gaussian für Signal", Mass, peak_gaussian_mean, peak_gaussian_sigma);           //Erstelle Guass PDF für Signal

    RooRealVar peak_yield("peak_yield", "Yield des Signal-Peaks", 227000, 200000, 244008);                                          //Definiere Yield (Anz. Events im Gauss)

    //Background
    RooRealVar background_parameter("bkgrd_param", "Parameter für Background e Fkt", -0.005, -0.006, -0.004, "1/MeV");           //Definiere Parameter der e-Fkt
    RooExponential background("bkgrd", "Exponentialfkt für Background", Mass, background_parameter);

    RooRealVar background_yield("bkgrd_yield", "Yield des Background", 17000, 0, 30000);

    //Füge PDFs zu Listen hinzu
    RooArgList shapes;
    RooArgList yields;
    shapes.add(peak_gaussian);
    yields.add(peak_yield);
    shapes.add(background);
    yields.add(background_yield);

    RooAddPdf totalPdf("totalPdf", "Summe aus Signal und Background", shapes, yields);

    totalPdf.fitTo(*ReducedDataSet, Extended() );

    //----------------------
    //Plotte den ganzen Spaß
    //----------------------
    
    //CreateRooPlot object with Mass on the (x) axis
    RooPlot* DMassFrame = Mass.frame(Bins(50), Name("Masse"), Title("Massenverteilung B2JPsiK (cutted) & fitted with Gauss"));
    
    //Plot histogram of Mass
    ReducedDataSet->plotOn(DMassFrame, MarkerSize(0.9));

    //Display fit parameters
    totalPdf.paramOn(DMassFrame, Format("NELU", AutoPrecision(2)), Layout(0.52, 1.0, 0.9));

    //Plot just the background
    totalPdf.plotOn(DMassFrame, Components(background), LineColor(kGreen) );

    //Plot total PDF
    totalPdf.plotOn(DMassFrame, LineColor(kRed));

    //Create Canvas and put Plot on the screen
    TCanvas* DMassCanvas = new TCanvas("DMassCanvas", "Fit of Mass", 200, 10,1000, 600);
    DMassFrame->Draw();
    DMassCanvas->SaveAs("./Histogramme/Massfit_cutted_Gauss.png");


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

    return 0;

}
