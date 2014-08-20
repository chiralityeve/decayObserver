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

    //Daten (ALLE) nach Anwendung des BDT (resonant und nicht-resonant!)
    TFile* fileData = TFile::Open("/afs/cern.ch/work/d/dberning/private/BDT/Applicationoutput/Bs2mumuf2_BDTselection_FINAL_newInput.root");
    TTree* treeData = dynamic_cast<TTree*>(fileData->Get("Bs2phimumuTuple/DecayTree"));







    //Selectioncuts definieren - Commoncut
    std::string selection = "TMVAResponse > 0.15";


    //härtere Selectionscuts definieren - zweiter Teil der Vorselektion
    std::string selection_hard = "TMVAResponse > 0.27";


    //Alle Cuts zusammen
    std::string allcuts = selection;
    std::string allcuts_hard = selection_hard;

   

  

  

    // ------------------------------------
    // Trees mit angewandten Cuts erstellen
    // ------------------------------------
   
    TFile* tmpfile = new TFile("./tmptree3.root", "RECREATE");
    TTree* treecut = treeData -> CopyTree(allcuts.c_str(), "", 400000);

    TFile* tmpfile2 = new TFile("./tmptree4.root", "RECREATE");
    TTree* treecut_hard = treeData -> CopyTree(allcuts_hard.c_str(), "", 800000);
    
   


    //Variablen Plotten
    TCanvas c("canvas", "dasvancas", 800, 600);
    TCanvas* cp = &c;


    TH1D* hist_f2 = new TH1D("combimass", "Kann verworfen werden", 200, 1400, 4000);
    TH1D* hist_B = new TH1D("combimass1", "Kann verworfen werden", 200, 4000, 5600);

    TH1D* hist_f2_cut = new TH1D("combimass2", "Combined Mass of K and p (Commoncut applied)", 200, 1400, 4000);
    TH1D* hist_B_cut = new TH1D("combimass3", "", 200, 4000, 5600);
    
    TH1D* hist_f2_hardcut = new TH1D("combimass4", "Combined Mass of K and p (Punzicut applied)", 200, 1400, 4000);
    TH1D* hist_B_hardcut = new TH1D("combimass5", "", 200, 4000, 5600);
    
  
    



    // -------------------------------------------------------------------------------------
    // Loop über Tree und Berechnung der neuen B0 und f2-Massen + Histogramme füllen damit (OHNE CUT)
    // -------------------------------------------------------------------------------------



    TLorentzVector K;
    TLorentzVector p;
    TLorentzVector Jpsi;

    double K_px, K_py, K_pz, K_E;
    double p_M, p_px, p_py, p_pz, p_E;
    double Jpsi_px, Jpsi_py, Jpsi_pz, Jpsi_E;

    TBranch* branchK_E = treeData -> GetBranch("Kplus_PE");
    TBranch* branchK_px = treeData -> GetBranch("Kplus_PX");
    TBranch* branchK_py = treeData -> GetBranch("Kplus_PY");
    TBranch* branchK_pz = treeData -> GetBranch("Kplus_PZ");
 
    TBranch* branchp_E = treeData -> GetBranch("Kminus_PE");
    TBranch* branchp_px = treeData -> GetBranch("Kminus_PX");
    TBranch* branchp_py = treeData -> GetBranch("Kminus_PY");
    TBranch* branchp_pz = treeData -> GetBranch("Kminus_PZ");   
    
    TBranch* branchJpsi_E = treeData -> GetBranch("J_psi_1S_PE");
    TBranch* branchJpsi_px = treeData -> GetBranch("J_psi_1S_PX");
    TBranch* branchJpsi_py = treeData -> GetBranch("J_psi_1S_PY");
    TBranch* branchJpsi_pz = treeData -> GetBranch("J_psi_1S_PZ");


    TLorentzVector B;
    TLorentzVector f2;

    Double_t B_M, f2_M;


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
        
    branchJpsi_E -> SetAddress(&Jpsi_E);                                 //pon einfach übernehmen
    branchJpsi_px -> SetAddress(&Jpsi_px);
    branchJpsi_py -> SetAddress(&Jpsi_py);
    branchJpsi_pz -> SetAddress(&Jpsi_pz);


    p_M = 938.272;                                              //Festsetzung der proton-Masse auf nominelle Masse

    for(Int_t i = 0; i < 400000; i++) {

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


        branchJpsi_E -> GetEntry(i);           //Hole Einträge aus Tree für J_Psi
        branchJpsi_px -> GetEntry(i);
        branchJpsi_py -> GetEntry(i);
        branchJpsi_pz -> GetEntry(i);
        Jpsi.SetE(Jpsi_E);
        Jpsi.SetPx(Jpsi_px);
        Jpsi.SetPy(Jpsi_py);
        Jpsi.SetPz(Jpsi_pz);


        f2 = K + p;
        B = K + p + Jpsi;

        f2_M = f2.M();
        B_M = B.M();
               
        hist_f2 -> Fill(f2_M);
        hist_B -> Fill(B_M);
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
    
    branchJpsi_E = treecut -> GetBranch("J_psi_1S_PE");
    branchJpsi_px = treecut -> GetBranch("J_psi_1S_PX");
    branchJpsi_py = treecut -> GetBranch("J_psi_1S_PY");
    branchJpsi_pz = treecut -> GetBranch("J_psi_1S_PZ");





    branchK_E -> SetAddress(&K_E);                                   //Altes K einfach übernehmen
    branchK_px -> SetAddress(&K_px);
    branchK_py -> SetAddress(&K_py);
    branchK_pz -> SetAddress(&K_pz);

    //branchp_E -> SetAddress(&p_E);                              //Misidentifiziertes Kaon wieder als pon darstellen
    branchp_px -> SetAddress(&p_px);
    branchp_py -> SetAddress(&p_py);
    branchp_pz -> SetAddress(&p_pz);
        
    branchJpsi_E -> SetAddress(&Jpsi_E);                                 //pon einfach übernehmen
    branchJpsi_px -> SetAddress(&Jpsi_px);
    branchJpsi_py -> SetAddress(&Jpsi_py);
    branchJpsi_pz -> SetAddress(&Jpsi_pz);



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


        branchJpsi_E -> GetEntry(i);           //Hole Einträge aus Tree für J_Psi
        branchJpsi_px -> GetEntry(i);
        branchJpsi_py -> GetEntry(i);
        branchJpsi_pz -> GetEntry(i);
        Jpsi.SetE(Jpsi_E);
        Jpsi.SetPx(Jpsi_px);
        Jpsi.SetPy(Jpsi_py);
        Jpsi.SetPz(Jpsi_pz);


        f2 = K + p;
        B = K + p + Jpsi;

        f2_M = f2.M();
        B_M = B.M();
               
        hist_f2_cut -> Fill(f2_M);
        hist_B_cut -> Fill(B_M);
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
    
    branchJpsi_E = treecut_hard -> GetBranch("J_psi_1S_PE");
    branchJpsi_px = treecut_hard -> GetBranch("J_psi_1S_PX");
    branchJpsi_py = treecut_hard -> GetBranch("J_psi_1S_PY");
    branchJpsi_pz = treecut_hard -> GetBranch("J_psi_1S_PZ");





    branchK_E -> SetAddress(&K_E);                                   //Altes K einfach übernehmen
    branchK_px -> SetAddress(&K_px);
    branchK_py -> SetAddress(&K_py);
    branchK_pz -> SetAddress(&K_pz);

    //branchp_E -> SetAddress(&p_E);                              //Misidentifiziertes Kaon wieder als pon darstellen
    branchp_px -> SetAddress(&p_px);
    branchp_py -> SetAddress(&p_py);
    branchp_pz -> SetAddress(&p_pz);
        
    branchJpsi_E -> SetAddress(&Jpsi_E);                                 //pon einfach übernehmen
    branchJpsi_px -> SetAddress(&Jpsi_px);
    branchJpsi_py -> SetAddress(&Jpsi_py);
    branchJpsi_pz -> SetAddress(&Jpsi_pz);



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


        branchJpsi_E -> GetEntry(i);           //Hole Einträge aus Tree für J_Psi
        branchJpsi_px -> GetEntry(i);
        branchJpsi_py -> GetEntry(i);
        branchJpsi_pz -> GetEntry(i);
        Jpsi.SetE(Jpsi_E);
        Jpsi.SetPx(Jpsi_px);
        Jpsi.SetPy(Jpsi_py);
        Jpsi.SetPz(Jpsi_pz);


        f2 = K + p;
        B = K + p + Jpsi;

        f2_M = f2.M();
        B_M = B.M();
               
        hist_f2_hardcut -> Fill(f2_M);
        hist_B_hardcut -> Fill(B_M);
    }










    // -----------------------------------
    // Do the Plotting
    // -----------------------------------


    //hist_f2 -> Draw();
    //c.SaveAs("/afs/cern.ch/user/d/dberning/private/Commongit/decayObserver/CMakeHome/plots/Protonhypothese/Ohnecuts.png");
    
    hist_f2_cut -> Draw();
    c.SaveAs("/afs/cern.ch/user/d/dberning/private/Commongit/decayObserver/CMakeHome/plots/Protonhypothese/Commoncut_afterBDT.png");

    
    hist_f2_hardcut -> Draw();
    c.SaveAs("/afs/cern.ch/user/d/dberning/private/Commongit/decayObserver/CMakeHome/plots/Protonhypothese/Punzicut_afterBDT.png");
   




        





        return 0;
    }

