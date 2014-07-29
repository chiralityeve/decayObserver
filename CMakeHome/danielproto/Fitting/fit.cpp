#include <iostream>
#include <string>
#include "math.h"
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TPaveStats.h>
#include <TGaxis.h>

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
    
    // Open MC-Data B2JPsiK File
    TFile* fileMC = TFile::Open("/fhgfs/users/dberninghoff/NTuples/B24mu/B2JpsiK_total_newpid.root");
    if (fileMC == 0) {
        // if we cannot open the file, print an error message and return immediatly
        printf("Error: cannot open MCData");
        return 1;
    }
    // get a pointer to the tree
    TTree *treeMC = dynamic_cast<TTree*>(fileMC->Get("DecayTree"));

    
    
    // Open Real-Data B2JPsiK File (!!!!!!!!!!!!!!!!)
    TFile* fileReal = TFile::Open("/fhgfs/users/dberninghoff/NTuples/B24mu/Data2012_stripping20_MagDown.root");
    if (fileReal == 0) {
        // if we cannot open the file, print an error message and return immediatly
        printf("Error: cannot open RealData");
        return 1;
    }
    // get a pointer to the tree
    TTree *treeReal = dynamic_cast<TTree*>(fileReal->Get("Bu2JpsiKTuple/Bu2JpsiKTuple"));

    treeReal->SetBranchStatus("*",0);
    treeReal->SetBranchStatus("Bplus_M",1);                 //nur Bplus_M aktivieren



    //Testhistogramme ausgeben
    TCanvas* canvasMC = new TCanvas("canvasmc", "canvasmc",200, 10, 800, 600);
    TCanvas* canvasReal = new TCanvas("canvasreal", "canvasreal", 200, 10, 800, 600);

    tree2hist(canvasMC, treeMC, "B_M", "B_M_MC_Uncutted", "m_B MC", 200, 4500, 6200,"", kBlue);
    canvasMC->SaveAs("./Histogramme/B_M_MC_uncutted.png");

    tree2hist(canvasReal, treeReal, "Bplus_M", "B_M_Real_Uncutted", "m_B RealData", 200, 4500, 6200,"",kBlue);
    canvasReal->SaveAs("./Histogramme/Bplus_M_Real_uncutted.png");

    //tree2hist(canvasReal, treeReal, "Bplus_M", "B_M_Real_Cutted", "m_B RealData", 200, 4500, 6200,"Bplus_M > 5200 && Bplus_M < 5350",kBlue,"norm");


    //---------------------
    //Let the FITTING BEGIN
    //--------------------
    
    //Lade Daten aus Tree
    RooRealVar Mass("Bplus_M", "Masse B_{+}", 4800, 5800, "MeV");       //Erstelle Variable Masse für gewünschte Range
    RooArgSet VarSet(Mass);                                             //Füge Masse zum ArgSet hinzu (um später an RooDataSet zu übergeben)
    RooDataSet dataSet("Datensatz_Masse", "Datensatz_Masse", treeReal, VarSet);     //Lade gewünschte Daten (Bplus_M) aus dem Tree

    //Fiting Parameter definieren
    //Signal
    RooRealVar peak_gaussian_mean("gaus_mean", "Mittelwert für Gaussian des Signals", 5280, 5250, 5310, "MeV");        //Fitting parameter
    RooRealVar peak_gaussian_sigma("gaus_sigma", "Sigma für Gaussian des Signals", 19, 0, 40, "MeV");
    RooGaussian peak_gaussian("peak_gaussian", "Gaussian für Signal", Mass, peak_gaussian_mean, peak_gaussian_sigma);           //Erstelle Guass PDF für Signal

    RooRealVar peak_yield("peak_yield", "Yield des Signal-Peaks", 400000, 350000, 450000);                                          //Definiere Yield (Anz. Events im Gauss)

    //Background
    RooRealVar background_parameter("bkgrd_param", "Parameter für Background e Fkt", -0.003, -0.0035, -0.0025, "1/MeV");           //Definiere Parameter der e-Fkt
    RooExponential background("bkgrd", "Exponentialfkt für Background", Mass, background_parameter);

    RooRealVar background_yield("bkgrd_yield", "Yield des Background", 725000, 700000, 750000);

    //Füge PDFs zu Listen hinzu
    RooArgList shapes;
    RooArgList yields;
    shapes.add(peak_gaussian);
    yields.add(peak_yield);
    shapes.add(background);
    yields.add(background_yield);

    RooAddPdf totalPdf("totalPdf", "Summe aus Signal und Background", shapes, yields);

    totalPdf.fitTo(dataSet, Extended());

    //----------------------
    //Plotte den ganzen Spaß
    //----------------------
    
    //CreateRooPlot object with Mass on the (x) axis
    RooPlot* DMassFrame = Mass.frame(Bins(50), Name("Masse"), Title("Massenverteilung B2JPsiK"));
    
    //Plot histogram of Mass
    dataSet.plotOn(DMassFrame, MarkerSize(0.9));

    //Display fit parameters
    totalPdf.paramOn(DMassFrame, Format("NELU", AutoPrecision(2)), Layout(0.508, 1.0, 0.9));

    //Plot just the background
    totalPdf.plotOn(DMassFrame, Components(background), LineColor(kGreen) );

    //Plot total PDF
    totalPdf.plotOn(DMassFrame, LineColor(kRed));

    //Create Canvas and put Plot on the screen
    TCanvas* DMassCanvas = new TCanvas("DMassCanvas", "Fit of Mass", 200, 10,1000, 600);
    DMassFrame->Draw();
    DMassCanvas->SaveAs("./Histogramme/Massfit_uncutted_Gauss.png");





    return 0;

}
