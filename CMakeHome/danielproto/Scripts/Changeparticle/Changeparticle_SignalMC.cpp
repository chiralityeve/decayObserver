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

#include <TLorentzVector.h>



using std::cout;
using std::endl;

// -----------------------------------
// Vergleiche verschiedene Selektionscuts im Hinblick auf die Untergrundunterdrückung (vor allem Peaking-Background)
// -----------------------------------




int main() {
    //Vergleiche Daten nach verschiedenen Cuts

    gStyle->SetOptStat(0);

    //Daten (ALLE)
    TFile* fileData = TFile::Open("/afs/cern.ch/work/d/dberning/private/Pruned/MC_Truthmatched/MC_merged_pruned_truthmatched.root");
    TTree* treeData = dynamic_cast<TTree*>(fileData->Get("Bs2phimumuTuple/DecayTree"));







    //Selectioncuts definieren - erster Teil der Vorselektion
    std::string selection = "Kplus_PIDK > -3 && Kminus_PIDK > -3";


    //härtere Selectionscuts definieren - zweiter Teil der Vorselektion
    std::string selection_hard = "Kplus_PIDK > -3 && Kminus_PIDK > -3 && phi_1020_M > 1300 && phi_1020_M < 1800";


    //Alle Cuts zusammen
    std::string allcuts = selection;
    std::string allcuts_hard = selection_hard;

   

  

  

    // ------------------------------------
    // Trees mit angewandten Cuts erstellen
    // ------------------------------------
   
    TFile* tmpfile = new TFile("./tmptree_MC.root", "RECREATE");
    TTree* treecut = treeData -> CopyTree(allcuts.c_str());

    TFile* tmpfile2 = new TFile("./tmptree2_MC.root", "RECREATE");
    TTree* treecut_hard = treeData -> CopyTree(allcuts_hard.c_str());

    
    
   


    //Variablen Plotten
    TCanvas c("canvas", "dasvancas", 800, 600);
    TCanvas* cp = &c;


    TH1D* hist_f2 = new TH1D("combimass", "SignalMC: Kombinierte Masse von K und p", 200, 1400, 4000);
    

    TH1D* hist_f2_cut = new TH1D("combimass2", "SignalMC: Kombinierte Masse von K und p (cut kaon_PIDK > -3 applied)", 200, 1400, 4000);
   
    
    TH1D* hist_f2_hardcut = new TH1D("combimass4", "SignalMC: Kombinierte Masse von K und p (kaon_PIDK and cut on f2-mass applied)", 200, 1400, 4000);
   
    
  
    



    // -------------------------------------------------------------------------------------
    // Loop über Tree und Berechnung der neuen B0 und f2-Massen + Histogramme füllen damit (OHNE CUT)
    // -------------------------------------------------------------------------------------



    TLorentzVector K;
    TLorentzVector p;
    

    double K_px, K_py, K_pz, K_E;
    double p_M, p_px, p_py, p_pz, p_E;
    

    TBranch* branchK_E = treeData -> GetBranch("Kplus_PE");
    TBranch* branchK_px = treeData -> GetBranch("Kplus_PX");
    TBranch* branchK_py = treeData -> GetBranch("Kplus_PY");
    TBranch* branchK_pz = treeData -> GetBranch("Kplus_PZ");
 
    TBranch* branchp_E = treeData -> GetBranch("Kminus_PE");
    TBranch* branchp_px = treeData -> GetBranch("Kminus_PX");
    TBranch* branchp_py = treeData -> GetBranch("Kminus_PY");
    TBranch* branchp_pz = treeData -> GetBranch("Kminus_PZ");   
     
    TLorentzVector f2;

    Double_t f2_M;


    //Ohne angewandte Selektion

    double nentries_all = treeData -> GetEntries();

    branchK_E -> SetAddress(&K_E);                                   //Altes K einfach übernehmen
    branchK_px -> SetAddress(&K_px);
    branchK_py -> SetAddress(&K_py);
    branchK_pz -> SetAddress(&K_pz);

    //branchp_E -> SetAddress(&p_E);       //Misidentifiziertes Kaon wieder als pon darstellen
    branchp_px -> SetAddress(&p_px);
    branchp_py -> SetAddress(&p_py);
    branchp_pz -> SetAddress(&p_pz);
        
 
    p_M = 938.272;                                              //Festsetzung der proton-Masse auf nominelle Masse

    for(Int_t i = 0; i < nentries_all; i++) {

        branchK_E-> GetEntry(i);           //Hole Einträge aus Tree für Kaon
        branchK_px -> GetEntry(i);
        branchK_py -> GetEntry(i);
        branchK_pz -> GetEntry(i);
        K.SetE(K_E);
        K.SetPx(K_px);
        K.SetPy(K_py);
        K.SetPz(K_pz);
         

        //treeData -> GetBranch("Kplus_M") -> GetEntry(i);           //Hole Einträge aus Tree für pon
        branchp_px -> GetEntry(i);
        branchp_py -> GetEntry(i);
        branchp_pz -> GetEntry(i);
        p.SetPx(p_px);
        p.SetPy(p_py);
        p.SetPz(p_pz);
        p_E = sqrt( pow(p_px, 2) + pow(p_py, 2) + pow(p_pz, 2) + pow(p_M, 2) );
        p.SetE(p_E);


     

        f2 = K + p;
        

        f2_M = f2.M();
        
               
        hist_f2 -> Fill(f2_M);
        
    }

   



    // -----------------------------------
    // Cutted Tree (nur mit Kaon DLLK > -3)
    // -----------------------------------

    branchK_E = treecut -> GetBranch("Kplus_PE");
    branchK_px = treecut -> GetBranch("Kplus_PX");
    branchK_py = treecut -> GetBranch("Kplus_PY");
    branchK_pz = treecut -> GetBranch("Kplus_PZ");
 
    branchp_E = treecut -> GetBranch("Kminus_PE");
    branchp_px = treecut -> GetBranch("Kminus_PX");
    branchp_py = treecut -> GetBranch("Kminus_PY");
    branchp_pz = treecut -> GetBranch("Kminus_PZ");   




    branchK_E -> SetAddress(&K_E);                                   //Altes K einfach übernehmen
    branchK_px -> SetAddress(&K_px);
    branchK_py -> SetAddress(&K_py);
    branchK_pz -> SetAddress(&K_pz);

    //branchp_E -> SetAddress(&p_E);                              //Misidentifiziertes Kaon wieder als pon darstellen
    branchp_px -> SetAddress(&p_px);
    branchp_py -> SetAddress(&p_py);
    branchp_pz -> SetAddress(&p_pz);
        


    for(Int_t i = 0; i < branchK_E -> GetEntries(); i++) {

        branchK_E-> GetEntry(i);           //Hole Einträge aus Tree für Kaon
        branchK_px -> GetEntry(i);
        branchK_py -> GetEntry(i);
        branchK_pz -> GetEntry(i);
        K.SetE(K_E);
        K.SetPx(K_px);
        K.SetPy(K_py);
        K.SetPz(K_pz);
         

        //treeData -> GetBranch("Kplus_M") -> GetEntry(i);           //Hole Einträge aus Tree für pon
        branchp_px -> GetEntry(i);
        branchp_py -> GetEntry(i);
        branchp_pz -> GetEntry(i);
        p.SetPx(p_px);
        p.SetPy(p_py);
        p.SetPz(p_pz);
        p_E = sqrt( pow(p_px, 2) + pow(p_py, 2) + pow(p_pz, 2) + pow(p_M, 2) );
        p.SetE(p_E);


     

        f2 = K + p;
        

        f2_M = f2.M();
        
               
        hist_f2_cut -> Fill(f2_M);
        
    }





    // -----------------------------------
    // Cutted Tree (komplette Vorselektion)
    // -----------------------------------

    branchK_E = treecut_hard -> GetBranch("Kplus_PE");
    branchK_px = treecut_hard -> GetBranch("Kplus_PX");
    branchK_py = treecut_hard -> GetBranch("Kplus_PY");
    branchK_pz = treecut_hard -> GetBranch("Kplus_PZ");
 
    branchp_E = treecut_hard -> GetBranch("Kminus_PE");
    branchp_px = treecut_hard -> GetBranch("Kminus_PX");
    branchp_py = treecut_hard -> GetBranch("Kminus_PY");
    branchp_pz = treecut_hard -> GetBranch("Kminus_PZ");   
    
  




    branchK_E -> SetAddress(&K_E);                                   //Altes K einfach übernehmen
    branchK_px -> SetAddress(&K_px);
    branchK_py -> SetAddress(&K_py);
    branchK_pz -> SetAddress(&K_pz);

    //branchp_E -> SetAddress(&p_E);                              //Misidentifiziertes Kaon wieder als pon darstellen
    branchp_px -> SetAddress(&p_px);
    branchp_py -> SetAddress(&p_py);
    branchp_pz -> SetAddress(&p_pz);
        
   

    for(Int_t i = 0; i < branchK_E -> GetEntries(); i++) {

        branchK_E-> GetEntry(i);           //Hole Einträge aus Tree für Kaon
        branchK_px -> GetEntry(i);
        branchK_py -> GetEntry(i);
        branchK_pz -> GetEntry(i);
        K.SetE(K_E);
        K.SetPx(K_px);
        K.SetPy(K_py);
        K.SetPz(K_pz);
         

        //treeData -> GetBranch("Kplus_M") -> GetEntry(i);           //Hole Einträge aus Tree für pon
        branchp_px -> GetEntry(i);
        branchp_py -> GetEntry(i);
        branchp_pz -> GetEntry(i);
        p.SetPx(p_px);
        p.SetPy(p_py);
        p.SetPz(p_pz);
        p_E = sqrt( pow(p_px, 2) + pow(p_py, 2) + pow(p_pz, 2) + pow(p_M, 2) );
        p.SetE(p_E);



        f2 = K + p;
    

        f2_M = f2.M();
    
               
        hist_f2_hardcut -> Fill(f2_M);
     
    }










    // -----------------------------------
    // Do the Plotting
    // -----------------------------------


    hist_f2 -> Draw();
    c.SaveAs("/afs/cern.ch/user/d/dberning/private/Commongit/decayObserver/CMakeHome/plots/Protonhypothese/SignalMC_Ohnecuts.png");
    
    hist_f2_cut -> Draw();
    c.SaveAs("/afs/cern.ch/user/d/dberning/private/Commongit/decayObserver/CMakeHome/plots/Protonhypothese/SignalMC_Nur_Kaon_DLLK.png");

    
    hist_f2_hardcut -> Draw();
    c.SaveAs("/afs/cern.ch/user/d/dberning/private/Commongit/decayObserver/CMakeHome/plots/Protonhypothese/SignalMC_Komplette_Vorselektion.png");
   




        





        return 0;
    }

