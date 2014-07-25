#include <vector>
#include <string>
#include <TH1D.h>
#include "Plotvariable.h"
#include <TTree.h>
#include <TChain.h>
#include <TFile.h>




// -----------------------------------------------
// 1. Vergleich Signal (MC) und background sample 
// -----------------------------------------------

void daniel_comparison_MC_BKG(std::vector<Plotvariable*> *vecp, bool &normalized_plots, int &nbins, std::string &saveto) {
    
    TChain* Bkgrtree = new TChain("DecayTree");                                                               //Background tree (B_M > 5500)
    Bkgrtree -> Add("/afs/cern.ch/work/d/dberning/private/Subsamples/Bsf2mumu_Data2011_background_subsample.root");
    Bkgrtree -> Add("/afs/cern.ch/work/d/dberning/private/Subsamples/Bsf2mumu_Data2012_background_subsample.root");

    TChain* MCtree = new TChain("DecayTree");                                                                 //Truthmatched MC
    MCtree -> Add("/afs/cern.ch/work/d/dberning/private/BDT/Bsf2mumu_MC_merged_truthmatched.root");
    
    
    //Define cuts
    //Cut auf Background: schließe J_psi-Resonanz und Psi(2S) aus und Schneide auf f2_mass (official exclude areas)
    std::string bkgrcut = "(J_psi_1S_M > 3316.6 || J_psi_1S_M < 2828.4) && (J_psi_1S_M < 3535.6 || J_psi_1S_M > 3873.0) && (phi_1020_M > 1300 && phi_1020_M < 1800)";



    normalized_plots = true;                               //<-------- Normalized plots? | In case of false you can still make canvases normalised by writing "norm" in options
    nbins = 200;                                           //<-------- Default number of bins
    saveto = "../plots/1_SignalVsBkgr/";                   //<-------- Path to save it


    //There are basically two types of Constructos:
    //
    //Type1: First (or only) histogram on one canvas
    //Plotvariable Type1(variable to plot, pointer to Tree, Title of Canvas, Label in legend, #bins, lower bound, upper bound, x-axis label, unit, cuts (optional), container (do not edit))
    //
    //Type2: Additional histogram(s) on same canvas
    //Plotvariable Type2(variable to plot, pointer to Tree, Label in legend, cuts(optional), container (do not edit))


    new Plotvariable("Kplus_PT", MCtree, "Transversal Momentum of K^{+}", "Signal", nbins, 0, 5000, "p_{T}", "MeV/c", vecp, "logy");                          //Type1
    new Plotvariable("Kplus_PT", Bkgrtree, "Background", bkgrcut, vecp);                                                                             //Type2
    
    new Plotvariable("Kminus_PT", MCtree, "Transversal Momentum of K^{-}", "Signal", nbins, 0, 5000, "p_{T}", "MeV/c", vecp, "logy");                   
    new Plotvariable("Kminus_PT", Bkgrtree, "Background", bkgrcut, vecp);                                                                     

    new Plotvariable("muplus_PT", MCtree, "Transversal Momentum of #mu^{+}", "Signal", nbins, 0, 6000, "p_{T}", "MeV/c", vecp, "logy");                        
    new Plotvariable("muplus_PT", Bkgrtree, "Background", bkgrcut, vecp);                                                                         

    new Plotvariable("muminus_PT", MCtree, "Transversal Momentum of #mu^{-}", "Signal", nbins, 0, 6000, "p_{T}", "MeV/c", vecp, "logy");                      
    new Plotvariable("muminus_PT", Bkgrtree, "Background", bkgrcut, vecp);                                                                         


    
    new Plotvariable("Kplus_P", MCtree, "Momentum of K^{+}", "Signal", nbins, 0, 50000, "p", "MeV/c", vecp);                   
    new Plotvariable("Kplus_P", Bkgrtree, "Background", bkgrcut, vecp);                                                                       

    new Plotvariable("Kminus_P", MCtree, "Momentum of K^{-}", "Signal", nbins, 0, 50000, "p", "MeV/c", vecp);                    
    new Plotvariable("Kminus_P", Bkgrtree, "Background", bkgrcut, vecp);                                                            

    new Plotvariable("muplus_P", MCtree, "Momentum of #mu^{+}", "Signal", nbins, 0, 100000, "p", "MeV/c", vecp);  
    new Plotvariable("muplus_P", Bkgrtree, "Background", bkgrcut, vecp);                                                                   

    new Plotvariable("muminus_P", MCtree, "Momentum of #mu^{-}", "Signal", nbins, 0, 100000, "p", "MeV/c", vecp);
    new Plotvariable("muminus_P", Bkgrtree, "Background", bkgrcut, vecp);



    new Plotvariable("Kplus_IPCHI2_OWNPV", MCtree, "K^{+}: #chi_{IP}^{2}", "Signal", nbins, 0, 50, "#chi_{IP}^{2}", "", vecp, "logy");                   
    new Plotvariable("Kplus_IPCHI2_OWNPV", Bkgrtree, "Background", bkgrcut, vecp);                                                                       

    new Plotvariable("Kminus_IPCHI2_OWNPV", MCtree, "K^{-}: #chi_{IP}^{2}", "Signal", nbins, 0, 50, "#chi_{IP}^{2}", "", vecp);
    new Plotvariable("Kminus_IPCHI2_OWNPV", Bkgrtree, "Background", bkgrcut, vecp);                                                            

    new Plotvariable("muplus_IPCHI2_OWNPV", MCtree, "#mu^{+}: #chi_{IP}^{2}", "Signal", nbins, 0, 140, "#chi_{IP}^{2}", "", vecp);  
    new Plotvariable("muplus_IPCHI2_OWNPV", Bkgrtree, "Background", bkgrcut, vecp);                                                                   

    new Plotvariable("muminus_IPCHI2_OWNPV", MCtree, "#mu^{-}: #chi_{IP}^{2}", "Signal", nbins, 0, 140, "#chi_{IP}^{2}", "", vecp);
    new Plotvariable("muminus_IPCHI2_OWNPV", Bkgrtree, "Background", bkgrcut, vecp);


    
    new Plotvariable("B0_PT", MCtree, "Transversal Momentum of B_{s}", "Signal", nbins, -10, 30000, "p_{T}", "MeV/c", vecp);                   
    new Plotvariable("B0_PT", Bkgrtree, "Background", bkgrcut, vecp);                                                                       
                                             
    new Plotvariable("B0_P", MCtree, "Momentum of B_{s}", "Signal", nbins, 0, 300000, "p", "MeV/c", vecp);  
    new Plotvariable("B0_P", Bkgrtree, "Background", bkgrcut, vecp);                                                                   

    new Plotvariable("phi_1020_PT", MCtree, "Transversal Momentum of f_{2}", "Signal", nbins, 0, 15000, "p_{T}", "MeV/c", vecp);                   
    new Plotvariable("phi_1020_PT", Bkgrtree, "Background", bkgrcut, vecp);                                                                       
                                             
    new Plotvariable("phi_1020_P", MCtree, "Momentum of f_{2}", "Signal", nbins, 0, 200000, "p", "MeV/c", vecp);  
    new Plotvariable("phi_1020_P", Bkgrtree, "Background", bkgrcut, vecp);



    new Plotvariable("B0_ENDVERTEX_CHI2", MCtree, "B_{s}: #chi_{vtx}^{2}", "Signal", nbins, -0.2, 30, "#chi_{vtx}^{2}", "", vecp);                   
    new Plotvariable("B0_ENDVERTEX_CHI2", Bkgrtree, "Background", bkgrcut, vecp);                                                                       
                                             
    new Plotvariable("phi_1020_ENDVERTEX_CHI2", MCtree, "f_{2}: #chi_{vtx}^{2}", "Signal", nbins, -0.2, 10, "#chi_{vtx}^{2}", "", vecp);  
    new Plotvariable("phi_1020_ENDVERTEX_CHI2", Bkgrtree, "Background", bkgrcut, vecp);





    new Plotvariable("Kplus_PIDK", MCtree, "K^{+}: DLL(K - #pi)", "Signal", nbins, -40, 120, "DLL(K - #pi)", "", vecp);                   
    new Plotvariable("Kplus_PIDK", Bkgrtree, "Background", bkgrcut, vecp);                                                                       

    new Plotvariable("Kminus_PIDK", MCtree, "K^{-}: DLL(K - #pi)", "Signal", nbins, -40, 120, "DLL(K - #pi)", "", vecp);                    
    new Plotvariable("Kminus_PIDK", Bkgrtree, "Background", bkgrcut, vecp);                                                            

    new Plotvariable("Kplus_PIDp", MCtree, "K^{+}: DLL(p - #pi)", "Signal", nbins, -40, 60, "DLL(K - #pi)", "", vecp);                   
    new Plotvariable("Kplus_PIDp", Bkgrtree, "Background", bkgrcut, vecp);                                                                       

    new Plotvariable("Kminus_PIDp", MCtree, "K^{-}: DLL(p - #pi)", "Signal", nbins, -40, 60, "DLL(K - #pi)", "", vecp);                    
    new Plotvariable("Kminus_PIDp", Bkgrtree, "Background", bkgrcut, vecp);   
    
    new Plotvariable("muplus_PIDmu", MCtree, "#mu^{+}: DLL(#mu - #pi)", "Signal", nbins, -5, 16, "DLL(#mu - #pi)", "", vecp);  
    new Plotvariable("muplus_PIDmu", Bkgrtree, "Background", bkgrcut, vecp);                                                                   

    new Plotvariable("muminus_PIDmu", MCtree, "#mu^{+}: DLL(#mu - #pi)", "Signal", nbins, -5, 16, "DLL(#mu - #pi)", "", vecp);
    new Plotvariable("muminus_PIDmu", Bkgrtree, "Background", bkgrcut, vecp);



    new Plotvariable("Kplus_TRACK_CHI2NDOF", MCtree, "K^{+}: #chi_{track}^{2}/DOF", "Signal", nbins, 0, 3, "#chi_{track}^{2}/DOF", "", vecp);                          //Type1
    new Plotvariable("Kplus_TRACK_CHI2NDOF", Bkgrtree, "Background", bkgrcut, vecp);                                                                             //Type2

    new Plotvariable("Kminus_TRACK_CHI2NDOF", MCtree, "K^{-}: #chi_{track}^{2}/DOF", "Signal", nbins, 0, 3, "#chi_{track}^{2}/DOF", "", vecp);                   
    new Plotvariable("Kminus_TRACK_CHI2NDOF", Bkgrtree, "Background", bkgrcut, vecp);                                                                     

    new Plotvariable("muplus_TRACK_CHI2NDOF", MCtree, "#mu^{+}: #chi_{track}^{2}/DOF", "Signal", nbins, 0, 3, "#chi_{track}^{2}/DOF", "", vecp);                        
    new Plotvariable("muplus_TRACK_CHI2NDOF", Bkgrtree, "Background", bkgrcut, vecp);                                                                         

    new Plotvariable("muminus_TRACK_CHI2NDOF", MCtree, "#mu^{-}: #chi_{track}^{2}/DOF", "Signal", nbins, 0, 3, "#chi_{track}^{2}/DOF", "", vecp);                      
    new Plotvariable("muminus_TRACK_CHI2NDOF", Bkgrtree, "Background", bkgrcut, vecp);            
    
    
}




// -----------------------------------
// Cut on TMVA-Variable
// -----------------------------------

void daniel_current(std::vector<Plotvariable*> *vecp, bool &normalized_plots, int &nbins, std::string &saveto) {
    
    TFile* file = new TFile("/afs/cern.ch/work/d/dberning/private/BDT/Output/Data_preselected_TMVAresponse.root", "update");
    TTree* tree = (TTree*)file->Get("Bs2phimumuTuple/DecayTree");
    
    
    //Define cuts
    //Cut auf Background: schließe J_psi-Resonanz und Psi(2S) aus und Schneide auf f2_mass (official exclude areas)
    std::string cutres_jpsi_wo_response = "J_psi_1S_M > 3047 && J_psi_1S_M < 3147";
    std::string cutres_jpsi = "TMVAResponse > 0 && J_psi_1S_M > 3047 && J_psi_1S_M < 3147";

    std::string cutres_phi2s_wo_response = "J_psi_1S_M > 3636 && J_psi_1S_M < 3736";
    std::string cutres_phi2s = "TMVAResponse > 0 && J_psi_1S_M > 3636 && J_psi_1S_M < 3736";


    
    
    //std::string cutnonres = "TMVAResponse > 0 && (J_psi_1S_M < 2997 || J_psi_1S_M > 3197)";


    normalized_plots = false;                               //<-------- Normalized plots?
    nbins = 200;                                           //<-------- Default number of bins
    saveto = "../plots/TMVAcutted/";                   //<-------- Path to save it


    //There are basically two types of Constructos:
    //
    //Type1: First (or only) histogram on one canvas
    //Plotvariable Type1(variable to plot, pointer to Tree, Title of Canvas, Label in legend, #bins, lower bound, upper bound, x-axis label, unit, cuts (optional), container (do not edit))
    //
    //Type2: Additional histogram(s) on same canvas
    //Plotvariable Type2(variable to plot, pointer to Tree, Label in legend, cuts(optional), container (do not edit))


    //normalised Plots
    //J_psi_1S
    new Plotvariable("B0_M", tree, "B_{s} Mass resonant Decay J/#psi", "without TMVA-Cut", nbins, 5200, 5550, "m_{B_{s}}", "MeV", cutres_jpsi_wo_response, vecp, "norm");
    new Plotvariable("B0_M", tree, "TMVAResponse > 0", cutres_jpsi, vecp);
   
    //Psi(2S)
    new Plotvariable("B0_M", tree, "B_{s} Mass resonant Decay #psi(2S)", "without TMVA-Cut", nbins, 5200, 5550, "m_{B_{s}}", "MeV", cutres_phi2s_wo_response, vecp, "norm");
    new Plotvariable("B0_M", tree, "TMVAResponse > 0", cutres_phi2s, vecp);


    //unnormalised Plots
    //J_psi_1S
    new Plotvariable("B0_M", tree, "B_{s} Mass resonant Decay J/#psi", "without TMVA-Cut", nbins, 5200, 5550, "m_{B_{s}}", "MeV", cutres_jpsi_wo_response, vecp);
    new Plotvariable("B0_M", tree, "TMVAResponse > 0", cutres_jpsi, vecp);
   
    new Plotvariable("B0_M", tree, "B_{s} Mass resonant Decay #psi(2S)", "without TMVA-Cut", nbins, 5200, 5550, "m_{B_{s}}", "MeV", cutres_phi2s_wo_response, vecp);
    new Plotvariable("B0_M", tree, "TMVAResponse > 0", cutres_phi2s, vecp);
   
}






















// -----------------------------------
// Test
// -----------------------------------
void daniel_test(std::vector<Plotvariable*> *vecp, bool &normalized_plots, int &nbins, std::string &saveto) {
    
    TChain* MCtree = new TChain("Bs2phimumuTuple/DecayTree");
    MCtree -> Add("/afs/cern.ch/work/c/clinn/public/f2mumu/Bsf2mumu_MC_down.root");
    MCtree -> Add("/afs/cern.ch/work/c/clinn/public/f2mumu/Bsf2mumu_MC_up.root");
    
    std::string cuts = "B0_BKGCAT == 20";
    
    normalized_plots = true;                //<-------- Normalized plots?
    nbins = 200;                            //<-------- Default number of bins
    saveto = "../plots/";                   //<-------- Path to save it


    //There are basically two types of Constructos:
    //
    //Type1: First (or only) histogram on one canvas
    //Plotvariable Type1(variable to plot, pointer to Tree, Title of Canvas, Label in legend, #bins, lower bound, upper bound, x-axis label, unit, cuts (optional), container (do not edit))
    //
    //Type2: Additional histogram(s) on same canvas
    //Plotvariable Type2(variable to plot, pointer to Tree, Label in legend, cuts(optional), container (do not edit))

    new Plotvariable("B0_M", MCtree, "Bs-Mass", "before truth", nbins, 5000, 5700, "m_{B}", "MeV", vecp);                          //Type1
    new Plotvariable("B0_M", MCtree, "after truth", cuts, vecp);                                                           //Type2

    new Plotvariable("phi_1020_M", MCtree, "f2-Mass", "before truth", nbins, 900, 2500, "m_{f2}", "MeV", vecp);
    new Plotvariable("phi_1020_M", MCtree, "afer truth", cuts, vecp);

    
    new Plotvariable("J_psi_1S_M", MCtree, "MuMu-Mass", "You should not see a legend here", nbins, 0, 4000, "m_{#{mu}#{mu}}", "MeV", vecp);

}

