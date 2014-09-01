#include <iostream>
#include <TTree.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TH1D.h>


int main() {



    //Data (pruned) after stripping
    TFile* filedata = new TFile("/afs/cern.ch/work/d/dberning/private/Pruned/Data/Data_merged_pruned_newVars.root", "READ");
    TTree* treedata = (TTree*)filedata->Get("Bs2phimumuTuple/DecayTree");


    TCanvas* canvas = new TCanvas("testcanvas", "title", 800, 600);
    //TH1D hist = TH1D("mod_hist", "titlehist", 200, -5, 10);

    
    
    //treedata -> Draw("Kplus_PIDK - Kplus_PIDp");
    //std::cout << treedata -> GetHistogram() -> GetNbinsX();

    treedata -> Draw("Kplus_PIDK-Kplus_PIDp>>test(100,-370,630)", "", "");
   
    
    canvas -> SaveAs("./test1.png");

    //treedata -> Draw("sqrt(Kplus_PIDK)>>wurst(200, 0, 20)", "", "");
    //canvas -> SaveAs("./test2.png");
        
    //treedata -> Draw("Kplus_PIDK>>test2", "", "");
        
       // TH1D* htemp;
      //  htemp = (TH1D*)tree_ -> GetHistogram();
     //   htemp->


    //canvas -> SaveAs("./test2.png");

    return 0;

}
