#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TMath.h"
#include "TLorentzVector.h"

int main(){
    
    TFile fin("/afs/cern.ch/work/d/dberning/private/Peaking_BKG_MCs/B2LambdaMuMu_MC.root", "READ");
    if(!fin.IsOpen()){
        std::cerr << "Coud not open inputfile!" << std::endl;
        return 0;
    }

    //Open old trees and get entries
    TTree *oldtree = (TTree*)fin.Get("Bs2phimumuTuple/DecayTree");
    TTree *oldlumi = (TTree*)fin.Get("MCBs2phimumuTuple/MCDecayTree");
   
    TDirectory* Diroldtree = &fin;
    Diroldtree = Diroldtree -> GetDirectory("Bs2phimumuTuple");

    TDirectory* Diroldlumi = &fin;
    Diroldlumi = Diroldlumi -> GetDirectory("MCBs2phimumuTuple");
    



    Long64_t oldtree_entries = oldtree->GetEntries();
   
    
    TFile fout("/afs/cern.ch/work/d/dberning/private/Peaking_BKG_MCs/B2LambdaMuMu_MC_newVars.root", "RECREATE");
    
    if(!fout.IsOpen()){
        std::cerr << "Coud not create ouputfile!" << std::endl;
        return 0;
    }
    std::cout << "files open -> begin calculation" << std::endl;


    TDirectory* Dirtree = &fout;
    Dirtree = Dirtree -> mkdir(Diroldtree->GetName(), Diroldtree -> GetTitle());

    TDirectory* Dirlumi = &fout;
    Dirlumi = Dirlumi -> mkdir(Diroldlumi->GetName(), Diroldlumi->GetTitle());
   



    


    //Create variables with nominal masses of used particles
    Double_t nominal_Proton_M = 938.272046;
    Double_t nominal_Pion_M = 139.57018;     //charged Pion

    //Copy Trees without Entries first
    
    Dirlumi -> cd();
    TTree *lumi = oldlumi->CloneTree(-1, "fast"); //lumitree can just be copied - no modification applied
    lumi->AutoSave();
   

    Dirtree -> cd();
    TTree *tree = oldtree->CloneTree(0);
    
    //Create new variables (masses)
    Double_t Kplus_Proton_M;
    Double_t Kminus_Proton_M;
    Double_t Kplus_Pion_M;
    Double_t Kminus_Pion_M;
    Double_t Proton_Pion_M;
    Double_t Pion_Proton_M;


    //Create new Branches
    TBranch *newB_Kplus_Proton_M = tree->Branch("Kplus_Proton_M",&Kplus_Proton_M,"Kplus_Proton_M/D");
    TBranch *newB_Kminus_Proton_M = tree->Branch("Kminus_Proton_M",&Kminus_Proton_M,"Kminus_Proton_M/D");
    TBranch *newB_Kplus_Pion_M = tree->Branch("Kplus_Pion_M",&Kplus_Pion_M,"Kplus_Pion_M/D");
    TBranch *newB_Kminus_Pion_M = tree->Branch("Kminus_Pion_M",&Kminus_Pion_M,"Kminus_Pion_M/D");
    TBranch *newB_Proton_Pion_M = tree->Branch("Proton_Pion_M",&Proton_Pion_M,"Proton_Pion_M/D");
    TBranch *newB_Pion_Proton_M = tree->Branch("Pion_Proton_M",&Pion_Proton_M,"Pion_Proton_M/D");


    //Get Branches of old Tree
    TBranch* B_Kplus_PE = oldtree->GetBranch("Kplus_PE");
    TBranch* B_Kplus_PX = oldtree->GetBranch("Kplus_PX");
    TBranch* B_Kplus_PY = oldtree->GetBranch("Kplus_PY");
    TBranch* B_Kplus_PZ = oldtree->GetBranch("Kplus_PZ");
    TBranch* B_Kminus_PE = oldtree->GetBranch("Kminus_PE");
    TBranch* B_Kminus_PX = oldtree->GetBranch("Kminus_PX");
    TBranch* B_Kminus_PY = oldtree->GetBranch("Kminus_PY");
    TBranch* B_Kminus_PZ = oldtree->GetBranch("Kminus_PZ");
   
    TBranch* newB_Kplus_PE = tree->GetBranch("Kplus_PE");
    TBranch* newB_Kplus_PX = tree->GetBranch("Kplus_PX");
    TBranch* newB_Kplus_PY = tree->GetBranch("Kplus_PY");
    TBranch* newB_Kplus_PZ = tree->GetBranch("Kplus_PZ");
    TBranch* newB_Kminus_PE = tree->GetBranch("Kminus_PE");
    TBranch* newB_Kminus_PX = tree->GetBranch("Kminus_PX");
    TBranch* newB_Kminus_PY = tree->GetBranch("Kminus_PY");
    TBranch* newB_Kminus_PZ = tree->GetBranch("Kminus_PZ");


    //Create variables for kinematics AND Lorentzvectors
    Double_t Kplus_PE, Kplus_PX, Kplus_PY, Kplus_PZ;
    Double_t Kminus_PE, Kminus_PX, Kminus_PY, Kminus_PZ;

    
    TLorentzVector Vec_Kplus, Vec_Kminus,
                   Vec_Kplus_temp, Vec_Kminus_temp;             //temporary Lorentzvectors which will be modified with nominal masses
    
    TLorentzVector Vec_Kplus_Proton, Vec_Kminus_Proton, Vec_Kplus_Pion, Vec_Kminus_Pion, Vec_Proton_Pion, Vec_Pion_Proton;

    //Set Addresses to just created Variables
    B_Kplus_PE -> SetAddress(&Kplus_PE);
    B_Kplus_PX -> SetAddress(&Kplus_PX);
    B_Kplus_PY -> SetAddress(&Kplus_PY);
    B_Kplus_PZ -> SetAddress(&Kplus_PZ);

    B_Kminus_PE -> SetAddress(&Kminus_PE);
    B_Kminus_PX -> SetAddress(&Kminus_PX);
    B_Kminus_PY -> SetAddress(&Kminus_PY);
    B_Kminus_PZ -> SetAddress(&Kminus_PZ);

    newB_Kplus_PE -> SetAddress(&Kplus_PE);
    newB_Kplus_PX -> SetAddress(&Kplus_PX);
    newB_Kplus_PY -> SetAddress(&Kplus_PY);
    newB_Kplus_PZ -> SetAddress(&Kplus_PZ);

    newB_Kminus_PE -> SetAddress(&Kminus_PE);
    newB_Kminus_PX -> SetAddress(&Kminus_PX);
    newB_Kminus_PY -> SetAddress(&Kminus_PY);
    newB_Kminus_PZ -> SetAddress(&Kminus_PZ);


    std::cout << "found " << oldtree_entries << " entries" << std::endl;


    //Loop over oldtree now and fill newtree
    for (Long64_t i = 0; i < oldtree_entries; i++){
        //Get variablevalues of entry i
        oldtree -> GetEntry(i);

        //Initialise Lorentzvectors Kplus and Kminus
        Vec_Kplus.SetE(Kplus_PE);
        Vec_Kplus.SetPx(Kplus_PX);
        Vec_Kplus.SetPy(Kplus_PY);
        Vec_Kplus.SetPz(Kplus_PZ);

        Vec_Kminus.SetE(Kminus_PE);
        Vec_Kminus.SetPx(Kminus_PX);
        Vec_Kminus.SetPy(Kminus_PY);
        Vec_Kminus.SetPz(Kminus_PZ);



        //1. Kplus_Proton
        Vec_Kplus_temp = Vec_Kplus;
        Vec_Kminus_temp = Vec_Kminus;
       
        Vec_Kminus_temp.SetE( sqrt( Vec_Kminus_temp.Vect().Mag2() + pow(nominal_Proton_M, 2) ) );       //Mass of Kminus modified, NOTE: Vect() gets 3-momentum!

        Vec_Kplus_Proton = Vec_Kplus_temp + Vec_Kminus_temp;
        Kplus_Proton_M = Vec_Kplus_Proton.M();


        //2. Kminus_Proton
        Vec_Kplus_temp = Vec_Kplus;
        Vec_Kminus_temp = Vec_Kminus;
        
        Vec_Kplus_temp.SetE( sqrt( Vec_Kplus_temp.Vect().Mag2() + pow(nominal_Proton_M, 2)) );       //Mass of Kplus modified

        Vec_Kminus_Proton = Vec_Kplus_temp + Vec_Kminus_temp;
        Kminus_Proton_M = Vec_Kminus_Proton.M();

        
        //3. Kplus_Pion
        Vec_Kplus_temp = Vec_Kplus;
        Vec_Kminus_temp = Vec_Kminus;
        
        Vec_Kminus_temp.SetE( sqrt( Vec_Kminus_temp.Vect().Mag2() + pow(nominal_Pion_M, 2)) );       //Mass of Kminus modified

        Vec_Kplus_Pion = Vec_Kplus_temp + Vec_Kminus_temp;
        Kplus_Pion_M = Vec_Kplus_Pion.M();


        //4. Kminus_Pion
        Vec_Kplus_temp = Vec_Kplus;
        Vec_Kminus_temp = Vec_Kminus;
        
        Vec_Kplus_temp.SetE( sqrt( Vec_Kplus_temp.Vect().Mag2() + pow(nominal_Pion_M, 2)) );       //Mass of Kplus modified

        Vec_Kminus_Pion = Vec_Kplus_temp + Vec_Kminus_temp;
        Kminus_Pion_M = Vec_Kminus_Pion.M();


        //5. Proton_Pion
        Vec_Kplus_temp = Vec_Kplus;
        Vec_Kminus_temp = Vec_Kminus;
        
        Vec_Kplus_temp.SetE( sqrt( Vec_Kplus_temp.Vect().Mag2() + pow(nominal_Proton_M, 2)) );       //Mass of Kplus modified
        Vec_Kminus_temp.SetE( sqrt( Vec_Kminus_temp.Vect().Mag2() + pow(nominal_Pion_M, 2)) );       //Mass of Kminus modified

        Vec_Proton_Pion = Vec_Kplus_temp + Vec_Kminus_temp;
        Proton_Pion_M = Vec_Proton_Pion.M();


        //6. Pion_Proton
        Vec_Kplus_temp = Vec_Kplus;
        Vec_Kminus_temp = Vec_Kminus;
        
        Vec_Kplus_temp.SetE( sqrt( Vec_Kplus_temp.Vect().Mag2() + pow(nominal_Pion_M, 2)) );       //Mass of Kplus modified
        Vec_Kminus_temp.SetE( sqrt( Vec_Kminus_temp.Vect().Mag2() + pow(nominal_Proton_M, 2)) );       //Mass of Kminus modified
        
        Vec_Pion_Proton = Vec_Kplus_temp + Vec_Kminus_temp;
        Pion_Proton_M = Vec_Pion_Proton.M();


        //Fill (new) Tree
        tree -> Fill();

    }
    tree -> AutoSave();

    
    return 0;
}
