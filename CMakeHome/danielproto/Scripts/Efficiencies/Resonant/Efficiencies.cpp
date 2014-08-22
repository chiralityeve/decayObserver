#include <iostream>
#include <fstream>

#include <string>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TPaveStats.h>
#include <TGaxis.h>
#include <TLegend.h>
#include <TStyle.h>
#include <math.h>
#include <TMath.h>


using std::cout;
using std::endl;


double getEffError(double num_all, double num_subset) {

    return (1.0/num_all)*TMath::Sqrt(num_subset*(1.0-num_subset/num_all));

}




int main() {



    //       ----------------------
    //       Open Files
    //       ----------------------

    //Truthmatched Montecarlo
    TFile* file = TFile::Open("/afs/cern.ch/work/d/dberning/private/Pruned/MC_Truthmatched/MC_merged_pruned_truthmatched_newVars.root");
    if (file == 0) {
        // if we cannot open the file, print an error message and return immediatly
        // printf("Error: cannot open
        //       http://lcg-heppkg.web.cern.ch/lcg-heppkg/ROOT/eventdata.root!\n");
        return 1;
    }

    // get a pointer to the truthmatched Tree
    TTree *tree = dynamic_cast<TTree*>(file->Get("Bs2phimumuTuple/DecayTree"));



    //Full MC (including MCDecayTree)
    TFile* file_generator = TFile::Open("/afs/cern.ch/work/d/dberning/private/RawData/Bsf2mumu_MC_Full_MCJpsi_M.root");
    if (file_generator == 0) {
        // if we cannot open the file, print an error message and return immediatly
        // printf("Error: cannot open
        //       http://lcg-heppkg.web.cern.ch/lcg-heppkg/ROOT/eventdata.root!\n");
        return 1;
    }

    // get a pointer to the generator Tree
    TTree *tree_gen = dynamic_cast<TTree*>(file_generator->Get("MCBs2phimumuTuple/MCDecayTree"));
    TTree *tree_gen_after_stripping = dynamic_cast<TTree*>(file_generator->Get("Bs2phimumuTuple/DecayTree"));


    
    //MonteCarlo with BDTresponse
    TFile* file_BDT = TFile::Open("/afs/cern.ch/work/d/dberning/private/BDT/Applicationoutput/MC_truthmatched_BDTResponse.root");
    if (file_BDT == 0) {
        // if we cannot open the file, print an error message and return immediatly
        // printf("Error: cannot open
        //       http://lcg-heppkg.web.cern.ch/lcg-heppkg/ROOT/eventdata.root!\n");
        return 1;
    }

    // get a pointer to the truthmatched Tree
    TTree *tree_BDT = dynamic_cast<TTree*>(file_BDT->Get("Bs2phimumuTuple/DecayTree"));


    cout << std::fixed;
    

    //----------------------------------
    // Generator level cut efficiency
    // ---------------------------------

    double generator_cut = 0.17171;
    double err_generator_cut = 0.00684;
   
    cout << endl << "------ Generator level Cut Efficiency -------" << endl << endl;
    cout << generator_cut << " +/- " << err_generator_cut << endl << endl;

    
    // ----------------------------------
    // Stripping-Effizienz berechnen
    // ----------------------------------
   
    std::string resonant_gen = "J_psi_1S_M > 3047 && J_psi_1S_M < 3147"; 

    double n_generated_events = tree_gen -> GetEntries(resonant_gen.c_str());
    double n_after_stripping = tree_gen_after_stripping -> GetEntries(resonant_gen.c_str());

    std::string truthmatched = resonant_gen + "&& B0_BKGCAT == 20";
    double n_truthmatched = tree_gen_after_stripping -> GetEntries(truthmatched.c_str());
    double strippingerr = getEffError(n_generated_events, n_truthmatched);


    cout << "------ Stripping Efficiency -------" << endl << endl;
    cout << "Generated Events:  " << n_generated_events << endl;
    cout << "Events after Stripping (not-truthmatched): " << n_after_stripping << endl; 
    cout << "Events after Stripping (truthmatched): " << n_truthmatched << endl << endl;
    cout << "=> Effizienz: " << n_truthmatched / n_generated_events << " +/- " << strippingerr << endl << endl;


    // ---------------------------------
    // Preselection-Effizienz berechnen
    // ---------------------------------
    
    std::string preselection = "phi_1020_M < 1800 && phi_1020_M  > 1300 && Kplus_PIDK > -3 && Kminus_PIDK > -3";
    preselection += " && (Kplus_PIDK - Kplus_PIDp) > -3 && (Kminus_PIDK - Kminus_PIDp) > -3";
    preselection += " && J_psi_1S_M > 3047 && J_psi_1S_M < 3147";     //Only Jpsi candidates

    double n_events = tree -> GetEntries(resonant_gen.c_str());
    double n_presel = tree -> GetEntries(preselection.c_str());
    double efferror = getEffError(n_events, n_presel); 

  
    cout << "------ Preselectionefficiency -------" << endl << endl;
    cout << "Events before Cuts (Truthmatched):  " << n_events << endl;
    cout << "Events after Preselection: " << n_presel << endl << endl;
    cout << "=> Effizienz: " << n_presel / n_events << " +/- " << efferror << endl << endl;


    // ---------------------------------
    // BDT Effizienz berechnen
    // ---------------------------------
    
    std::string punzi = preselection + " && TMVAResponse > 0.244909";
    std::string common = preselection + " && TMVAResponse > 0.123853";

    double n_before_BDT = tree_BDT -> GetEntries(preselection.c_str());
    double n_after_BDT_punzi = tree_BDT -> GetEntries(punzi.c_str());
    double n_after_BDT_common = tree_BDT -> GetEntries(common.c_str());


    cout << "------ BDT Efficiency --------" << endl << endl;
    cout << "Events before BDT-cut (preselected, truthmatched):  " << n_before_BDT << endl << endl;
    
    cout << "Events after Common-cut: " << n_after_BDT_common << "\t\tEffizienz: " << n_after_BDT_common / n_before_BDT << " +/- " 
        << getEffError(n_before_BDT, n_after_BDT_common) << endl;
   
    cout << "Events after Punzi-cut: " << n_after_BDT_punzi << "\t\tEffizienz: " << n_after_BDT_punzi / n_before_BDT << " +/- " 
        << getEffError(n_before_BDT, n_after_BDT_punzi) << endl << endl;
   

    // ----------------------------------
    // Trigger Effizienz berechnen
    // ----------------------------------
    
    std::string trigger =  "(B0_L0MuonDecision_TOS == 1 || B0_L0DiMuonDecision_TOS == 1)";
    trigger += " && (B0_Hlt1TrackAllL0Decision_TOS == 1 || B0_Hlt1TrackMuonDecision_TOS == 1 || B0_Hlt1DiMuonLowMassDecision_TOS == 1 || B0_Hlt1DiMuonHighMassDecision_TOS == 1 || B0_Hlt1SingleMuonHighPTDecision_TOS == 1)";
    trigger += " && (B0_Hlt2SingleMuonDecision_TOS == 1 || B0_Hlt2DiMuonDetachedDecision_TOS == 1 || B0_Hlt2DiMuonDetachedHeavyDecision_TOS == 1 || B0_Hlt2Topo2BodyBBDTDecision_TOS == 1 || B0_Hlt2Topo3BodyBBDTDecision_TOS == 1 || B0_Hlt2Topo4BodyBBDTDecision_TOS == 1 || B0_Hlt2TopoMu2BodyBBDTDecision_TOS == 1 || B0_Hlt2TopoMu3BodyBBDTDecision_TOS == 1 || B0_Hlt2TopoMu4BodyBBDTDecision_TOS == 1)";

    std::string punzi_trigger = punzi + " && " + trigger;
    std::string common_trigger = common + " && " + trigger;

    double n_after_trigger_punzi = tree_BDT -> GetEntries(punzi_trigger.c_str());
    double n_after_trigger_common = tree_BDT -> GetEntries(common_trigger.c_str());

   
    cout << "------ Trigger Efficiency --------" << endl << endl;
    cout << "Events after BDT Common-cut (preselected, truthmatched):  " << n_after_BDT_common << endl;
    cout << "Events after BDT Punzi-cut (preselected, truthmatched):  " << n_after_BDT_punzi << endl << endl;
    
    cout << "Events after Common-cut and Triggerrequ.: " << n_after_trigger_common << "\t\tEffizienz: " << n_after_trigger_common / n_after_BDT_common << " +/- " 
        << getEffError(n_after_BDT_common, n_after_trigger_common) << endl;
   
    cout << "Events after Punzi-cut and Triggerrequ: " << n_after_trigger_punzi << "\t\tEffizienz: " << n_after_trigger_punzi / n_after_BDT_punzi<< " +/- " 
        << getEffError(n_after_BDT_punzi, n_after_trigger_punzi) << endl << endl;



    // -------------------------
    // Totale Effizienz 
    // -------------------------

    double err_ohne_gencut_common = getEffError(n_generated_events, n_after_trigger_common);
    double err_ohne_gencut_punzi = getEffError(n_generated_events, n_after_trigger_punzi);

    double total_error_common = sqrt( pow(err_ohne_gencut_common, 2) + pow(err_generator_cut, 2));
    double total_error_punzi = sqrt( pow(err_ohne_gencut_punzi, 2) + pow(err_generator_cut, 2));
    
    cout << "------ Total Efficiency --------" << endl << endl;
    cout << "With Common-Cut:" << generator_cut * n_after_trigger_common / n_generated_events << " +/- " << total_error_common << endl;
    cout << "With Punzi-Cut:" << generator_cut * n_after_trigger_punzi / n_generated_events << " +/- " << total_error_punzi << endl << endl;

   }

