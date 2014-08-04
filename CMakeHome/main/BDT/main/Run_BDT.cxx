
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TLegend.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TMVA/Factory.h"
#include "TMVA/Reader.h"
#include "TMVA/Tools.h"
#include "TStyle.h"
#include "TMath.h"



// Calculate optimal BDT cuts ===============================
double get_BDTcut(TFile *file, double fNSignal, double fNBackground){
    TH1* sigeff = (TH1*) file->Get("Method_BDT/BDT/MVA_BDT_effS");
    TH1* bkgeff = (TH1*) file->Get("Method_BDT/BDT/MVA_BDT_effB");
    TH1* sSig = (TH1*) sigeff->Clone();
    double maxSig = -1;
    for (Int_t i=1; i<=sigeff->GetNbinsX(); i++) {
        Float_t eS = sigeff->GetBinContent( i );
        Float_t S = eS * fNSignal;
        Float_t B = bkgeff->GetBinContent( i ) * fNBackground;

        Double_t sig = S/TMath::Sqrt(S+B);
        if (sig > maxSig) 
            maxSig    = sig;
        sSig->SetBinContent( i, sig );   
    }
    double BDT_cut = sSig->GetXaxis()->GetBinCenter(sSig->GetMaximumBin());
    std::cout<<"Maximum Significance "<<maxSig<<" when cutting at "<<BDT_cut<<std::endl;

    return BDT_cut;
}





// Main function
int main( int argc, char** argv )
{
    if(argc != 2) {
        std::cout << "Type in training, application or cutting as argument" << std::endl;
        return 1;
    }
    std::string arg1 = argv[1];

    gStyle->SetOptStat(0);
    //  gROOT->SetStyle("Plain");
    // gROOT->ProcessLine(".x /afs/cern.ch/work/c/clinn/macros/lhcbStyle.C");






    // -----------------------------------
    // Begin EDITING HERE
    // -----------------------------------



    double nSig = 396.5, 
           nBkg = 24341.4;

    TString fname1 = "/afs/cern.ch/work/d/dberning/private/BDT/Input/BDT_Signal.root";           //Signal
    TString fname2 = "/afs/cern.ch/work/d/dberning/private/BDT/Input/BDT_Background.root";                     //Background      
    TFile *input1 = TFile::Open( fname1 );
    TFile *input2 = TFile::Open( fname2 );
    TTree *signal1     = (TTree*)input1->Get("Bs2phimumuTuple/DecayTree");
    //TTree *background1 = (TTree*)input1->Get("bkg_large");
    //TTree *signal2     = (TTree*)input2->Get("jpsiphi");
    TTree *background2 = (TTree*)input2->Get("Bs2phimumuTuple/DecayTree");

    TFile *outfile[2];

    //Declaration of Variables for the training which are used for the application as well
    double cut;
    TString id( "Bs2mumuf2_BDTselection_FINAL_newInput" );
    TString save_path = ( "/afs/cern.ch/work/d/dberning/private/BDT/Trainingoutput/" );


    if(arg1 == "training") {
        //=== TMVA Selection ===========================================
        outfile[0] = TFile::Open( save_path + id + ".root", "RECREATE" );

        TMVA::Factory *factory1 = new TMVA::Factory( id, outfile[0],
                "!V:!Silent:Color:DrawProgressBar:Transformations=I;G:AnalysisType=Classification" );
        factory1->AddSignalTree    ( signal1,     1.0     );
        factory1->AddBackgroundTree( background2, 1.0 );
        //factory1->SetSignalWeightExpression("B0_SWeights");
        factory1->PrepareTrainingAndTestTree( "", "SplitMode=random:!V" );

        factory1->AddVariable( "B0_PT", 'F' );
        factory1->AddVariable( "B0_IPCHI2_OWNPV", 'F' );
        factory1->AddVariable( "B0_FDCHI2_OWNPV", 'F' );
        factory1->AddVariable( "B0_DIRA_OWNPV", 'F' );
        factory1->AddVariable( "B0_ENDVERTEX_CHI2", 'F' );
        factory1->AddVariable( "Kplus_IPCHI2_OWNPV", 'F' );
        factory1->AddVariable( "Kminus_IPCHI2_OWNPV", 'F' );
        factory1->AddVariable( "muminus_IPCHI2_OWNPV", 'F' );
        factory1->AddVariable( "muplus_IPCHI2_OWNPV", 'F' );
        factory1->AddVariable( "Kplus_PIDK", 'F' );
        factory1->AddVariable( "Kminus_PIDK", 'F' );
        factory1->AddVariable( "muminus_PIDmu", 'F' );
        factory1->AddVariable( "muplus_PIDmu", 'F' );
        //factory1->AddVariable( "min(K+ PIDK, K-PIDK) := min(Kplus_ProbNNk, Kminus_ProbNNk)", 'F' );
        //factory1->AddVariable( "max(K+ PIDK, K-PIDK) := max(Kplus_ProbNNk, Kminus_ProbNNk)", 'F' );
        //factory1->AddVariable( "min(mu+ PIDmu, mu-PIDmu) := min(muplus_ProbNNmu, muminus_ProbNNmu)", 'F' );
        factory1->AddVariable( "Kminus_PT", 'F' );
        factory1->AddVariable( "Kplus_PT", 'F' );

        //zusätzliche Variablen
        //factory1->AddVariable( "B0_P", 'F' );
        //factory1->AddVariable( "Kplus_P", 'F' );
        //factory1->AddVariable( "Kminus_P", 'F' );
        //factory1->AddVariable( "phi_1020_PT", 'F' );
        //factory1->AddVariable( "phi_1020_P", 'F' );
        factory1->AddVariable( "phi_1020_ENDVERTEX_CHI2", 'F' );
        factory1->AddVariable( "Kplus_PIDp", 'F' );
        factory1->AddVariable( "Kminus_PIDp", 'F' );
        factory1->AddVariable( "Kplus_TRACK_CHI2NDOF", 'F' );
        factory1->AddVariable( "Kminus_TRACK_CHI2NDOF", 'F' );
        factory1->AddVariable( "muplus_TRACK_CHI2NDOF", 'F' );
        factory1->AddVariable( "muminus_TRACK_CHI2NDOF", 'F' );




        factory1->BookMethod( TMVA::Types::kBDT, "BDT",
                "!H:!V:NTrees=800:MinNodeSize=2.5%:MaxDepth=2:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=150");





        factory1->TrainAllMethods();
        factory1->TestAllMethods();
        factory1->EvaluateAllMethods();

        cut = get_BDTcut(outfile[0], nSig, nBkg);





        outfile[0]->Close();
        //delete factory1;




    }   //End of Training



















    else if(arg1 == "cutting") {
        //Begin Cut-Optimization (with Christians Function) =======================
        outfile[0] = TFile::Open( save_path + id + ".root", "READ" );
        cut = get_BDTcut(outfile[0], nSig, nBkg);

        outfile[0] -> Close();
    
    }   //End of Cut-Optimization


















    else if(arg1 == "application") {
        // Begin MVA Application for 2012 =============================

        TMVA::Reader *reader1 = new TMVA::Reader( "!Color:!Silent" );

        Float_t b_pt, b_ipchi2, b_fdchi2, b_dira, b_vtxchi2;
        Float_t kp_ipchi2, km_ipchi2, mup_ipchi2, mum_ipchi2;
        Float_t kp_pidk, km_pidk, mup_pidmu, mum_pidmu;
        Float_t kp_pt, km_pt, kp_p, km_p, b_p, phi_p, phi_pt, phi_vtxchi2;
        Float_t kp_pidp, km_pidp, kp_trackchi2, km_trackchi2, mup_trackchi2, mum_trackchi2;

        reader1->AddVariable( "B0_PT", &b_pt );
        reader1->AddVariable( "B0_IPCHI2_OWNPV", &b_ipchi2 );
        reader1->AddVariable( "B0_FDCHI2_OWNPV", &b_fdchi2 );
        reader1->AddVariable( "B0_DIRA_OWNPV", &b_dira );
        reader1->AddVariable( "B0_ENDVERTEX_CHI2", &b_vtxchi2 );
        reader1->AddVariable( "Kplus_IPCHI2_OWNPV", &kp_ipchi2 );
        reader1->AddVariable( "Kminus_IPCHI2_OWNPV", &km_ipchi2 );
        reader1->AddVariable( "muminus_IPCHI2_OWNPV", &mum_ipchi2 );
        reader1->AddVariable( "muplus_IPCHI2_OWNPV", &mup_ipchi2 );
        reader1->AddVariable( "Kplus_PIDK", &kp_pidk );
        reader1->AddVariable( "Kminus_PIDK", &km_pidk );
        reader1->AddVariable( "muminus_PIDmu", &mum_pidmu );
        reader1->AddVariable( "muplus_PIDmu", &mup_pidmu );
        reader1->AddVariable( "Kminus_PT", &km_pt );
        reader1->AddVariable( "Kplus_PT", &kp_pt );

        //zusätzliche Variablen
        //reader1->AddVariable( "B0_P", &b_p );
        //reader1->AddVariable( "Kplus_P", &kp_p );
        //reader1->AddVariable( "Kminus_P", &km_p );
        //reader1->AddVariable( "phi_1020_PT", &phi_pt );
        //reader1->AddVariable( "phi_1020_P", &phi_p );
        reader1->AddVariable( "phi_1020_ENDVERTEX_CHI2", &phi_vtxchi2 );
        reader1->AddVariable( "Kplus_PIDp", &kp_pidp );
        reader1->AddVariable( "Kminus_PIDp", &km_pidp );
        reader1->AddVariable( "Kplus_TRACK_CHI2NDOF", &kp_trackchi2 );
        reader1->AddVariable( "Kminus_TRACK_CHI2NDOF", &km_trackchi2 );
        reader1->AddVariable( "muplus_TRACK_CHI2NDOF", &mup_trackchi2 );
        reader1->AddVariable( "muminus_TRACK_CHI2NDOF", &mum_trackchi2 );


        
        reader1->BookMVA( "BDT", "weights/" + id + "_BDT.weights.xml" );






        
        TFile *output1 = new TFile("/afs/cern.ch/work/d/dberning/private/BDT/Applicationoutput/" + id + ".root", "update");         //File which Tree with Data
        TTree *tree     = (TTree*)output1->Get("Bs2phimumuTuple/DecayTree");


        std::cout << "--- TMVAClassificationApp    : Using output file: " << output1->GetName() << std::endl;

        //Set double variables... (for the reader there is float needed and for the tree-branches doubles... -.-)
        Double_t db_pt, db_ipchi2, db_fdchi2, db_dira, db_vtxchi2;
        Double_t dkp_ipchi2, dkm_ipchi2, dmup_ipchi2, dmum_ipchi2;
        Double_t dkp_pidk, dkm_pidk, dmup_pidmu, dmum_pidmu;
        Double_t dkp_pt, dkm_pt, dkp_p, dkm_p, db_p, dphi_p, dphi_pt, dphi_vtxchi2;
        Double_t dkp_pidp, dkm_pidp, dkp_trackchi2, dkm_trackchi2, dmup_trackchi2, dmum_trackchi2;

        //SetBranchAddress for the outputtree
        tree->SetBranchAddress( "B0_PT", &db_pt );
        std::cout << "Branchadress B0_PT gesetzt" << std::endl;
        tree->SetBranchAddress( "B0_IPCHI2_OWNPV", &db_ipchi2 );
        tree->SetBranchAddress( "B0_FDCHI2_OWNPV", &db_fdchi2 );
        tree->SetBranchAddress( "B0_DIRA_OWNPV", &db_dira );
        tree->SetBranchAddress( "B0_ENDVERTEX_CHI2", &db_vtxchi2 );
        tree->SetBranchAddress( "Kplus_IPCHI2_OWNPV", &dkp_ipchi2 );
        tree->SetBranchAddress( "Kminus_IPCHI2_OWNPV", &dkm_ipchi2 );
        tree->SetBranchAddress( "muminus_IPCHI2_OWNPV", &dmum_ipchi2 );
        tree->SetBranchAddress( "muplus_IPCHI2_OWNPV", &dmup_ipchi2 );
        tree->SetBranchAddress( "Kplus_PIDK", &dkp_pidk );
        tree->SetBranchAddress( "Kminus_PIDK", &dkm_pidk );
        tree->SetBranchAddress( "muminus_PIDmu", &dmum_pidmu );
        tree->SetBranchAddress( "muplus_PIDmu", &dmup_pidmu );
        tree->SetBranchAddress( "Kminus_PT", &dkm_pt );
        tree->SetBranchAddress( "Kplus_PT", &dkp_pt );

        //zusätzliche Variablen
        //tree->SetBranchAddress( "B0_P", &db_p );
        //tree->SetBranchAddress( "Kplus_P", &dkp_p );
        //tree->SetBranchAddress( "Kminus_P", &dkm_p );
        //tree->SetBranchAddress( "phi_1020_PT", &dphi_pt );
        //tree->SetBranchAddress( "phi_1020_P", &dphi_p );
        tree->SetBranchAddress( "phi_1020_ENDVERTEX_CHI2", &dphi_vtxchi2 );
        tree->SetBranchAddress( "Kplus_PIDp", &dkp_pidp );
        tree->SetBranchAddress( "Kminus_PIDp", &dkm_pidp );
        tree->SetBranchAddress( "Kplus_TRACK_CHI2NDOF", &dkp_trackchi2 );
        tree->SetBranchAddress( "Kminus_TRACK_CHI2NDOF", &dkm_trackchi2 );
        tree->SetBranchAddress( "muplus_TRACK_CHI2NDOF", &dmup_trackchi2 );
        tree->SetBranchAddress( "muminus_TRACK_CHI2NDOF", &dmum_trackchi2 );

        std::cout << "Branchadressen gesetzt" << std::endl;


        //Create new Branch for TMVAresponse
        Double_t tmvaresponse;
        TBranch* branch_tmvaresponse = tree->Branch("TMVAResponse", &tmvaresponse, 'F');


        std::cout << "--- Processing: " << tree->GetEntries() << " events" << std::endl;
        for (Long64_t ievt=0; ievt < tree->GetEntries(); ievt++) {
            if (ievt%50000 == 0) { 
                std::cout << "--- ... Processing event: " << ievt;
                std::cout << "  |  EVALUATED BDT VALUE:   " << reader1->EvaluateMVA("BDT") << std::endl; 
            }
            tree -> GetEntry(ievt);

            //Convert the Double_t's to Float_t's...
            b_pt = db_pt;
            b_ipchi2 = db_ipchi2;
            b_fdchi2 = db_fdchi2;
            b_dira = db_dira;
            b_vtxchi2 = db_vtxchi2;
            kp_ipchi2 = dkp_ipchi2;
            km_ipchi2 = dkm_ipchi2;
            mum_ipchi2 = dmum_ipchi2;
            mup_ipchi2 = dmup_ipchi2;
            kp_pidk = dkp_pidk;
            km_pidk = dkm_pidk;
            mum_pidmu = dmum_pidmu;
            mup_pidmu = dmup_pidmu;
            km_pt = dkm_pt;
            kp_pt = dkp_pt;
            b_p = db_p;
            kp_p = dkp_p;
            km_p = dkm_p;
            phi_pt = dphi_pt;
            phi_p = dphi_p;
            phi_vtxchi2 = dphi_vtxchi2;
            kp_pidp = dkp_pidp;
            km_pidp = dkm_pidp;
            kp_trackchi2 = dkp_trackchi2;
            km_trackchi2 = dkm_trackchi2;
            mup_trackchi2 = dmup_trackchi2;
            mum_trackchi2 = dmum_trackchi2;


            tmvaresponse = reader1->EvaluateMVA("BDT");
            branch_tmvaresponse->Fill();

        }



        tree->Print();
        tree->AutoSave(); 

        delete output1;

    }   //End of application

    else {
        std::cout << "Wrong argument: use training or application" << std::endl;
    }

}

