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

#include "Value_w_Err.h"


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
    TFile* file = TFile::Open("/afs/cern.ch/work/d/dberning/private/BDT_w_bdtana/Peaking_BKG_MCs/B2KstarMuMu_MC_pruned_TMVAResponse.root");
    if (file == 0) {
        // if we cannot open the file, print an error message and return immediatly
        // printf("Error: cannot open
        //       http://lcg-heppkg.web.cern.ch/lcg-heppkg/ROOT/eventdata.root!\n");
        return 1;
    }

    // get a pointer to the truthmatched Tree
    TTree *tree = dynamic_cast<TTree*>(file->Get("Bs2phimumuTuple/DecayTree"));



    //Full MC (including MCDecayTree)
    TFile* file_generator = TFile::Open("/afs/cern.ch/work/d/dberning/private/Pruned/Peaking_BKG_MCs/B2KstarMuMu_MC_pruned.root");
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
    TFile* file_BDT = TFile::Open("/afs/cern.ch/work/d/dberning/private/BDT_w_bdtana/Peaking_BKG_MCs/B2KstarMuMu_MC_pruned_TMVAResponse.root");
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

    double generator_cut = 0.16448;
    double err_generator_cut = 0.00060;

    double rel_err_generator_cut = err_generator_cut / generator_cut;
   
    cout << endl << "---------------------------------------------";
    cout << endl << "-----------------B2KstarMuMu-----------------";
    cout << endl << "---------------------------------------------" << endl;


    cout << endl << "------ Generator level Cut Efficiency -------" << endl << endl;
    cout << generator_cut << " +/- " << err_generator_cut << endl << endl;

    
    // ----------------------------------
    // Stripping-Effizienz berechnen
    // ----------------------------------
   
    //std::string resonant_gen = "J_psi_1S_M > 3047 && J_psi_1S_M < 3147"; 

    double n_generated_events = tree_gen -> GetEntries();
    double n_after_stripping = tree_gen_after_stripping -> GetEntries();

    //std::string truthmatched = resonant_gen + "";
    double n_truthmatched = tree_gen_after_stripping -> GetEntries();
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
    //preselection += " && J_psi_1S_M > 3047 && J_psi_1S_M < 3147";     //Only Jpsi candidates

    double n_events = tree -> GetEntries();
    double n_presel = tree -> GetEntries(preselection.c_str());
    double efferror = getEffError(n_events, n_presel); 

  
    cout << "------ Preselectionefficiency (not cut on Jpsi) -------" << endl << endl;
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

    //Error of one factor
    double eff_ohne_gencut_common = n_after_trigger_common / n_generated_events;
    
    double err_ohne_gencut_common = getEffError(n_generated_events, n_after_trigger_common);
    double rel_err_ohne_gencut_common = err_ohne_gencut_common / eff_ohne_gencut_common;

    
    
    double eff_ohne_gencut_punzi = n_after_trigger_punzi / n_generated_events;
    
    double err_ohne_gencut_punzi = getEffError(n_generated_events, n_after_trigger_punzi);
    double rel_err_ohne_gencut_punzi = err_ohne_gencut_punzi / eff_ohne_gencut_punzi;

    //Error of second factor already calculated (gencut)
    double total_eff_common = eff_ohne_gencut_common * generator_cut;
    double total_rel_error_common = sqrt( pow(rel_err_ohne_gencut_common, 2) + pow(rel_err_generator_cut, 2));
    double total_error_common = total_rel_error_common * total_eff_common;
    
    double total_eff_punzi = eff_ohne_gencut_punzi * generator_cut;
    double total_rel_error_punzi = sqrt( pow(rel_err_ohne_gencut_punzi, 2) + pow(rel_err_generator_cut, 2));
    double total_error_punzi = total_rel_error_punzi * total_eff_punzi;
    
    cout << "------ Total Efficiency --------" << endl << endl;
    cout << "With Common-Cut: (" << total_eff_common*100 << " +/- " << total_error_common*100 << ") %" <<  endl;
    cout << "With Punzi-Cut: (" << total_eff_punzi*100  << " +/- " << total_error_punzi*100 << ") %" << endl << endl;






    // -------------------------------------
    // Verrechnen mit Lumi und Effizienzen
    // --------------------------------------

    //Lumi in inverse micro barn
    Value_w_Err lumi(3.0 * pow(10, 9));

    //Cross-section of B0 in micro barn
    Value_w_Err sigmaB0(38.1);

    //Branching Ratio of B2JpsiKstar and Jpsi2MuMu and Kstar2KPi
    Value_w_Err BR(7.9462 * pow(10, -5), 0.36 * pow(10, -5));

    //Efficiency
    Value_w_Err Eff_punzi(total_eff_punzi, total_error_punzi);
    Value_w_Err Eff_common(total_eff_common, total_error_common);


    //Result
    Value_w_Err Npeaking_punzi = lumi * sigmaB0 * BR *  Eff_punzi;
    Value_w_Err Npeaking_common = lumi * sigmaB0 * BR *  Eff_common;

    cout << "------ Peaking Background Events after Selection -------------" << endl << endl;
    cout << "Common-Cut: N(peaking) = "; Npeaking_common.Print(); cout << endl;
    cout << "Punzi-Cut: N(peaking) = "; Npeaking_punzi.Print(); cout << endl;



   }

