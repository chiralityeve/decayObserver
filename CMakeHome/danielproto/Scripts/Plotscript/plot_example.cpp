#include <vector>
#include <string>
#include <TH1D.h>
#include "Plotvariable.h"
#include <TTree.h>
#include <TChain.h>


void kevin_current(std::vector<Plotvariable*> *vecp, bool &normalized_plots, int &nbins, std::string &saveto) {
   
    //Load your data here (either as TChain's or TTree's - note that these files have to be created on the HEAP with the command NEW)
    TChain* MCtree = new TChain("Bs2phimumuTuple/DecayTree");
    MCtree -> Add("/afs/cern.ch/work/c/clinn/public/f2mumu/Bsf2mumu_MC_down.root");
    MCtree -> Add("/afs/cern.ch/work/c/clinn/public/f2mumu/Bsf2mumu_MC_up.root");

    
    //Define your desired cuts here, which will be used later for the plots
    std::string cuts = "B0_BKGCAT == 20";
    

    //These settings have to be set
    normalized_plots = false;                //<-------- Normalized plots? In case of false you can still produce single plots 
                                             //          normalised by adding the keyword "norm" to the optionsparameter (see below)
    nbins = 200;                             //<-------- Default number of bins
    saveto = "../plots/";                    //<-------- Path to save it


    //There are basically two types of Constructos:
    //
    //TYPE1: First (or only) histogram on one canvas
    //  Plotvariable Type1(variable to plot, pointer to Tree, Title of Canvas, Label in legend, #bins, lower bound, upper bound, 
    //                     x-axis label, unit, cuts (optional), container (do not edit), options)
    //
    //TYPE2: Additional histogram(s) on same canvas
    //  Plotvariable Type2(variable to plot, pointer to Tree, Label in legend, cuts(optional), container (do not edit), options)

    new Plotvariable("B0_M", MCtree, "Bs-Mass", "before truth", nbins, 5000, 5700, "m_{B}", "MeV", cuts, vecp, "norm,logy,logx");                          //Type1
    new Plotvariable("B0_M", MCtree, "after truth", cuts, vecp);                                                                   //Type2

    new Plotvariable("phi_1020_M", MCtree, "f2-Mass", "before truth", nbins, 900, 2500, "m_{f2}", "MeV", vecp);
    new Plotvariable("phi_1020_M", MCtree, "afer truth", cuts, vecp);

    
    new Plotvariable("J_psi_1S_M", MCtree, "MuMu-Mass", "You should not see a legend here", nbins, 0, 4000, "m_{#{mu}#{mu}}", "MeV", vecp);

}
    //Information about options:
    //currently the keywords "norm", "logy" and "logx" are allowed. "norm" is used to create single normalised histograms (when normalized_plots = false is set).
    //"logy" and "logx" are used to create logarithmic scales. Note that the options only show effect in TYPE1 constructors.
