#include <iostream>
#include <string>
#include "math.h"
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TPaveStats.h>
#include <TGaxis.h>
#include <TChain.h>
#include <TLegend.h>
#include <TStyle.h>
#include <vector>
#include "Plotvariable.h"
#include "tree2hist.h"


using std::cout;
using std::endl;




// --------------------------------------------------------
// This Script creates Histograms from TTree - Variables
// --------------------------------------------------------



int main() {
    gStyle->SetOptStat(0);          //Do not plot infobox in histogram

    
    std::vector<Plotvariable*> vec;
    std::vector<Plotvariable*> *vecp = &vec;








    // -----------------------------------
    // Start Editing here (Example given) 
    // -----------------------------------



    //Read NTuples (can be either TTrees or TChains)
    TChain MC("Bs2phimumuTuple/DecayTree");
    TChain* MCtree = &MC;
    MCtree -> Add("/afs/cern.ch/work/c/clinn/public/f2mumu/Bsf2mumu_MC_down.root");
    MCtree -> Add("/afs/cern.ch/work/c/clinn/public/f2mumu/Bsf2mumu_MC_up.root");

    //TFile* fileB = TFile::Open("/afs/cern.ch/work/c/clinn/public/f2mumu/Bsf2mumu_MC_up.root");
    //TTree *MCtreeup = dynamic_cast<TTree*>(fileB->Get("Bs2phimumuTuple/DecayTree"));


    //Define Cuts that will be used
    std::string cuts = "B0_BKGCAT == 20";


    bool normalized_plots = true;               //<-------- Normalized plots?
    int nbins = 200;                            //<-------- Default number of bins
    std::string saveto = "../plots/";



    //There are basically two types of Constructos:
    //
    //Type1: First (or only) histogram on one canvas
    //Plotvariable Type1(variable to plot, pointer to Tree, Title of Canvas, Label in legend, #bins, lower bound, upper bound, x-axis label, unit, cuts (optional), container (do not edit))
    //
    //Type2: Additional histogram(s) on same canvas
    //Plotvariable Type2(variable to plot, pointer to Tree, Label in legend, cuts(optional), container (do not edit))

    Plotvariable B_M("B0_M", MCtree, "Bs-Mass", "before truth", nbins, 5000, 5700, "m_{B}", "MeV", vecp);                          //Type1
    Plotvariable B_M_truth("B0_M", MCtree, "after truth", cuts, vecp);                                                           //Type2

    Plotvariable f2_M("phi_1020_M", MCtree, "f2-Mass", "before truth", nbins, 900, 2500, "m_{f2}", "MeV", vecp);
    Plotvariable f2_M_truth("phi_1020_M", MCtree, "afer truth", cuts, vecp);

    
    Plotvariable mumu_M("J_psi_1S_M", MCtree, "MuMu-Mass", "You should not see a legend here", nbins, 0, 4000, "m_{#{mu}#{mu}}", "MeV", vecp);


    // -----------------------------------
    // End of Editing Area
    // -----------------------------------









    // -----------------------------------
    // Plotprocess
    // -----------------------------------

    TCanvas c("canvas", "dasvancas", 800, 600);         //Create canvas
    TCanvas* cp = &c;

    TLegend legend(0.8, 0.85, 0.99, 0.99);               //Create legend
    TLegend* legendp = &legend;
    legendp -> SetFillColor(kWhite);
    legendp -> Draw();


    TH1D motherhist;                                  
    TH1D* motherhistp = &motherhist;                  //Pointer to the first histogram in the canvas (which inherits several properties)

    TH1D temphist;                                     //temporary histogram
    TH1D* temphistp = &temphist;

    unsigned int vector_size = vecp -> size();

    double maxbincontent = 0;                           //maxbincontent (needed if more plots are made in the same canvas)
    unsigned int nplots = 0;                            //number of plots on the same canvas (counter)

    std::string savepath;
    std::string norm = "";
    std::string normsame = "same";
    std::string normierte = "";
    if(normalized_plots) {
        norm = "norm";
        normsame = "normsame";
        normierte = "Normierte";
    }

    for(unsigned int i = 0; i < vector_size; i++) {
        if((vec[i] -> Getsamecanvas()) == false) {                  //Plot it on a new canvas
            maxbincontent = 0;                                      //Reset counters
            nplots = 0;
            legendp -> Clear();                                     //Clear legend in case of new canvas

            motherhistp = vec[i] -> plot(kBlue, 1, norm); 
            maxbincontent = motherhistp -> GetMaximum();

            


            savepath = saveto + vec[i]->Getname() + ".png";                                         //PROBLEM: if name contains fancy characters

            cp -> SaveAs(savepath.c_str());
        }
        else{                                                                                       //Plot it on the same canvas  
            nplots += 1;                                                                            //(number+1) of plots on same canvas (raise by one)

            if(nplots == 1) {
                temphistp = vec[i] -> plot(kRed, 1, normsame);                          //Edit the layout here
                legendp -> AddEntry(motherhistp, vec[i-1]->Getlegendname().c_str(), "l");
                legendp -> AddEntry(temphistp, vec[i]->Getlegendname().c_str(), "l");
            }
            if(nplots == 2) {
                temphistp = vec[i] -> plot(kGreen+2, 1, normsame);
                legendp -> AddEntry(temphistp, vec[i]->Getlegendname().c_str(), "l");
            }
            if(nplots > 2) {
                temphistp = vec[i] -> plot(kOrange, 1, normsame);
                legendp -> AddEntry(temphistp, vec[i]->Getlegendname().c_str(), "l");
            }

            if(maxbincontent < temphistp -> GetBinContent(temphistp->GetMaximumBin())) {
                maxbincontent = temphistp -> GetMaximum();
                motherhistp -> SetMaximum(maxbincontent + maxbincontent/10);
            }

            legendp->Draw();
            cp -> SaveAs(savepath.c_str());
        }
    }









    return 0;
}

