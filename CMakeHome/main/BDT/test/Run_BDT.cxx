
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




int main( int argc, char** argv )
{

  gStyle->SetOptStat(0);
 //  gROOT->SetStyle("Plain");
 // gROOT->ProcessLine(".x /afs/cern.ch/work/c/clinn/macros/lhcbStyle.C");

   double nSig2012 = 1022., nBkg2012 = 1314.;
   double nSig2011 = 479., nBkg2011 = 868.;
   
   


   TString fname1 = "/afs/cern.ch/work/d/dberning/private/BDT/Bsf2mumu_MC_merged_truthmatched.root";
   TString fname2 = "/afs/cern.ch/work/d/dberning/private/BDT/Non_resonant_Background.root";
   TFile *input1 = TFile::Open( fname1 );
   TFile *input2 = TFile::Open( fname2 );
   TTree *signal1     = (TTree*)input1->Get("DecayTree");
   //TTree *background1 = (TTree*)input1->Get("bkg_large");
   //TTree *signal2     = (TTree*)input2->Get("jpsiphi");
   TTree *background2 = (TTree*)input2->Get("DecayTree");

   TFile *outfile[2];
 
//=== TMVA Selection 2012 ===========================================
   TString id( "BDT_2012" );
   outfile[0] = TFile::Open( id+".root", "RECREATE" );

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
   //factory1->AddVariable( "min(K+ PIDK, K-PIDK) := min(Kplus_ProbNNk, Kminus_ProbNNk)", 'F' );
   //factory1->AddVariable( "max(K+ PIDK, K-PIDK) := max(Kplus_ProbNNk, Kminus_ProbNNk)", 'F' );
   //factory1->AddVariable( "min(mu+ PIDmu, mu-PIDmu) := min(muplus_ProbNNmu, muminus_ProbNNmu)", 'F' );
   factory1->AddVariable( "Kminus_PT", 'F' );
   factory1->AddVariable( "Kplus_PT", 'F' );

   std::cout << "Wir sind nach Add" << std::endl; 
   factory1->BookMethod( TMVA::Types::kBDT, "BDT",
                          "!H:!V:NTrees=800:MinNodeSize=2.5%:MaxDepth=2:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=150");
   //factory1->BookMethod( TMVA::Types::kBDT, "BDT",
   //                        "!H:!V:NTrees=800:MinNodeSize=1.5%:MaxDepth=2:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=150");

   std::cout << "stage2" << std::endl;
   factory1->TrainAllMethods();
   factory1->TestAllMethods();
   factory1->EvaluateAllMethods();
  
   double cut2012 = get_BDTcut(outfile[0], nSig2012, nBkg2012);
   

   outfile[0]->Close();
   delete factory1;


/*
//=== TMVA Selection 2011 =======================================

   id = "BDT_2011";
   outfile[1] = TFile::Open( id + ".root", "RECREATE" );

   TMVA::Factory *factory2 = new TMVA::Factory( id, outfile[1],
                                               "!V:!Silent:Color:DrawProgressBar:Transformations=I;G:AnalysisType=Classification" );
   factory2->AddSignalTree    ( signal2,     1.0     );
   factory2->AddBackgroundTree( background2, 1.0 );
   factory2->SetSignalWeightExpression("B0_SWeights");
   factory2->PrepareTrainingAndTestTree( "", "SplitMode=random:!V" );
   
   factory2->AddVariable( "B0_PT", 'F' );
   factory2->AddVariable( "B0_IPCHI2_OWNPV", 'F' );
   factory2->AddVariable( "B0_FDCHI2_OWNPV", 'F' );
   factory2->AddVariable( "B0_DIRA_OWNPV", 'F' );
   factory2->AddVariable( "B0_ENDVERTEX_CHI2", 'F' );
   factory2->AddVariable( "Kplus_IPCHI2_OWNPV", 'F' );
   factory2->AddVariable( "Kminus_IPCHI2_OWNPV", 'F' );
   factory2->AddVariable( "muminus_IPCHI2_OWNPV", 'F' );
   factory2->AddVariable( "muplus_IPCHI2_OWNPV", 'F' );
   factory2->AddVariable( "min(K+ PIDK, K-PIDK) := min(Kplus_ProbNNk, Kminus_ProbNNk)", 'F' );
   factory2->AddVariable( "max(K+ PIDK, K-PIDK) := max(Kplus_ProbNNk, Kminus_ProbNNk)", 'F' );
   factory2->AddVariable( "min(mu+ PIDmu, mu-PIDmu) := min(muplus_ProbNNmu, muminus_ProbNNmu)", 'F' );
   factory2->AddVariable( "Kminus_PT", 'F' );
   factory2->AddVariable( "Kplus_PT", 'F' );
   
   factory2->BookMethod( TMVA::Types::kBDT, "BDT",
                          "!H:!V:NTrees=800:MinNodeSize=2.5%:MaxDepth=2:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=150");
  // factory2->BookMethod( TMVA::Types::kBDT, "BDT",
  //                        "!H:!V:NTrees=800:MinNodeSize=1.5%:MaxDepth=2:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=150");

  factory2->TrainAllMethods();
  factory2->TestAllMethods();
  factory2->EvaluateAllMethods();
   
  double cut2011 = get_BDTcut(outfile[1], nSig2011, nBkg2011);
  double cutReso2011 = get_BDTcut(outfile[1], nResoSig2011, nResoBkg2011);

  outfile[1]->Close();

  delete factory2;
*/

/*
// Begin MVA Application for 2012 =============================

   TMVA::Reader *reader1 = new TMVA::Reader( "!Color:!Silent" );
   Float_t b_pt, b_ipchi2, b_fdchi2, b_vtxchi2, b_dira;
   Float_t kp_ipchi2, km_ipchi2, mup_ipchi2, mum_ipchi2;
   Float_t min_pidk, max_pidk, min_pidmu, km_pt, kp_pt;

   reader1->AddVariable( "B0_PT", &b_pt );
   reader1->AddVariable( "B0_IPCHI2_OWNPV", &b_ipchi2 );
   reader1->AddVariable( "B0_FDCHI2_OWNPV", &b_fdchi2);
   reader1->AddVariable( "B0_DIRA_OWNPV", &b_dira);
   reader1->AddVariable( "B0_ENDVERTEX_CHI2", &b_vtxchi2 );
   reader1->AddVariable( "Kplus_IPCHI2_OWNPV", &kp_ipchi2);
   reader1->AddVariable( "Kminus_IPCHI2_OWNPV", &km_ipchi2 );
   reader1->AddVariable( "muminus_IPCHI2_OWNPV", &mum_ipchi2);
   reader1->AddVariable( "muplus_IPCHI2_OWNPV", &mup_ipchi2);
   reader1->AddVariable( "min(K+ PIDK, K-PIDK) := min(Kplus_ProbNNk, Kminus_ProbNNk)", &min_pidk );
   reader1->AddVariable( "max(K+ PIDK, K-PIDK) := max(Kplus_ProbNNk, Kminus_ProbNNk)", &max_pidk);
   reader1->AddVariable( "min(mu+ PIDmu, mu-PIDmu) := min(muplus_ProbNNmu, muminus_ProbNNmu)", &min_pidmu );
   reader1->AddVariable( "Kminus_PT", &km_pt );
   reader1->AddVariable( "Kplus_PT", &kp_pt );

   reader1->BookMVA( "BDT method", "weights/BDT_2012_BDT.weights.xml" );

   TFile *f = TFile::Open( fname1 );

   std::cout << "--- TMVAClassificationApp    : Using input file: " << f->GetName() << std::endl;
   
   TTree* phimumu = (TTree*)f->Get("phimumu");
   TTree* jpsiphi = (TTree*)f->Get("jpsiphi");

   
   TFile *output1 = new TFile("phimumu_2012_selected.root","recreate");
   TTree *sigtree = phimumu->CloneTree(0);
   TTree *resotree = jpsiphi->CloneTree(0);

   Double_t db_pt, db_ipchi2, db_fdchi2, db_vtxchi2, db_dira;
   Double_t dkp_ipchi2, dkm_ipchi2, dmup_ipchi2, dmum_ipchi2;
   Double_t kp_pidk, km_pidk, mum_pidmu, mup_pidmu, dkm_pt, dkp_pt;;

  //phimumu
   phimumu->SetBranchAddress( "B0_PT", &db_pt );
   phimumu->SetBranchAddress( "B0_IPCHI2_OWNPV", &db_ipchi2 );
   phimumu->SetBranchAddress( "B0_FDCHI2_OWNPV", &db_fdchi2);
   phimumu->SetBranchAddress( "B0_DIRA_OWNPV", &db_dira);
   phimumu->SetBranchAddress( "B0_ENDVERTEX_CHI2", &db_vtxchi2 );
   phimumu->SetBranchAddress( "Kplus_IPCHI2_OWNPV", &dkp_ipchi2);
   phimumu->SetBranchAddress( "Kminus_IPCHI2_OWNPV", &dkm_ipchi2 );
   phimumu->SetBranchAddress( "muminus_IPCHI2_OWNPV", &dmum_ipchi2);
   phimumu->SetBranchAddress( "muplus_IPCHI2_OWNPV", &dmup_ipchi2);
   phimumu->SetBranchAddress("Kminus_ProbNNk",&km_pidk);
   phimumu->SetBranchAddress("Kplus_ProbNNk",&kp_pidk);
   phimumu->SetBranchAddress("muminus_ProbNNmu",&mum_pidmu);
   phimumu->SetBranchAddress("muplus_ProbNNmu",&mup_pidmu);
   phimumu->SetBranchAddress( "Kminus_PT", &dkm_pt );
   phimumu->SetBranchAddress( "Kplus_PT", &dkp_pt );


   std::cout << "--- Processing: " << phimumu->GetEntries() << " events" << std::endl;
   for (Long64_t ievt=0; ievt < phimumu->GetEntries(); ievt++) {
      if (ievt%1000 == 0) std::cout << "--- ... Processing event: " << ievt << std::endl;

      phimumu->GetEntry(ievt);

      b_pt = float(db_pt);
      b_ipchi2 = float(db_ipchi2);
      b_fdchi2 = float(db_fdchi2);
      b_dira = float(db_dira);
      b_vtxchi2 = float(db_vtxchi2);
      kp_ipchi2 = float(dkp_ipchi2);
      km_ipchi2 = float(dkm_ipchi2);
      mup_ipchi2 = float(dmup_ipchi2);
      mum_ipchi2 = float(dmum_ipchi2);

      min_pidk = std::min(kp_pidk, km_pidk);
      max_pidk = std::max(kp_pidk, km_pidk);
      min_pidmu = std::min(mup_pidmu, mum_pidmu);
  
      km_pt = float(dkm_pt);
      kp_pt = float(dkp_pt);
//     std::cout<<"EVALUATED BDT VALUE:   "<<reader1->EvaluateMVA("BDT method")<<"   "<<cut2012<<std::endl; 
     if(reader1->EvaluateMVA("BDT method") > cut2012)
	sigtree->Fill();

   }


  //jpsiphi
   jpsiphi->SetBranchAddress( "B0_PT", &db_pt );
   jpsiphi->SetBranchAddress( "B0_IPCHI2_OWNPV", &db_ipchi2 );
   jpsiphi->SetBranchAddress( "B0_FDCHI2_OWNPV", &db_fdchi2);
   jpsiphi->SetBranchAddress( "B0_DIRA_OWNPV", &db_dira);
   jpsiphi->SetBranchAddress( "B0_ENDVERTEX_CHI2", &db_vtxchi2 );
   jpsiphi->SetBranchAddress( "Kplus_IPCHI2_OWNPV", &dkp_ipchi2);
   jpsiphi->SetBranchAddress( "Kminus_IPCHI2_OWNPV", &dkm_ipchi2 );
   jpsiphi->SetBranchAddress( "muminus_IPCHI2_OWNPV", &dmum_ipchi2);
   jpsiphi->SetBranchAddress( "muplus_IPCHI2_OWNPV", &dmup_ipchi2);
   jpsiphi->SetBranchAddress("Kminus_ProbNNk",&km_pidk);
   jpsiphi->SetBranchAddress("Kplus_ProbNNk",&kp_pidk);
   jpsiphi->SetBranchAddress("muminus_ProbNNmu",&mum_pidmu);
   jpsiphi->SetBranchAddress("muplus_ProbNNmu",&mup_pidmu);
   jpsiphi->SetBranchAddress( "Kminus_PT", &dkm_pt );
   jpsiphi->SetBranchAddress( "Kplus_PT", &dkp_pt );

   std::cout << "--- Processing: " << jpsiphi->GetEntries() << " events" << std::endl;
   for (Long64_t ievt=0; ievt < jpsiphi->GetEntries(); ievt++) {
      if (ievt%1000 == 0) std::cout << "--- ... Processing event: " << ievt << std::endl;

      jpsiphi->GetEntry(ievt);

      b_pt = float(db_pt);
      b_ipchi2 = float(db_ipchi2);
      b_fdchi2 = float(db_fdchi2);
      b_dira = float(db_dira);
      b_vtxchi2 = float(db_vtxchi2);
      kp_ipchi2 = float(dkp_ipchi2);
      km_ipchi2 = float(dkm_ipchi2);
      mup_ipchi2 = float(dmup_ipchi2);
      mum_ipchi2 = float(dmum_ipchi2);

      min_pidk = std::min(kp_pidk, km_pidk);
      max_pidk = std::max(kp_pidk, km_pidk);
      min_pidmu = std::min(mup_pidmu, mum_pidmu);
      km_pt = float(dkm_pt);
      kp_pt = float(dkp_pt);

//     std::cout<<"EVALUATED BDT VALUE:   "<<reader1->EvaluateMVA("BDT method")<<std::endl; 
     if(reader1->EvaluateMVA("BDT method") > cutReso2012)
	resotree->Fill();
  

   }
   
   sigtree->AutoSave();   
   resotree->AutoSave();
   delete output1;

// Begin MVA Application for 2011 =============================

   TMVA::Reader *reader2 = new TMVA::Reader( "!Color:!Silent" );

   reader2->AddVariable( "B0_PT", &b_pt );
   reader2->AddVariable( "B0_IPCHI2_OWNPV", &b_ipchi2 );
   reader2->AddVariable( "B0_FDCHI2_OWNPV", &b_fdchi2);
   reader2->AddVariable( "B0_DIRA_OWNPV", &b_dira);
   reader2->AddVariable( "B0_ENDVERTEX_CHI2", &b_vtxchi2 );
   reader2->AddVariable( "Kplus_IPCHI2_OWNPV", &kp_ipchi2);
   reader2->AddVariable( "Kminus_IPCHI2_OWNPV", &km_ipchi2 );
   reader2->AddVariable( "muminus_IPCHI2_OWNPV", &mum_ipchi2);
   reader2->AddVariable( "muplus_IPCHI2_OWNPV", &mup_ipchi2);
   reader2->AddVariable( "min(K+ PIDK, K-PIDK) := min(Kplus_ProbNNk, Kminus_ProbNNk)", &min_pidk );
   reader2->AddVariable( "max(K+ PIDK, K-PIDK) := max(Kplus_ProbNNk, Kminus_ProbNNk)", &max_pidk);
   reader2->AddVariable( "min(mu+ PIDmu, mu-PIDmu) := min(muplus_ProbNNmu, muminus_ProbNNmu)", &min_pidmu );
   reader2->AddVariable( "Kminus_PT", &km_pt);
   reader2->AddVariable( "Kplus_PT", &kp_pt );

   reader2->BookMVA( "BDT method", "weights/BDT_2011_BDT.weights.xml" );

   f = TFile::Open( fname2 );

   std::cout << "--- TMVAClassificationApp    : Using input file: " << f->GetName() << std::endl;
   
   phimumu = (TTree*)f->Get("phimumu");
   jpsiphi = (TTree*)f->Get("jpsiphi");

   
   output1 = new TFile("phimumu_2011_selected.root","recreate");
   sigtree = phimumu->CloneTree(0);
   resotree = jpsiphi->CloneTree(0);

  //phimumu
   phimumu->SetBranchAddress( "B0_PT", &db_pt );
   phimumu->SetBranchAddress( "B0_IPCHI2_OWNPV", &db_ipchi2 );
   phimumu->SetBranchAddress( "B0_FDCHI2_OWNPV", &db_fdchi2);
   phimumu->SetBranchAddress( "B0_DIRA_OWNPV", &db_dira);
   phimumu->SetBranchAddress( "B0_ENDVERTEX_CHI2", &db_vtxchi2 );
   phimumu->SetBranchAddress( "Kplus_IPCHI2_OWNPV", &dkp_ipchi2);
   phimumu->SetBranchAddress( "Kminus_IPCHI2_OWNPV", &dkm_ipchi2 );
   phimumu->SetBranchAddress( "muminus_IPCHI2_OWNPV", &dmum_ipchi2);
   phimumu->SetBranchAddress( "muplus_IPCHI2_OWNPV", &dmup_ipchi2);
   phimumu->SetBranchAddress("Kminus_ProbNNk",&km_pidk);
   phimumu->SetBranchAddress("Kplus_ProbNNk",&kp_pidk);
   phimumu->SetBranchAddress("muminus_ProbNNmu",&mum_pidmu);
   phimumu->SetBranchAddress("muplus_ProbNNmu",&mup_pidmu);
   phimumu->SetBranchAddress("Kminus_ProbNNk",&km_pidk);
   phimumu->SetBranchAddress("Kplus_ProbNNk",&kp_pidk);
   phimumu->SetBranchAddress( "Kminus_PT", &dkm_pt );
   phimumu->SetBranchAddress( "Kplus_PT", &dkp_pt );

   std::cout << "--- Processing: " << phimumu->GetEntries() << " events" << std::endl;
   for (Long64_t ievt=0; ievt < phimumu->GetEntries(); ievt++) {
      if (ievt%1000 == 0) std::cout << "--- ... Processing event: " << ievt << std::endl;

      phimumu->GetEntry(ievt);

      b_pt = float(db_pt);
      b_ipchi2 = float(db_ipchi2);
      b_fdchi2 = float(db_fdchi2);
      b_dira = float(db_dira);
      b_vtxchi2 = float(db_vtxchi2);
      kp_ipchi2 = float(dkp_ipchi2);
      km_ipchi2 = float(dkm_ipchi2);
      mup_ipchi2 = float(dmup_ipchi2);
      mum_ipchi2 = float(dmum_ipchi2);

      min_pidk = std::min(kp_pidk, km_pidk);
      max_pidk = std::max(kp_pidk, km_pidk);
      min_pidmu = std::min(mup_pidmu, mum_pidmu);
      km_pt = float(dkm_pt);
      kp_pt = float(dkp_pt);

//     std::cout<<"EVALUATED BDT VALUE:   "<<reader->EvaluateMVA("BDT method")<<"   "<<cut2011<<std::endl; 
     if(reader2->EvaluateMVA("BDT method") > cut2011)
	sigtree->Fill();
  

   }

  //jpsiphi
   jpsiphi->SetBranchAddress( "B0_PT", &db_pt );
   jpsiphi->SetBranchAddress( "B0_IPCHI2_OWNPV", &db_ipchi2 );
   jpsiphi->SetBranchAddress( "B0_FDCHI2_OWNPV", &db_fdchi2);
   jpsiphi->SetBranchAddress( "B0_DIRA_OWNPV", &db_dira);
   jpsiphi->SetBranchAddress( "B0_ENDVERTEX_CHI2", &db_vtxchi2 );
   jpsiphi->SetBranchAddress( "Kplus_IPCHI2_OWNPV", &dkp_ipchi2);
   jpsiphi->SetBranchAddress( "Kminus_IPCHI2_OWNPV", &dkm_ipchi2 );
   jpsiphi->SetBranchAddress( "muminus_IPCHI2_OWNPV", &dmum_ipchi2);
   jpsiphi->SetBranchAddress( "muplus_IPCHI2_OWNPV", &dmup_ipchi2);
   jpsiphi->SetBranchAddress("Kminus_ProbNNk",&km_pidk);
   jpsiphi->SetBranchAddress("Kplus_ProbNNk",&kp_pidk);
   jpsiphi->SetBranchAddress("muminus_ProbNNmu",&mum_pidmu);
   jpsiphi->SetBranchAddress("muplus_ProbNNmu",&mup_pidmu);
   jpsiphi->SetBranchAddress( "Kminus_PT", &dkm_pt );
   jpsiphi->SetBranchAddress( "Kplus_PT", &dkp_pt );

   std::cout << "--- Processing: " << jpsiphi->GetEntries() << " events" << std::endl;
   for (Long64_t ievt=0; ievt < jpsiphi->GetEntries(); ievt++) {
      if (ievt%1000 == 0) std::cout << "--- ... Processing event: " << ievt << std::endl;

      jpsiphi->GetEntry(ievt);

      b_pt = float(db_pt);
      b_ipchi2 = float(db_ipchi2);
      b_fdchi2 = float(db_fdchi2);
      b_dira = float(db_dira);
      b_vtxchi2 = float(db_vtxchi2);
      kp_ipchi2 = float(dkp_ipchi2);
      km_ipchi2 = float(dkm_ipchi2);
      mup_ipchi2 = float(dmup_ipchi2);
      mum_ipchi2 = float(dmum_ipchi2);

      min_pidk = std::min(kp_pidk, km_pidk);
      max_pidk = std::max(kp_pidk, km_pidk);
      min_pidmu = std::min(mup_pidmu, mum_pidmu);
      km_pt = float(dkm_pt);
      kp_pt = float(dkp_pt);

//     std::cout<<"EVALUATED BDT VALUE:   "<<reader->EvaluateMVA("BDT method")<<std::endl; 
     if(reader2->EvaluateMVA("BDT method") > cutReso2011)
	resotree->Fill();
  

   }
   
   sigtree->AutoSave();   
   resotree->AutoSave();
   delete output1;
*/
}

