#include <iostream>
#include <string>
#include "math.h"
//#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
//#include <TH1D.h>
//#include <TPaveStats.h>
//#include <TGaxis.h>
#include <TChain.h>

//#include "tree2hist.h"

//using std::cout;
//using std::endl;

int main() {

    //Beide Polaritäten in TChain einlesen
    TChain ch("DecayTree");
    //ch.Add("/afs/cern.ch/work/n/nfomin/public/bs2jpsiphi/2011MagDown/bs2jpsiphifinal.root");
    //ch.Add("/afs/cern.ch/work/n/nfomin/public/bs2jpsiphi/2011MagUp/bs2jpsiphifinal.root");
    //ch.Add("/afs/cern.ch/work/n/nfomin/public/bs2jpsiphi/2012MagUp/bs2jpsiphifinal.root");
    //ch.Add("/afs/cern.ch/work/n/nfomin/public/bs2jpsiphi/2012MagDown/bs2jpsiphifinal.root");
    
    //ch.Add("/afs/cern.ch/work/d/dberning/private/Data_resonant_preselected/Bsf2mumu_Data_merged.root");

    ch.Add("/afs/cern.ch/work/d/dberning/private/BDT/Non_resonant_Background.root");

    TChain* chp = &ch;


    //Branches deaktivieren (optional)
    /*treeold->SetBranchStatus("Kaon_TRUE*",0);
      treeold->SetBranchStatus("mu2_TRUE*",0);
      treeold->SetBranchStatus("mu1_TRUE*",0);
      treeold->SetBranchStatus("J_psi_1S_TRUE*",0);
      treeold->SetBranchStatus("Bplus_TRUE*",0);*/




    // open new Tree
    TFile filenew("/afs/cern.ch/work/d/dberning/private/BDT/Non_resonant_Background_preselected.root", "RECREATE");

    /* 
    //Führe TIS-Cuts aus
    std::string cuts = "(Bplus_L0MuonDecision_TIS == 1 || Bplus_L0DiMuonDecision_TIS == 1) && ";
    cuts += "(Bplus_Hlt1TrackAllL0Decision_TIS == 1 || Bplus_Hlt1TrackMuonDecision_TIS == 1) && ";
    cuts += "(Bplus_Hlt2Topo2BodyBBDTDecision_TIS == 1 || Bplus_Hlt2Topo3BodyBBDTDecision_TIS == 1 || Bplus_Hlt2Topo4BodyBBDTDecision_TIS == 1 || ";
    cuts += "Bplus_Hlt2TopoMu2BodyBBDTDecision_TIS == 1 || Bplus_Hlt2TopoMu3BodyBBDTDecision_TIS == 1 || Bplus_Hlt2TopoMu4BodyBBDTDecision_TIS == 1 || ";
    cuts += "Bplus_Hlt2SingleMuonDecision_TIS == 1 || Bplus_Hlt2DiMuonDetachedDecision_TIS == 1)";


    //Führe TOS-Cuts aus
    std::string cuts = "(Bplus_L0MuonDecision_TOS == 1 || Bplus_L0DiMuonDecision_TOS == 1) && ";
    cuts += "(Bplus_Hlt1TrackAllL0Decision_TOS == 1 || Bplus_Hlt1TrackMuonDecision_TOS == 1) && ";
    cuts += "(Bplus_Hlt2Topo2BodyBBDTDecision_TOS == 1 || Bplus_Hlt2Topo3BodyBBDTDecision_TOS == 1 || Bplus_Hlt2Topo4BodyBBDTDecision_TOS == 1 || ";
    cuts += "Bplus_Hlt2TopoMu2BodyBBDTDecision_TOS == 1 || Bplus_Hlt2TopoMu3BodyBBDTDecision_TOS == 1 || Bplus_Hlt2TopoMu4BodyBBDTDecision_TOS == 1 || ";
    cuts += "Bplus_Hlt2SingleMuonDecision_TOS == 1 || Bplus_Hlt2DiMuonDetachedDecision_TOS == 1)";
    */

    //Vorselektion
    //std::string cuts = "B0_M > 5500 && (J_psi_1S_M > 3316.6 || J_psi_1S_M < 2828.4) && (J_psi_1S_M < 3535.6 || J_psi_1S_M > 3873.0) && (phi_1020_M > 1300 && phi_1020_M < 1800)";
    std::string  cuts = "Kplus_PIDK > -3 && Kminus_PIDK > -3";


    //Kopiervorgang ausführen
    TTree* tnew = chp->CopyTree(cuts.c_str());

    tnew->Write();

 

    delete tnew;
    //delete ttemp;

    return 0;
}

