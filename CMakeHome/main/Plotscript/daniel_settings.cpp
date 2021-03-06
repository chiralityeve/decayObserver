#include <vector>
#include <string>
#include <TH1D.h>
#include "Plotvariable.h"
#include "Plotvariable_2D.h"
#include <TTree.h>
#include <TChain.h>
#include <TFile.h>





// -----------------------------------------------
// Plots to compare PID variables (MC <-> Data)
// -----------------------------------------------

void daniel_comparePID(std::vector<Plotvariable*> *vecp, bool &normalized_plots, int &nbins, std::string &saveto) {

    //Data (pruned) after stripping
    TFile* filedata = new TFile("/afs/cern.ch/work/d/dberning/private/Pruned/Data/Data_merged_pruned_newVars.root", "READ");
    TTree* treedata = (TTree*)filedata->Get("Bs2phimumuTuple/DecayTree");

    //Resonant Data after preselection with sweights
    TFile* filesweights = new TFile("/afs/cern.ch/work/d/dberning/private/Sweighting/PIDcomparison/Data_resonant_preselected_newVars_sweights.root", "READ");
    TTree* treesweights = (TTree*)filesweights->Get("DecayTree");

    //Resonant Truthmatched Data after preselection
    TFile* fileresMC = new TFile("/afs/cern.ch/work/d/dberning/private/Pruned/MC_res_Preselected/MC_truthmatched_resonant_preselected_newVars.root", "READ");
    TTree* treeresMC = (TTree*)fileresMC->Get("Bs2phimumuTuple/DecayTree");

    //Define Cut to select resonant Decay
    std::string resonant = "J_psi_1S_M > 3047 && J_psi_1S_M < 3147";

    std::string non_resonant = "(J_psi_1S_M > 3316.6 || J_psi_1S_M < 2828.4) && (J_psi_1S_M < 3535.6 || J_psi_1S_M > 3873.0)";

    std::string preselection = "phi_1020_M > 1300 && phi_1020_M < 1800 && Kplus_PIDK > -3 && Kminus_PIDK > -3";
    preselection += " && (Kplus_PIDK - Kplus_PIDp) > -3 && (Kminus_PIDK - Kminus_PIDp) > -3";

    std::string f2_cut = "phi_1020_M > 1300 && phi_1020_M < 1800";


    normalized_plots = false;                               //<-------- Normalized plots? | In case of false you can still make canvases normalised by writing "norm" in options
    nbins = 200;                                           //<-------- Default number of bins
    saveto = "../plots/Compare_PIDvariables/";                   //<-------- Path to save it


    //There are basically two types of Constructos:
    //
    //Type1: First (or only) histogram on one canvas
    //Plotvariable Type1(variable to plot, pointer to Tree, Title of Canvas, Label in legend, #bins, lower bound, upper bound, x-axis label, unit, cuts (optional), container (do not edit))
    //
    //Type2: Additional histogram(s) on same canvas
    //Plotvariable Type2(variable to plot, pointer to Tree, Label in legend, cuts(optional), container (do not edit))


    //Compare PID variables between the two different channels
    new Plotvariable("Kplus_PIDK", treedata, "Kplus DLL(K - #pi) resonant #leftrightarrow non-resonant after cut on f2-mass", "non-resonant", 
            nbins,  -50, 70,  "Kplus DLL(K - #pi)", "", non_resonant + " && " + f2_cut, vecp, "norm");                   
    new Plotvariable("Kplus_PIDK", treedata, "resonant", resonant + " && " + f2_cut,  vecp);      
 
    new Plotvariable("Kplus_PIDp", treedata, "Kplus DLL(p - #pi) resonant #leftrightarrow non-resonant after cut on f2-mass", "non-resonant", 
            nbins,  -50, 70,  "Kplus DLL(p - #pi)", "", non_resonant + " && " + f2_cut, vecp, "norm");                   
    new Plotvariable("Kplus_PIDp", treedata, "resonant", resonant + " && " + f2_cut,  vecp);   

    new Plotvariable("Kplus_PIDK-Kplus_PIDp", treedata, "Kplus DLL(K - p) resonant #leftrightarrow non-resonant after cut on f2-mass", "non-resonant", 
            nbins,  -50, 70,  "Kplus DLL(K - p)", "",  non_resonant + " && " + f2_cut, vecp, "norm");                   
    new Plotvariable("Kplus_PIDK-Kplus_PIDp", treedata, "resonant", resonant + " && " + f2_cut,  vecp);   
    
    new Plotvariable("muplus_PIDmu", treedata, "Muplus DLL(#mu - #pi) resonant #leftrightarrow non-resonant after cut on f2-mass", "non-resonant", 
            nbins,  -5, 15,  "Muplus DLL(#mu - #pi)", "", non_resonant + " && " + f2_cut, vecp, "norm");                   
    new Plotvariable("muplus_PIDmu", treedata, "resonant", resonant + " && " + f2_cut,  vecp); 


    //Do we see a resonant Peak?
    new Plotvariable("B0_M", treedata, "B_{s}-mass after applying resonant preselection", "resonant",
            nbins, 5200, 5550, "m(B_{s})", "MeV", resonant + " && " + preselection, vecp);

    new Plotvariable("phi_1020_M", treedata, "f_{2}'-mass after applying resonant preselection", "resonant",
            nbins, 1280, 1820, "m(f_{2}')", "MeV", resonant + " && " + preselection, vecp);

    new Plotvariable("phi_1020_M", treedata, "f_{2}'-mass after resonant preselection and cut on B_{s}-mass", "resonant",
            nbins, 1280, 1820, "m(f_{2}')", "MeV", resonant + " && " + preselection + "&& B0_M > 5341 && B0_M < 5391", vecp);

  
    //Sweighted
    new Plotvariable("Kplus_PIDK", treesweights, "Kplus DLL(K - #pi) resonant after preselection", "Sweighted Data", 
            nbins,  -5, 70,  "Kplus DLL(K - #pi)", "", "sig0_yield_sw", vecp, "norm");                   
    new Plotvariable("Kplus_PIDK", treeresMC, "truthm. MC",   vecp);      
 
    new Plotvariable("Kplus_PIDp", treesweights, "Kplus DLL(p - #pi) resonant after preselection", "Sweighted Data", 
            nbins,  -50, 70,  "Kplus DLL(p - #pi)", "", "sig0_yield_sw", vecp, "norm");                   
    new Plotvariable("Kplus_PIDp", treeresMC, "truthm. MC",   vecp);   

    new Plotvariable("Kplus_PIDK-Kplus_PIDp", treesweights, "Kplus DLL(K - p) resonant after preselection", "Sweighted Data", 
            nbins,  -5, 70,  "Kplus DLL(K - p)", "", "sig0_yield_sw",  vecp, "norm");                   
    new Plotvariable("Kplus_PIDK-Kplus_PIDp", treeresMC, "truthm. MC",   vecp);   
    
    new Plotvariable("muplus_PIDmu", treesweights, "Muplus DLL(#mu - #pi) resonant after preselection", "Sweighted Data", 
            nbins,  -5, 15,  "Muplus DLL(#mu - #pi)", "", "sig0_yield_sw", vecp, "norm");                   
    new Plotvariable("muplus_PIDmu", treeresMC, "truthm. MC",   vecp); 
    
}













// -----------------------------------------------
// Plots for LHCb presentation 
// -----------------------------------------------

void daniel_LHCbpresentation(std::vector<Plotvariable*> *vecp, bool &normalized_plots, int &nbins, std::string &saveto) {

    //Data (pruned) after stripping
    TFile* filedata = new TFile("/afs/cern.ch/work/d/dberning/private/Pruned/Data/Data_merged_pruned_newVars.root", "READ");
    TTree* treedata = (TTree*)filedata->Get("Bs2phimumuTuple/DecayTree");

    //Preselected Data (+triggered | no cut on Jpsi mass so far)
    TFile* filepresel = new TFile("/afs/cern.ch/work/d/dberning/private/Pruned/Data_preselected/Data_merged_pruned_preselected_triggered_newVars.root", "READ");
    TTree* treepresel = (TTree*)filepresel->Get("Bs2phimumuTuple/DecayTree");

    //Resonant decay after BDT punzi Cut (+ triggered)
    TFile* filerespunzi = new TFile("/afs/cern.ch/work/d/dberning/private/BDT/Cutted/Data_resonant_triggered_punzi.root", "READ");
    TTree* treerespunzi = (TTree*)filerespunzi->Get("Bs2phimumuTuple/DecayTree");



    //Truthmatched MC
    TFile* fileMC = new TFile("/afs/cern.ch/work/d/dberning/private/Pruned/MC_Truthmatched/MC_merged_pruned_truthmatched_newVars.root", "READ");
    TTree* treeMC = (TTree*)fileMC->Get("Bs2phimumuTuple/DecayTree");


    //Peaking Background
    TFile* file_peakingBKGR = new TFile("/afs/cern.ch/work/d/dberning/private/Pruned/LambdaMuMu_MC/LambdaMuMu_MC_P8.root", "read");
    TTree* tree_peakingBKGR = (TTree*)file_peakingBKGR->Get("Bs2phimumuTuple/DecayTree");



    //Define Cut to select resonant Decay
    std::string resonant = "J_psi_1S_M > 3047 && J_psi_1S_M < 3147";

    std::string preselection = "phi_1020_M > 1300 && phi_1020_M < 1800 && Kplus_PIDK > -3 && Kminus_PIDK > -3";
    preselection += " && (Kplus_PIDK - Kplus_PIDp) > -3 && (Kminus_PIDK - Kminus_PIDp) > -3";

    std::string f2_cut = "phi_1020_M > 1300 && phi_1020_M < 1800";


    normalized_plots = false;                               //<-------- Normalized plots? | In case of false you can still make canvases normalised by writing "norm" in options
    nbins = 200;                                           //<-------- Default number of bins
    saveto = "../plots/0_For_Presentation/";                   //<-------- Path to save it


    //There are basically two types of Constructos:
    //
    //Type1: First (or only) histogram on one canvas
    //Plotvariable Type1(variable to plot, pointer to Tree, Title of Canvas, Label in legend, #bins, lower bound, upper bound, x-axis label, unit, cuts (optional), container (do not edit))
    //
    //Type2: Additional histogram(s) on same canvas
    //Plotvariable Type2(variable to plot, pointer to Tree, Label in legend, cuts(optional), container (do not edit))


    //Plot KK-Mass after stripping
    new Plotvariable("phi_1020_M", treedata, "", "Data", nbins, 1100, 2000, "m(KK)", "MeV/c^{2}", vecp, "norm");                   
    new Plotvariable("phi_1020_M", treeMC, "Simulation", vecp);      

    //Plot mumu-Mass after stripping (only in Data)
    new Plotvariable("J_psi_1S_M", treedata, "", "Data", nbins, 2500, 4200, "m(#mu#mu)", "MeV/c^{2}", vecp);

    //Plot resonant Bs-Mass before and after BDT punzi-Cut
    new Plotvariable("B0_M", treepresel, "", "Before BDT", nbins, 5200, 5700, "m(B_{s})", "MeV/c^{2}", resonant, vecp);
    new Plotvariable("B0_M", treerespunzi, "", "After BDT", nbins, 5200, 5700, "m(B_{s})", "MeV/c^{2}", vecp);

    //Plot Bs-Mass before and after preselection
    new Plotvariable("B0_M", treedata, "", "Data before Presel", nbins, 5200, 5700, "m(B_{s})", "MeV/c^{2}", vecp);
    new Plotvariable("B0_M", treedata, "", "Data after Presel", nbins, 5200, 5700, "m(B_{s})", "MeV/c^{2}", preselection, vecp);

    //Plot Lambda_b -> Lambda(15250)mumu potential peaking background
    new Plotvariable("B0_M", tree_peakingBKGR, "", "after f2-cut", nbins, 5200, 5700, "m(#Lambda_{b})", "MeV/c^{2}", vecp);
    new Plotvariable("B0_M", tree_peakingBKGR, "", "after complete preselection", nbins, 5200, 5700, "m(#Lambda_{b})", "MeV/c^{2}", preselection, vecp);
}




//-------------------------------------------------
// Plots of new created branches: DiMasses of Kaon
//-------------------------------------------------

void daniel_newbranches(std::vector<Plotvariable*> *vecp, bool &normalized_plots, int &nbins, std::string &saveto) {

    TFile* file = new TFile("/afs/cern.ch/work/d/dberning/private/Pruned/Data/Data_merged_pruned_newVars.root", "READ");
    TTree* tree = (TTree*)file->Get("Bs2phimumuTuple/DecayTree");

    TFile* file_afterBDT = new TFile("/afs/cern.ch/work/d/dberning/private/BDT/Applicationoutput/Bs2mumuf2_BDTselection_FINAL_newVars.root", "READ");
    TTree* tree_afterBDT = (TTree*)file_afterBDT->Get("Bs2phimumuTuple/DecayTree");

    normalized_plots = false;
    nbins = 200;
    saveto = "../plots/DiKaon_newBranches/";

    //Vorselektion
    std::string preselection = "phi_1020_M > 1300 && phi_1020_M < 1800 && Kplus_PIDK > -3 && Kminus_PIDK > -3";
    preselection += " && (Kplus_PIDK - Kplus_PIDp) > -3 && (Kminus_PIDK - Kminus_PIDp) > -3";

    //BDT-Cut
    std::string punzi = "TMVAResponse > 0.265699";


    //Plotting
    
    //Kaon Kaon
    new Plotvariable("phi_1020_M", tree, "Mass of KK", "w/o Cuts", nbins, 800, 2200, "m_{Kp}", "MeV", "", vecp); 
    new Plotvariable("phi_1020_M", tree, "after Preselection", preselection, vecp);

    new Plotvariable("phi_1020_M", tree_afterBDT, "Mass of KK after BDT (Punzi FOM)", "w/o Cuts", nbins, 800, 2200, "m_{Kp}", "MeV", punzi, vecp); 
    
    //Kaon with Proton
    new Plotvariable("Kplus_Proton_M", tree, "Mass of Kp (Kaon exchanged by a Proton)", "w/o Cuts", nbins, 1300, 2500, "m_{Kp}", "MeV", "", vecp); 
    new Plotvariable("Kplus_Proton_M", tree, "after Preselection", preselection, vecp);

    new Plotvariable("Kplus_Proton_M", tree_afterBDT, "Mass of Kp after BDT (Punzi FOM)", "w/o Cuts", nbins, 1300, 2500, "m_{Kp}", "MeV", punzi, vecp); 
    
    //Kaon with Pion
    new Plotvariable("Kplus_Pion_M", tree, "Mass of K#pi (Kaon exchanged by a Pion)", "w/o Cuts", nbins, 550, 2000, "m_{K#pi}", "MeV", "", vecp); 
    new Plotvariable("Kplus_Pion_M", tree, "after Preselection", preselection, vecp);

    new Plotvariable("Kplus_Pion_M", tree_afterBDT, "Mass of K#pi after BDT (Punzi FOM)", "w/o Cuts", nbins, 550, 2000, "m_{K#pi}", "MeV", punzi, vecp); 
   
    //Proton with Pion
    new Plotvariable("Proton_Pion_M", tree, "Mass of p#pi (Kaons exchanged by Proton and Pion)", "w/o Cuts", nbins, 800, 4000, "m_{p#pi}", "MeV", "", vecp); 
    new Plotvariable("Proton_Pion_M", tree, "after Preselection", preselection, vecp);

    new Plotvariable("Proton_Pion_M", tree_afterBDT, "Mass of p#pi after BDT (Punzi FOM)", "w/o Cuts", nbins, 800, 4000, "m_{p#pi}", "MeV", punzi, vecp); 




    //Normalised Plots
    
    //Kaon Kaon
    new Plotvariable("phi_1020_M", tree, "Normalised Mass of KK", "w/o Cuts", nbins, 800, 2200, "m_{Kp}", "MeV", "", vecp, "norm"); 
    new Plotvariable("phi_1020_M", tree, "after Preselection", preselection, vecp);

    new Plotvariable("phi_1020_M", tree_afterBDT, "after BDT", punzi, vecp);

    //Kaon with Proton
    new Plotvariable("Kplus_Proton_M", tree, "Normalised Mass of Kp (Kaon exchanged by a Proton)", "w/o Cuts", nbins, 1300, 2500, "m_{Kp}", "MeV", "", vecp, "norm"); 
    new Plotvariable("Kplus_Proton_M", tree, "after Preselection", preselection, vecp);

    new Plotvariable("Kplus_Proton_M", tree_afterBDT, "after BDT", punzi, vecp);
    
    //Kaon with Pion
    new Plotvariable("Kplus_Pion_M", tree, "Normalised Mass of K#pi (Kaon exchanged by a Pion)", "w/o Cuts", nbins, 550, 2000, "m_{K#pi}", "MeV", "", vecp, "norm"); 
    new Plotvariable("Kplus_Pion_M", tree, "after Preselection", preselection, vecp);

    new Plotvariable("Kplus_Pion_M", tree_afterBDT, "after BDT", punzi, vecp);
   
    //Proton with Pion
    new Plotvariable("Proton_Pion_M", tree, "Normalised Mass of p#pi (Kaons exchanged by Proton and Pion)", "w/o Cuts", nbins, 800, 4000, "m_{p#pi}", "MeV", "", vecp, "norm"); 
    new Plotvariable("Proton_Pion_M", tree, "after Preselection", preselection, vecp);
    
    new Plotvariable("Proton_Pion_M", tree_afterBDT, "after BDT", punzi, vecp);




}









// -----------------------------------
// Cut on TMVA-Variable for new BDT-Training (with new preselection)
// -----------------------------------

void daniel_newBDT(std::vector<Plotvariable*> *vecp, bool &normalized_plots, int &nbins, std::string &saveto) {

    TFile* file = new TFile("/afs/cern.ch/work/d/dberning/private/BDT/Applicationoutput/Bs2mumuf2_BDTselection_FINAL_newVars.root", "READ");
    TTree* tree = (TTree*)file->Get("Bs2phimumuTuple/DecayTree");

    TFile* file_BDTrareDecay = new TFile("/afs/cern.ch/work/d/dberning/private/BDT/Cutted/Bs2mumuf2_rareDecay_blinded_newVars.root", "read");
    TTree* tree_BDTrareDecay = (TTree*)file_BDTrareDecay->Get("Bs2phimumuTuple/DecayTree");

    //Define cuts
    //Cut auf Background: schließe J_psi-Resonanz und Psi(2S) aus und Schneide auf f2_mass (official exclude areas)
    std::string cutres_jpsi_wo_response = "J_psi_1S_M > 3047 && J_psi_1S_M < 3147";
    

    //Using a cut on TMVAresponse calculated with common FOM
    std::string cutres_new_jpsi = "TMVAResponse > 0.130847 && " + cutres_jpsi_wo_response;
    std::string cutres_new_jpsi_bcut = cutres_new_jpsi + " && B0_M > 5316.3 && B0_M < 5416.3";

    //Using a cut on TMVAresponse calculated with Punzi FOM
    std::string cutres_punzi_jpsi = "TMVAResponse > 0.265699 && " + cutres_jpsi_wo_response;
    std::string cutres_punzi_jpsi_bcut = cutres_punzi_jpsi + " && B0_M > 5316.3 && B0_M < 5416.3";


    //Cuts for Raredecay Backgroundlevel-Check
    std::string raredecay_common = "TMVAResponse > 0.130847";
    std::string raredecay_punzi = "TMVAResponse > 0.265699";






    //std::string cutnonres = "TMVAResponse > 0 && (J_psi_1S_M < 2997 || J_psi_1S_M > 3197)";


    normalized_plots = false;                               //<-------- Normalized plots?
    nbins = 200;                                           //<-------- Default number of bins
    saveto = "../plots/TMVAcutted_newVars_resonant_Decay/";                   //<-------- Path to save it


    //There are basically two types of Constructos:
    //
    //Type1: First (or only) histogram on one canvas
    //Plotvariable Type1(variable to plot, pointer to Tree, Title of Canvas, Label in legend, #bins, lower bound, upper bound, x-axis label, unit, cuts (optional), container (do not edit))
    //
    //Type2: Additional histogram(s) on same canvas
    //Plotvariable Type2(variable to plot, pointer to Tree, Label in legend, cuts(optional), container (do not edit))




        //Comparison while looking at Bs-Mass
        new Plotvariable("B0_M", tree, "B_{s} Mass resonant Decay J/#psi", "without TMVA-Cut", nbins, 5200, 5550, "m_{B_{s}}", "MeV", cutres_jpsi_wo_response, vecp); 
        new Plotvariable("B0_M", tree, "Common-FOM Cut", cutres_new_jpsi, vecp);
        new Plotvariable("B0_M", tree, "Punzi-FOM Cut", cutres_punzi_jpsi, vecp);

        //only Common-FOM
        new Plotvariable("B0_M", tree, "B_{s} Mass resonant Decay J/#psi  (Common-FOM Cut)", "", nbins, 5200, 5550, "m_{B_{s}}", "MeV", cutres_new_jpsi, vecp); 

        //only Punzi-FOM
        new Plotvariable("B0_M", tree, "B_{s} Mass resonant Decay J/#psi  (Punzi-FOM Cut)", "", nbins, 5200, 5550, "m_{B_{s}}", "MeV", cutres_punzi_jpsi, vecp); 

        //only between different FOMs
        new Plotvariable("B0_M", tree, "B_{s} Mass resonant Decay J/#psi", "Common-FOM Cut", nbins, 5200, 5550, "m_{B_{s}}", "MeV", cutres_new_jpsi, vecp);  
        new Plotvariable("B0_M", tree, "Punzi-FOM Cut", cutres_punzi_jpsi, vecp);






        //Comparison while looking at f2-Mass
        new Plotvariable("phi_1020_M", tree, "f_{2} Mass resonant Decay J/#psi", "without TMVA-Cut", nbins, 1250, 1850, "m_{f_{2}}", "MeV", cutres_jpsi_wo_response, vecp); 
        new Plotvariable("phi_1020_M", tree, "Common-FOM Cut", cutres_new_jpsi, vecp);
        new Plotvariable("phi_1020_M", tree, "Punzi-FOM Cut", cutres_punzi_jpsi, vecp);

        //only Common-FOM
        new Plotvariable("phi_1020_M", tree, "f_{2} Mass resonant Decay J/#psi (Cut with Common-FOM)", "", nbins, 1250, 1850, "m_{f_{2}}", "MeV", cutres_new_jpsi, vecp); 

        //only Punzi-FOM
        new Plotvariable("phi_1020_M", tree, "f_{2} Mass resonant Decay J/#psi (Cut with Punzi-FOM)", "", nbins, 1250, 1850, "m_{f_{2}}", "MeV", cutres_punzi_jpsi, vecp); 

        //only between different FOMs
        new Plotvariable("phi_1020_M", tree, "f_{2} Mass resonant Decay J/#psi", "Common-FOM Cut", nbins, 1250, 1850, "m_{f_{2}}", "MeV", cutres_new_jpsi, vecp); 
        new Plotvariable("phi_1020_M", tree, "Punzi-FOM Cut", cutres_punzi_jpsi, vecp);







        //Comparison of f2-Mass with and without cut on Bs-Mass
        new Plotvariable("phi_1020_M", tree, "f_{2} Mass resonant Decay J/#psi (TMVA-Cut at 0.13 (Common-FOM))", "with cut on Bs", nbins, 1250, 1850, "m_{f_{2}}", "MeV", cutres_new_jpsi_bcut, vecp);
        new Plotvariable("phi_1020_M", tree, "without cut on Bs", cutres_new_jpsi, vecp);

        new Plotvariable("phi_1020_M", tree, "f_{2} Mass resonant Decay J/#psi (TMVA-Cut at 0.27 (Punzi-FOM))", "with cut on Bs", nbins, 1250, 1850, "m_{f_{2}}", "MeV", cutres_punzi_jpsi_bcut, vecp);
        new Plotvariable("phi_1020_M", tree, "without cut on Bs", cutres_punzi_jpsi, vecp);



        //Check Backgroundlevel in rareDecay (under exclusion of the resonances) after applying both BDT cuts
        new Plotvariable("B0_M", tree_BDTrareDecay, "B_{s} Mass rare Decay with Common FOM (Backgroundlevel after excluded resonances)", "", nbins, 
                5100, 5600, "m_{B_{s}}", "MeV", raredecay_common, vecp);
        new Plotvariable("B0_M", tree_BDTrareDecay, "B_{s} Mass rare Decay with Punzi FOM (Backgroundlevel after excluded resonances)", "", nbins, 
                5100, 5600, "m_{B_{s}}", "MeV", raredecay_punzi, vecp);


}





// ----------------------------------------------
// BDT vs  previous Analysis  (performance Check)
// ----------------------------------------------

void daniel_BDTvsPrevAna(std::vector<Plotvariable*> *vecp, bool &normalized_plots, int &nbins, std::string &saveto) {

    TFile* file = new TFile("/afs/cern.ch/work/d/dberning/private/Prev_Analysis/Selected_and_Pruned.root", "read");
    TTree* tree = (TTree*)file->Get("Bs2phimumuTuple/DecayTree");

    TFile* file_BDT = new TFile("/afs/cern.ch/work/d/dberning/private/BDT/Applicationoutput/Bs2mumuf2_BDTselection_FINAL_newInput.root", "read");
    TTree* tree_BDT = (TTree*)file_BDT->Get("Bs2phimumuTuple/DecayTree");

    TFile* file_BDTrareDecay = new TFile("/afs/cern.ch/work/d/dberning/private/BDT/Cutted/Bs2mumuf2_rareDecay_blinded.root", "read");
    TTree* tree_BDTrareDecay = (TTree*)file_BDTrareDecay->Get("Bs2phimumuTuple/DecayTree");

    TFile* file_peakingBKGR = new TFile("/afs/cern.ch/work/d/dberning/private/Pruned/LambdaMuMu_MC/LambdaMuMu_MC_P8.root", "read");
    TTree* tree_peakingBKGR = (TTree*)file_peakingBKGR->Get("Bs2phimumuTuple/DecayTree");


    //Define cuts for BDT
    //Cut on Jpsi_Mass 
    std::string cutres_jpsi_wo_response = "J_psi_1S_M > 3047 && J_psi_1S_M < 3147";
    std::string cutres_jpsi = "TMVAResponse > 0 && J_psi_1S_M > 3047 && J_psi_1S_M < 3147";

    //Using a cut on TMVAresponse calculated with common FOM
    std::string cutres_common_jpsi = "TMVAResponse > 0.152508 && " + cutres_jpsi_wo_response;
    std::string cutres_common_jpsi_bcut = cutres_common_jpsi + " && B0_M > 5316.3 && B0_M < 5416.3";

    //Using a cut on TMVAresponse calculated with Punzi FOM
    std::string cutres_punzi_jpsi = "TMVAResponse > 0.269563 && " + cutres_jpsi_wo_response;
    std::string cutres_punzi_jpsi_bcut = cutres_punzi_jpsi + " && B0_M > 5316.3 && B0_M < 5416.3";


    //Define Cuts for prev Analysis
    std::string Bscut = "B0_M > 5341.3 && B0_M < 5391.3";       //+- 25 MeV around nominal Mass
    std::string f2cut = "phi_1020_M > 1300 && phi_1020_M < 1800";


    //Cuts for Selection of rare Decay (BDT) (resonances exlusion already done in the file)
    std::string raredecay_common = "TMVAResponse > 0.152508";
    std::string raredecay_punzi = "TMVAResponse > 0.269563";

    
    std::string proton = "(Kplus_PIDK - Kplus_PIDp) > -3 && (Kminus_PIDK - Kminus_PIDp) > -3";

    
   
    

    //std::string cutnonres = "TMVAResponse > 0 && (J_psi_1S_M < 2997 || J_psi_1S_M > 3197)";

    
    normalized_plots = false;                                //<-------- Normalized plots?
    nbins = 125;                                             //<-------- Default number of bins
    saveto = "../plots/BDTvsPrevAna/";                       //<-------- Path to save it


    //There are basically two types of Constructos:
    //
    //Type1: First (or only) histogram on one canvas
    //Plotvariable Type1(variable to plot, pointer to Tree, Title of Canvas, Label in legend, #bins, lower bound, upper bound, x-axis label, unit, cuts (optional), container (do not edit))
    //
    //Type2: Additional histogram(s) on same canvas
    //Plotvariable Type2(variable to plot, pointer to Tree, Label in legend, cuts(optional), container (do not edit))


        //Prev. Analysis (f2 Mass cut)
        new Plotvariable("B0_M", tree, "B_{s} Mass res. Decay, cutbased Selection (with cut on f2 Mass)", "Data", nbins, 5100, 5600, "m_{B_{s}}", "MeV", f2cut, vecp, "norm");
        
        //Plot potential Peaking Background alone
        new Plotvariable("B0_M", tree_peakingBKGR, "#Lambda_{b} #rightarrow #mu#muKp (p identified as K, pot. peaking BKGR, cut on f2)", "BKGR", nbins, 
                5100, 5600, "m_{B_{s}}", "MeV", f2cut, vecp);
        new Plotvariable("B0_M", tree_peakingBKGR, "#Lambda_{b} #rightarrow #mu#muKp (p identified as K, pot. peaking BKGR, without cut on f2)", "BKGR", nbins, 
                5100, 5600, "m_{B_{s}}", "MeV", vecp);
        new Plotvariable("B0_M", tree_peakingBKGR, "#Lambda_{b} #rightarrow #mu#muKp (p identified as K, pot. peaking BKGR, with cut on f2 and K_DLL(K-P) > -3)", "BKGR", 
                nbins, 5100, 5600, "m_{B_{s}}", "MeV", f2cut + "&&" + proton, vecp);


        //BDT with cut after Common FOM
        new Plotvariable("B0_M", tree_BDT, "B_{s} Mass res. Decay, BDT cut with common FOM (with cut on f2 Mass)", "Data", nbins, 5100, 5600, "m_{B_{s}}", "MeV", 
                cutres_common_jpsi, vecp, "norm");
        new Plotvariable("B0_M", tree_peakingBKGR, "pot. peaking BKGR", f2cut,  vecp);
        

        //BDT with cut after Punzi FOM
        new Plotvariable("B0_M", tree_BDT, "B_{s} Mass res. Decay, BDT cut with Punzi FOM (with cut on f2 Mass)", "Data", nbins, 5100, 5600, "m_{B_{s}}", "MeV", 
                cutres_punzi_jpsi,vecp, "norm");
        new Plotvariable("B0_M", tree_peakingBKGR, "pot. peaking BKGR", f2cut, vecp);
        
        
        //All together
        new Plotvariable("B0_M", tree, "B_{s} Mass res. Decay (with cut on f2 Mass)", "cutbased", nbins, 5100, 5600, "m_{B_{s}}", "MeV", f2cut, vecp);
        new Plotvariable("B0_M", tree_BDT, "BDT common FOM", cutres_common_jpsi, vecp);
        new Plotvariable("B0_M", tree_BDT, "BDT punzi FOM", cutres_punzi_jpsi, vecp);



        //Check Backgroundlevel in rareDecay (under exclusion of the resonances) after applying both BDT cuts
        new Plotvariable("B0_M", tree_BDTrareDecay, "B_{s} Mass rare Decay with Common FOM (Backgroundlevel after excluded resonances)", "", nbins, 
                5100, 5600, "m_{B_{s}}", "MeV", raredecay_common, vecp);
        new Plotvariable("B0_M", tree_BDTrareDecay, "B_{s} Mass rare Decay with Punzi FOM (Backgroundlevel after excluded resonances)", "", nbins, 
                5100, 5600, "m_{B_{s}}", "MeV", raredecay_punzi, vecp);







        /*
        //Bs-Mass (s. Ana-Note page 4)
        new Plotvariable("B0_M", tree, "B_{s} Mass resonant Decay J/#psi", "", nbins, 5100, 5600, "m_{B_{s}}", "MeV",  vecp); 
        new Plotvariable("B0_M", tree, "B_{s} Mass resonant Decay J/#psi (with cut on f2 Mass)", "", nbins, 5100, 5600, "m_{B_{s}}", "MeV", f2cut, vecp); 

        //f2-Mass (s. Paper page 2)
        new Plotvariable("phi_1020_M", tree, "f_{2} Mass resonant Decay J/#psi (with cut on Bs-Mass)", "", nbins, 900, 2400, "m_{f_{2}}", "MeV", Bscut, vecp, "logy"); 
        */

}








// -----------------------------------------------
// 1. Vergleich Signal (MC) und background sample 
// -----------------------------------------------

void daniel_current(std::vector<Plotvariable*> *vecp, bool &normalized_plots, int &nbins, std::string &saveto) {

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

void daniel_TMVAcut(std::vector<Plotvariable*> *vecp, bool &normalized_plots, int &nbins, std::string &saveto) {

    TFile* file = new TFile("/afs/cern.ch/work/d/dberning/private/BDT/Applicationoutput/Bs2mumuf2_BDTselection_FINAL_newInput.root", "update");
    TTree* tree = (TTree*)file->Get("Bs2phimumuTuple/DecayTree");


    //Define cuts
    //Cut auf Background: schließe J_psi-Resonanz und Psi(2S) aus und Schneide auf f2_mass (official exclude areas)
    std::string cutres_jpsi_wo_response = "J_psi_1S_M > 3047 && J_psi_1S_M < 3147";
    std::string cutres_jpsi = "TMVAResponse > 0 && J_psi_1S_M > 3047 && J_psi_1S_M < 3147";

    //Using a cut on TMVAresponse calculated with common FOM
    std::string cutres_new_jpsi = "TMVAResponse > 0.152508 && " + cutres_jpsi_wo_response;
    std::string cutres_new_jpsi_bcut = cutres_new_jpsi + " && B0_M > 5316.3 && B0_M < 5416.3";

    //Using a cut on TMVAresponse calculated with Punzi FOM
    std::string cutres_punzi_jpsi = "TMVAResponse > 0.269563 && " + cutres_jpsi_wo_response;
    std::string cutres_punzi_jpsi_bcut = cutres_punzi_jpsi + " && B0_M > 5316.3 && B0_M < 5416.3";








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


    for(int i = 0; i < 2; i++) {

        //Comparison while looking at Bs-Mass
        new Plotvariable("B0_M", tree, "B_{s} Mass resonant Decay J/#psi", "without TMVA-Cut", nbins, 5200, 5550, "m_{B_{s}}", "MeV", cutres_jpsi_wo_response, vecp); 
        new Plotvariable("B0_M", tree, "Common-FOM Cut", cutres_new_jpsi, vecp);
        new Plotvariable("B0_M", tree, "Punzi-FOM Cut", cutres_punzi_jpsi, vecp);

        //only Common-FOM
        new Plotvariable("B0_M", tree, "B_{s} Mass resonant Decay J/#psi  (Common-FOM Cut)", "", nbins, 5200, 5550, "m_{B_{s}}", "MeV", cutres_new_jpsi, vecp); 

        //only Punzi-FOM
        new Plotvariable("B0_M", tree, "B_{s} Mass resonant Decay J/#psi  (Punzi-FOM Cut)", "", nbins, 5200, 5550, "m_{B_{s}}", "MeV", cutres_punzi_jpsi, vecp); 

        //only between different FOMs
        new Plotvariable("B0_M", tree, "B_{s} Mass resonant Decay J/#psi", "Common-FOM Cut", nbins, 5200, 5550, "m_{B_{s}}", "MeV", cutres_new_jpsi, vecp);  
        new Plotvariable("B0_M", tree, "Punzi-FOM Cut", cutres_punzi_jpsi, vecp);






        //Comparison while looking at f2-Mass
        new Plotvariable("phi_1020_M", tree, "f_{2} Mass resonant Decay J/#psi", "without TMVA-Cut", nbins, 1250, 1850, "m_{f_{2}}", "MeV", cutres_jpsi_wo_response, vecp); 
        new Plotvariable("phi_1020_M", tree, "Common-FOM Cut", cutres_new_jpsi, vecp);
        new Plotvariable("phi_1020_M", tree, "Punzi-FOM Cut", cutres_punzi_jpsi, vecp);

        //only Common-FOM
        new Plotvariable("phi_1020_M", tree, "f_{2} Mass resonant Decay J/#psi (Cut with Common-FOM)", "", nbins, 1250, 1850, "m_{f_{2}}", "MeV", cutres_new_jpsi, vecp); 

        //only Punzi-FOM
        new Plotvariable("phi_1020_M", tree, "f_{2} Mass resonant Decay J/#psi (Cut with Punzi-FOM)", "", nbins, 1250, 1850, "m_{f_{2}}", "MeV", cutres_punzi_jpsi, vecp); 

        //only between different FOMs
        new Plotvariable("phi_1020_M", tree, "f_{2} Mass resonant Decay J/#psi", "Common-FOM Cut", nbins, 1250, 1850, "m_{f_{2}}", "MeV", cutres_new_jpsi, vecp); 
        new Plotvariable("phi_1020_M", tree, "Punzi-FOM Cut", cutres_punzi_jpsi, vecp);







        //Comparison of f2-Mass with and without cut on Bs-Mass
        new Plotvariable("phi_1020_M", tree, "f_{2} Mass resonant Decay J/#psi (TMVA-Cut at 0.15 (Common-FOM))", "with cut on Bs", nbins, 1250, 1850, "m_{f_{2}}", "MeV", cutres_new_jpsi_bcut, vecp);
        new Plotvariable("phi_1020_M", tree, "without cut on Bs", cutres_new_jpsi, vecp);

        new Plotvariable("phi_1020_M", tree, "f_{2} Mass resonant Decay J/#psi (TMVA-Cut at 0.27 (Punzi-FOM))", "with cut on Bs", nbins, 1250, 1850, "m_{f_{2}}", "MeV", cutres_punzi_jpsi_bcut, vecp);
        new Plotvariable("phi_1020_M", tree, "without cut on Bs", cutres_punzi_jpsi, vecp);

        nbins = 80;

    }




}








// ----------------------------------------
// Investigate bump at ~5800 MeV in B0_M
// ---------------------------------------

void daniel_current2(std::vector<Plotvariable*> *vecp, bool &normalized_plots, int &nbins, std::string &saveto) {

    TFile* file = new TFile("/afs/cern.ch/work/d/dberning/private/Pruned/Data/Data_merged_pruned.root", "READ");
    TTree* tree = (TTree*)file->Get("Bs2phimumuTuple/DecayTree");


    //Define cuts
    std::string cut_f2 = "phi_1020_M > 1300 && phi_1020_M < 1800 &&";

    std::string cut_pidkm3 = cut_f2 + "Kplus_PIDK > -3 && Kminus_PIDK > -3";
    std::string cut_pidk0 = cut_f2 + "Kplus_PIDK > 0 && Kminus_PIDK > 0";
    std::string cut_pidk5 = cut_f2 + "Kplus_PIDK > 5 && Kminus_PIDK > 5";
    std::string cut_pidk10 = cut_f2 + "Kplus_PIDK > 10 && Kminus_PIDK > 10";





    normalized_plots = false;                                   //<-------- Normalized plots?
    nbins = 200;                                                //<-------- Default number of bins
    saveto = "../plots/Bump_investigation/";                   //<-------- Path to save it


    //There are basically two types of Constructos:
    //
    //Type1: First (or only) histogram on one canvas
    //Plotvariable Type1(variable to plot, pointer to Tree, Title of Canvas, Label in legend, #bins, lower bound, upper bound, x-axis label, unit, cuts (optional), container (do not edit))
    //
    //Type2: Additional histogram(s) on same canvas
    //Plotvariable Type2(variable to plot, pointer to Tree, Label in legend, cuts(optional), container (do not edit))


    new Plotvariable("B0_M", tree, "B_{s} Mass with Kaon PID_K > 0 after stripping", "", nbins, 5200, 6200, "m_{B_{s}}", "MeV", cut_pidk0, vecp);
    new Plotvariable("B0_M", tree, "B_{s} Mass with Kaon PID_K > 5 after stripping", "", nbins, 5200, 6200, "m_{B_{s}}", "MeV", cut_pidk5, vecp);
    new Plotvariable("B0_M", tree, "B_{s} Mass with Kaon PID_K > 10 after stripping", "", nbins, 5200, 6200, "m_{B_{s}}", "MeV", cut_pidk10, vecp);


    new Plotvariable_2D("B0_M:J_psi_1S_M", tree, "2D-Plot m_{B_{s}} : m_{J#psi} after preselection", nbins, 2000, 5000, 4500, 7000, "m_{J#psi}", "MeV", "m_{B_{s}}", "MeV", cut_pidkm3, vecp, "colz");
    new Plotvariable_2D("B0_M:J_psi_1S_M", tree, "2D-Plot m_{B_{s}} : m_{J#psi} after preselection", nbins, 2000, 5000, 4500, 7000, "m_{J#psi}", "MeV", "m_{B_{s}}", "MeV", cut_pidkm3, vecp, "");

    new Plotvariable_2D("B0_M:phi_1020_M", tree, "2D-Plot m_{B_{s}} : m_{f_{2}} after preselection", nbins, 1280, 1820, 4500, 7000, "m_{f_{2}}", "MeV", "m_{B_{s}}", "MeV", cut_pidkm3, vecp, "colz");
    new Plotvariable_2D("B0_M:phi_1020_M", tree, "2D-Plot m_{B_{s}} : m_{f_{2}} after preselection", nbins, 1280, 1820, 4500, 7000, "m_{f_{2}}", "MeV", "m_{B_{s}}", "MeV", cut_pidkm3, vecp, "");
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

