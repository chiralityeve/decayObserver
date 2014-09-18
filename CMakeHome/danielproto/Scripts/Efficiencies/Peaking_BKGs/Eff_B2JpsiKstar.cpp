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
    TFile* file = TFile::Open("/afs/cern.ch/work/d/dberning/private/BDT_w_bdtana/Peaking_BKG_MCs/B2JpsiKstar_MC_pruned_TMVAResponse.root");
    if (file == 0) {
        // if we cannot open the file, print an error message and return immediatly
        // printf("Error: cannot open
        //       http://lcg-heppkg.web.cern.ch/lcg-heppkg/ROOT/eventdata.root!\n");
        return 1;
    }

    // get a pointer to the truthmatched Tree
    TTree *tree = dynamic_cast<TTree*>(file->Get("Bs2phimumuTuple/DecayTree"));



    //Full MC (including MCDecayTree)
    TFile* file_generator = TFile::Open("/afs/cern.ch/work/d/dberning/private/Pruned/Peaking_BKG_MCs/B2JpsiKstar_MC_pruned.root");
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
    TFile* file_BDT = TFile::Open("/afs/cern.ch/work/d/dberning/private/BDT_w_bdtana/Peaking_BKG_MCs/B2JpsiKstar_MC_pruned_TMVAResponse.root");
    if (file_BDT == 0) {
        // if we cannot open the file, print an error message and return immediatly
        // printf("Error: cannot open
        //       http://lcg-heppkg.web.cern.ch/lcg-heppkg/ROOT/eventdata.root!\n");
        return 1;
    }

    // get a pointer to the truthmatched Tree
    TTree *tree_BDT = dynamic_cast<TTree*>(file_BDT->Get("Bs2phimumuTuple/DecayTree"));


    //cout << std::fixed;
    
    //-------------------------------------------------------
    // Define generator level efficiency and cuts -----------
    //-------------------------------------------------------

    Value_w_Err generator_eff(0.16049, 0.00043);

    std::string truthmatching = "1";

    std::string preselection = "phi_1020_M < 1800 && phi_1020_M  > 1300 && Kplus_PIDK > -3 && Kminus_PIDK > -3";
    preselection += " && (Kplus_PIDK - Kplus_PIDp) > -3 && (Kminus_PIDK - Kminus_PIDp) > -3";
    //preselection += " && J_psi_1S_M > 3047 && J_psi_1S_M < 3147";     //Only Jpsi candidates
    preselection += " && (J_psi_1S_M < 2828.4 || J_psi_1S_M > 3316.6) && (J_psi_1S_M < 3535.6 || J_psi_1S_M > 3873.0)";
    preselection += " && B0_M > 5150 && B0_M < 5700";       //Bs Fit window

    std::string punzi = preselection + " && TMVAResponse > 0.244909";
    std::string common = preselection + " && TMVAResponse > 0.123853";
    
    std::string trigger =  "(B0_L0MuonDecision_TOS == 1 || B0_L0DiMuonDecision_TOS == 1)";
    trigger += " && (B0_Hlt1TrackAllL0Decision_TOS == 1 || B0_Hlt1TrackMuonDecision_TOS == 1 || B0_Hlt1DiMuonLowMassDecision_TOS == 1 || B0_Hlt1DiMuonHighMassDecision_TOS == 1 || B0_Hlt1SingleMuonHighPTDecision_TOS == 1)";
    trigger += " && (B0_Hlt2SingleMuonDecision_TOS == 1 || B0_Hlt2DiMuonDetachedDecision_TOS == 1 || B0_Hlt2DiMuonDetachedHeavyDecision_TOS == 1 || B0_Hlt2Topo2BodyBBDTDecision_TOS == 1 || B0_Hlt2Topo3BodyBBDTDecision_TOS == 1 || B0_Hlt2Topo4BodyBBDTDecision_TOS == 1 || B0_Hlt2TopoMu2BodyBBDTDecision_TOS == 1 || B0_Hlt2TopoMu3BodyBBDTDecision_TOS == 1 || B0_Hlt2TopoMu4BodyBBDTDecision_TOS == 1)";

    std::string punzi_trigger = punzi + " && " + trigger;
    std::string common_trigger = common + " && " + trigger;




    //----------------------------------
    // Generator level cut efficiency
    // ---------------------------------
 
    cout << endl << "---------------------------------------------";
    cout << endl << "-----------------B2JpsiKstar-----------------";
    cout << endl << "---------------------------------------------" << endl;


    cout << endl << "------ Generator level Cut Efficiency -------" << endl << endl;
    generator_eff.Print(); cout  << endl << endl;



    

    // ----------------------------------
    // Stripping-Effizienz berechnen
    // ----------------------------------
   
    

    double n_generated_events = tree_gen -> GetEntries();
    double n_after_stripping = tree_gen_after_stripping -> GetEntries();

    
    double n_truthmatched = tree_gen_after_stripping -> GetEntries(truthmatching.c_str());
    double strippingerr = getEffError(n_generated_events, n_truthmatched);

    Value_w_Err stripping_eff( n_truthmatched / n_generated_events, strippingerr );


    cout << "------ Stripping Efficiency -------" << endl << endl;
    cout << "Generated Events:  " << n_generated_events << endl;
    cout << "Events after Stripping (not-truthmatched): " << n_after_stripping << endl; 
    cout << "Events after Stripping (truthmatched): " << n_truthmatched << endl << endl;
    cout << "=> Effizienz: "; stripping_eff.Print(); cout  << endl << endl;





    // ---------------------------------
    // Preselection-Effizienz berechnen
    // ---------------------------------
    


    double n_events = tree -> GetEntries();
    double n_presel = tree -> GetEntries(preselection.c_str());
    double efferror = getEffError(n_events, n_presel);

    Value_w_Err presel_eff( n_presel / n_events, efferror);

  
    cout << "------ Preselectionefficiency (Veto on Jpsi and Psi(2S) and cut on Bs-Mass) -------" << endl << endl;
    cout << "Events before Cuts (Truthmatched):  " << n_events << endl;
    cout << "Events after Preselection: " << n_presel << endl << endl;
    cout << "=> Effizienz: "; presel_eff.Print(); cout << endl << endl;




    // ---------------------------------
    // BDT Effizienz berechnen
    // ---------------------------------
    
    
    double n_before_BDT = tree_BDT -> GetEntries(preselection.c_str());
    double n_after_BDT_punzi = tree_BDT -> GetEntries(punzi.c_str());
    double n_after_BDT_common = tree_BDT -> GetEntries(common.c_str());

    Value_w_Err punzi_eff( n_after_BDT_punzi / n_before_BDT, getEffError(n_before_BDT, n_after_BDT_punzi) );
    Value_w_Err common_eff( n_after_BDT_common / n_before_BDT, getEffError(n_before_BDT, n_after_BDT_common) );


    cout << "------ BDT Efficiency --------" << endl << endl;
    cout << "Events before BDT-cut (preselected, truthmatched):  " << n_before_BDT << endl << endl;
    
    cout << "Events after Common-cut: " << n_after_BDT_common << "\t\tEffizienz: "; common_eff.Print(); cout << endl;
   
    cout << "Events after Punzi-cut: " << n_after_BDT_punzi << "\t\tEffizienz: "; punzi_eff.Print(); cout << endl << endl;
   




    // ----------------------------------
    // Trigger Effizienz berechnen
    // ----------------------------------
    


    double n_after_trigger_punzi = tree_BDT -> GetEntries(punzi_trigger.c_str());
    double n_after_trigger_common = tree_BDT -> GetEntries(common_trigger.c_str());

    Value_w_Err trigger_punzi_eff( n_after_trigger_punzi / n_after_BDT_punzi, getEffError(n_after_BDT_punzi, n_after_trigger_punzi));
    Value_w_Err trigger_common_eff( n_after_trigger_common / n_after_BDT_common, getEffError(n_after_BDT_common, n_after_trigger_common));
   
    cout << "------ Trigger Efficiency --------" << endl << endl;
    cout << "Events after BDT Common-cut (preselected, truthmatched):  " << n_after_BDT_common << endl;
    cout << "Events after BDT Punzi-cut (preselected, truthmatched):  " << n_after_BDT_punzi << endl << endl;
    
    cout << "Events after Common-cut and Triggerrequ.: " << n_after_trigger_common << "\t\tEffizienz: "; trigger_common_eff.Print(); cout << endl;
   
    cout << "Events after Punzi-cut and Triggerrequ: " << n_after_trigger_punzi << "\t\tEffizienz: "; trigger_punzi_eff.Print(); cout << endl << endl;



    // -------------------------
    // Totale Effizienz 
    // -------------------------

    //Error of one factor (without generator level cut)
    Value_w_Err total_common_eff_wo_gencut( n_after_trigger_common / n_generated_events, getEffError(n_generated_events, n_after_trigger_common));
    
    Value_w_Err total_punzi_eff_wo_gencut( n_after_trigger_punzi / n_generated_events, getEffError(n_generated_events, n_after_trigger_punzi));
    


    //Error altogether with gencut as well
    Value_w_Err total_common_eff = total_common_eff_wo_gencut * generator_eff;
    Value_w_Err total_punzi_eff = total_punzi_eff_wo_gencut * generator_eff;

    //in percent
    Value_w_Err total_common_eff_percent = total_common_eff * 100;
    Value_w_Err total_punzi_eff_percent = total_punzi_eff * 100;
    
    cout << "------ Total Efficiency --------" << endl << endl;
    cout << "With Common-Cut: ("; total_common_eff_percent.Print(); cout << ") %" << endl;
    cout << "With Punzi-Cut: ("; total_punzi_eff_percent.Print(); cout << ") %"  << endl << endl << endl << endl;








    // ---------------------------------------------------------------
    // Verrechnen mit Kontrollkanal - ab hier nur noch mit Punzi..
    // ---------------------------------------------------------------

    cout << "------ Peaking Background Events after Selection (using controlchannel)-------------" << endl << endl; 



    //Resonant (control) decay------------------------
    //Number resonant Decays
    Value_w_Err Nres(1892.17, 80.47);

    cout << "Resonant decays after selection: "; Nres.Print(); cout << endl;

    //BR(res)
    Value_w_Err Bres1(2.6 * pow(10, -4), 0.6 * pow(10, -4));    //Bs -> f2 Jpsi NOTE: eigentlich asymmetrischer Fehler +0.9 , -0.6 ....
    Value_w_Err Bres2(5.961 * pow(10, -2), 0.033 * pow(10, -2));  //Jpsi -> mu mu
    Value_w_Err Bres3(44.35 * pow(10, -2), 1.1 * pow(10,-2));   //f2 -> K+ K-
    Value_w_Err Bres = Bres1 * Bres2 * Bres3;

    cout << "Resonant BR: "; Bres.Print(); cout  << "\tNOTE: Symmetric error for Bs -> f2 Jpsi assumed" << endl;

    //Efficiency res. Decay
    Value_w_Err Res_punzi_eff(0.4258 * pow(10, -2), 0.0186 * pow(10, -2));
    cout << "Resonant Efficiency (punzi-cut): "; Res_punzi_eff.Print(); cout  << endl << endl;

    //This decay----------------------------------
    //BR(this)
    Value_w_Err Bthis1(1.32 * pow(10, -3), 0.06 * pow(10, -3));      //B -> Jpsi K*
    Value_w_Err Bthis2(5.961 * pow(10, -2), 0.033 * pow(10, -2));   //Jpsi -> Mu Mu
    Value_w_Err Bthis3(0.5, 0);                                     //K* -> K+- pi+-
    Value_w_Err Bthis =  Bthis1 * Bthis2 * Bthis3;

    cout << "BR of this decay: "; Bthis.Print(); cout  << endl;



    //Efficiency calculated above
    
    //Ratio of production fractions
    Value_w_Err fsfd(0.259, 0.015);     //Value of fs/fd
    Value_w_Err one(1.0);
    Value_w_Err fdfs = one / fsfd;

    cout << "fd/fs: "; fdfs.Print(); cout << endl << endl;

    //Result
    Value_w_Err Npeaking_punzi = Nres / (Bres * Res_punzi_eff) * fdfs * Bthis * total_punzi_eff;
    

    
    cout << "Punzi-Cut: N(peaking) = "; Npeaking_punzi.Print(); cout << endl << endl;






}
