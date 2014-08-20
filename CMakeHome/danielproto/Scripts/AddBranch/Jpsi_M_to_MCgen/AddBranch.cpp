#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TMath.h"
#include "TLorentzVector.h"

int main(){
    
    TFile fin("/afs/cern.ch/work/d/dberning/private/RawData/Bsf2mumu_MC_Full.root", "READ");
    if(!fin.IsOpen()){
        std::cerr << "Coud not open inputfile!" << std::endl;
        return 0;
    }

    //Open old trees and get entries
    TTree *oldtree = (TTree*)fin.Get("MCBs2phimumuTuple/MCDecayTree");
    TTree *oldlumi = (TTree*)fin.Get("Bs2phimumuTuple/DecayTree");
   
    TDirectory* Diroldtree = &fin;
    Diroldtree = Diroldtree -> GetDirectory("MCBs2phimumuTuple");

    TDirectory* Diroldlumi = &fin;
    Diroldlumi = Diroldlumi -> GetDirectory("Bs2phimumuTuple");
    



    Long64_t oldtree_entries = oldtree->GetEntries();
   
    
    TFile fout("/afs/cern.ch/work/d/dberning/private/RawData/Bsf2mumu_MC_Full_MCJpsi_M.root", "RECREATE");
    
    if(!fout.IsOpen()){
        std::cerr << "Coud not create ouputfile!" << std::endl;
        return 0;
    }
    std::cout << "files open -> begin calculation" << std::endl;


    TDirectory* Dirtree = &fout;
    Dirtree = Dirtree -> mkdir(Diroldtree->GetName(), Diroldtree -> GetTitle());

    TDirectory* Dirlumi = &fout;
    Dirlumi = Dirlumi -> mkdir(Diroldlumi->GetName(), Diroldlumi->GetTitle());
   



    


   
    //Copy Trees without Entries first
    
    Dirlumi -> cd();
    TTree *lumi = oldlumi->CloneTree(-1, "fast"); //lumitree can just be copied - no modification applied
    lumi->AutoSave();
   

    Dirtree -> cd();
    TTree *tree = oldtree->CloneTree(0);
    
    //Create new variables (masses)
    Double_t Jpsi_1S_M;



    //Create new Branches
    TBranch *newB_Jpsi_1S_M = tree->Branch("J_psi_1S_M",&Jpsi_1S_M,"J_psi_1S_M/D");
    

    //Get Branches of old Tree
    TBranch* B_muplus_PE = oldtree->GetBranch("muplus_TRUEP_E");
    TBranch* B_muplus_PX = oldtree->GetBranch("muplus_TRUEP_X");
    TBranch* B_muplus_PY = oldtree->GetBranch("muplus_TRUEP_Y");
    TBranch* B_muplus_PZ = oldtree->GetBranch("muplus_TRUEP_Z");
    TBranch* B_muminus_PE = oldtree->GetBranch("muminus_TRUEP_E");
    TBranch* B_muminus_PX = oldtree->GetBranch("muminus_TRUEP_X");
    TBranch* B_muminus_PY = oldtree->GetBranch("muminus_TRUEP_Y");
    TBranch* B_muminus_PZ = oldtree->GetBranch("muminus_TRUEP_Z");
   
    TBranch* newB_muplus_PE = tree->GetBranch("muplus_TRUEP_E");
    TBranch* newB_muplus_PX = tree->GetBranch("muplus_TRUEP_X");
    TBranch* newB_muplus_PY = tree->GetBranch("muplus_TRUEP_Y");
    TBranch* newB_muplus_PZ = tree->GetBranch("muplus_TRUEP_Z");
    TBranch* newB_muminus_PE = tree->GetBranch("muminus_TRUEP_E");
    TBranch* newB_muminus_PX = tree->GetBranch("muminus_TRUEP_X");
    TBranch* newB_muminus_PY = tree->GetBranch("muminus_TRUEP_Y");
    TBranch* newB_muminus_PZ = tree->GetBranch("muminus_TRUEP_Z");


    //Create variables for kinematics AND Lorentzvectors
    Double_t muplus_PE, muplus_PX, muplus_PY, muplus_PZ;
    Double_t muminus_PE, muminus_PX, muminus_PY, muminus_PZ;

   
    TLorentzVector Vec_muplus, Vec_muminus;
    TLorentzVector Vec_Jpsi;        //temporary Lorentzvectors which will be modified with nominal masses
    
   

    //Set Addresses to just created Variables
    B_muplus_PE -> SetAddress(&muplus_PE);
    B_muplus_PX -> SetAddress(&muplus_PX);
    B_muplus_PY -> SetAddress(&muplus_PY);
    B_muplus_PZ -> SetAddress(&muplus_PZ);

    B_muminus_PE -> SetAddress(&muminus_PE);
    B_muminus_PX -> SetAddress(&muminus_PX);
    B_muminus_PY -> SetAddress(&muminus_PY);
    B_muminus_PZ -> SetAddress(&muminus_PZ);

    newB_muplus_PE -> SetAddress(&muplus_PE);
    newB_muplus_PX -> SetAddress(&muplus_PX);
    newB_muplus_PY -> SetAddress(&muplus_PY);
    newB_muplus_PZ -> SetAddress(&muplus_PZ);

    newB_muminus_PE -> SetAddress(&muminus_PE);
    newB_muminus_PX -> SetAddress(&muminus_PX);
    newB_muminus_PY -> SetAddress(&muminus_PY);
    newB_muminus_PZ -> SetAddress(&muminus_PZ);


    std::cout << "found " << oldtree_entries << " entries" << std::endl;


    //Loop over oldtree now and fill newtree
    for (Long64_t i = 0; i < oldtree_entries; i++){
        //Get variablevalues of entry i
        oldtree -> GetEntry(i);

        //Initialise Lorentzvectors muplus and muminus
        Vec_muplus.SetE(muplus_PE);
        Vec_muplus.SetPx(muplus_PX);
        Vec_muplus.SetPy(muplus_PY);
        Vec_muplus.SetPz(muplus_PZ);

        Vec_muminus.SetE(muminus_PE);
        Vec_muminus.SetPx(muminus_PX);
        Vec_muminus.SetPy(muminus_PY);
        Vec_muminus.SetPz(muminus_PZ);



        Vec_Jpsi = Vec_muplus + Vec_muminus;
        
        Jpsi_1S_M = Vec_Jpsi.M();
        
        





        //Fill (new) Tree
        tree -> Fill();

    }
    tree -> AutoSave();

    
    return 0;
}
