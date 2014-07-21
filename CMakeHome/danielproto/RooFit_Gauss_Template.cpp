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

//DooCore included
//#include <doofit/plotting/Plot/Plot.h>
//#include "doocore/lutils/lutils.h"


using std::cout;
using std::endl;
using namespace RooFit;

int main() {

// Open Real-Data B2JPsiK File (!!!!!!!!!!!!!!!!) TOSonly!!!!
TFile* fileReal = TFile::Open("/fhgfs/users/dberninghoff/NTuples/B24mu/Final/FinalData_merged_TOS.root");
if (fileReal == 0) {
// if we cannot open the file, print an error message and return immediatly
printf("Error: cannot open RealData");
return 1;
}
// get a pointer to the tree
TTree *treeReal = dynamic_cast<TTree*>(fileReal->Get("Bu2JpsiKTuple"));

treeReal->SetBranchStatus("*",0);
treeReal->SetBranchStatus("Bplus_M",1); //Bplus_M aktivieren

treeReal->SetBranchStatus("Bplus_IPCHI2_OWNPV",1); //ab hier: aktiviere Branches für Cuts
treeReal->SetBranchStatus("Bplus_ENDVERTEX_CHI2",1);
treeReal->SetBranchStatus("Bplus_ENDVERTEX_NDOF",1);
treeReal->SetBranchStatus("mu1_IPCHI2_OWNPV",1);
treeReal->SetBranchStatus("mu2_IPCHI2_OWNPV", 1);
treeReal->SetBranchStatus("mu1_PIDK",1);
treeReal->SetBranchStatus("mu2_PIDK",1);
treeReal->SetBranchStatus("mu1_PIDmu",1);
treeReal->SetBranchStatus("mu2_PIDmu",1);
treeReal->SetBranchStatus("Kaon_PIDK",1);
treeReal->SetBranchStatus("J_psi_1S_M",1);


//Cuts anwenden (erstmal ohne B+_M Window Cut)
std::string cuts = "Bplus_IPCHI2_OWNPV < 9 && Bplus_ENDVERTEX_CHI2 / Bplus_ENDVERTEX_NDOF < 9 && mu1_IPCHI2_OWNPV > 16 && mu2_IPCHI2_OWNPV > 16";
cuts += " && mu1_PIDK < 0 && mu2_PIDK < 0 && mu1_PIDmu > 0 && mu2_PIDmu > 0 && Kaon_PIDK > 5";
cuts += " && J_psi_1S_M < 3146 && J_psi_1S_M > 3046"; //bis hier: selection Cut
cuts += " && Bplus_M > 5180"; //cutte den Tail weg


//---------------------
//Let the FITTING BEGIN
//--------------------
//Lade Daten aus Tree
RooRealVar Mass("Bplus_M", "Masse B_{+}", 5180, 5500, "MeV"); //Erstelle Variable Masse für gewünschte Range

RooRealVar B_IPCHI2("Bplus_IPCHI2_OWNPV", "Bplus_IPCHI2_OWNPV", 0, 25); //Variablen, auf die Cuts angewendet werden..
RooRealVar B_ENDV_CHI2("Bplus_ENDVERTEX_CHI2", "Bplus_ENDVERTEX_CHI2", 0, 50);
RooRealVar B_ENDV_NDOF("Bplus_ENDVERTEX_NDOF", "Bplus_ENDVERTEX_NDOF", 0,6);
RooRealVar mu1_IPCHI2("mu1_IPCHI2_OWNPV", "mu1_IPCHI2_OWNPV", 0, 520000);
RooRealVar mu2_IPCHI2("mu2_IPCHI2_OWNPV", "mu2_IPCHI2_OWNPV",0,270000);
RooRealVar mu1_PIDK("mu1_PIDK", "mu1_PIDK", -210, 100);
RooRealVar mu2_PIDK("mu2_PIDK", "mu2_PIDK", -210, 100);
RooRealVar mu1_PIDmu("mu1_PIDmu", "mu1_PIDmu", -80, 30);
RooRealVar mu2_PIDmu("mu2_PIDmu", "mu2_PIDmu", -80, 30);
RooRealVar Kaon_PIDK("Kaon_PIDK", "Kaon_PIDK", -1100, 300);
RooRealVar J_psi_M("J_psi_1S_M", "J_psi_1S_M", 2900, 3220);

RooArgSet VarSet(Mass, B_IPCHI2, B_ENDV_CHI2, B_ENDV_NDOF, mu1_IPCHI2, mu2_IPCHI2, mu1_PIDK, mu2_PIDK); //Füge ersten 8 RealVars zum ArgSet hinzu
VarSet.add(mu1_PIDmu); //Füge 4 restliche RealVars hinzu
VarSet.add(mu2_PIDmu);
VarSet.add(Kaon_PIDK);
VarSet.add(J_psi_M);
RooDataSet dataSet("Datensatz_Masse", "Datensatz_Masse", treeReal, VarSet); //Lade gewünschte Daten (Bplus_M) aus dem Tree
RooDataSet* ReducedDataSet = (RooDataSet*)dataSet.reduce(cuts.c_str());

//Fiting Parameter definieren
//Signal
RooRealVar peak_gaussian_mean("gaus_mean", "Mittelwert für Gaussian des Signals", 5280, 5250, 5320, "MeV"); //Fitting parameter
RooRealVar peak_gaussian_sigma("gaus_sigma", "Sigma für Gaussian des Signals", 19, 0, 40, "MeV");
RooGaussian peak_gaussian("peak_gaussian", "Gaussian für Signal", Mass, peak_gaussian_mean, peak_gaussian_sigma); //Erstelle Guass PDF für Signal

RooRealVar peak_yield("peak_yield", "Yield des Signal-Peaks", 450000, 350000, 550000); //Definiere Yield (Anz. Events im Gauss)

//Background
RooRealVar background_parameter("bkgrd_param", "Parameter für Background e Fkt", -0.005, -0.009, -0.004, "1/MeV"); //Definiere Parameter der e-Fkt
RooExponential background("bkgrd", "Exponentialfkt für Background", Mass, background_parameter);

RooRealVar background_yield("bkgrd_yield", "Yield des Background", 34000, 20000, 50000);

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
RooPlot* DMassFrame = Mass.frame(Bins(50), Name("Masse"), Title(""));
//Plot histogram of Mass
ReducedDataSet->plotOn(DMassFrame, MarkerSize(0.9));

//Display fit parameters
totalPdf.paramOn(DMassFrame, Format("NELU", AutoPrecision(2)), Layout(0.52, 1.0, 0.9));

//Plot just the background
totalPdf.plotOn(DMassFrame, Components(background), LineColor(kGreen) );

//Plot total PDF
totalPdf.plotOn(DMassFrame, LineColor(kRed));

//Create logarithmic Canvas and save it (without parameterbox)
TCanvas* DMassCanvas = new TCanvas("DMassCanvas", "Fit of Mass", 200, 10,1000, 600);
DMassCanvas->SetLogy(1);
DMassFrame->Draw();
DMassCanvas->SaveAs("./Histogramme_Yields/1_Gauss_yield_logy.png");


//Display fit parameters
totalPdf.paramOn(DMassFrame, Format("NELU", AutoPrecision(2)), Layout(0.53, 1.0, 0.9));


//Create Canvas and put Plot on the screen (non-logarithmic)
DMassCanvas->SetLogy(0);
DMassFrame->Draw();
DMassCanvas->SaveAs("./Histogramme_Yields/1_Gauss_yield.png");


//PullPlot
//doocore::lutils::setStyle();
//doocore::lutils::PlotPulls("Gauss_Pull", DMassFrame, "Pull-Plots", true, false);

cout << "Yield:" << peak_yield.getVal() << endl;
cout << "Fehler:" << peak_yield.getError() << endl;

return 0; 
