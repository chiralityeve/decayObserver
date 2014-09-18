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
    TFile* file = TFile::Open("/afs/cern.ch/work/d/dberning/private/Sweighting/PIDcomparison/Data_resonant_preselected_newVars_sweights.root");
    if (file == 0) {
        // if we cannot open the file, print an error message and return immediatly
        // printf("Error: cannot open
        //       http://lcg-heppkg.web.cern.ch/lcg-heppkg/ROOT/eventdata.root!\n");
        return 1;
    }

    // get a pointer to the truthmatched Tree
    TTree *tree = dynamic_cast<TTree*>(file->Get("DecayTree"));

    double sig0, sig1, bkg0, sum;










tree->GetBranch("sig0_yield_sw")->SetAddress(&sig0);
tree->GetBranch("sig1_yield_sw")->SetAddress(&sig1);
tree->GetBranch("bkg0_yield_sw")->SetAddress(&bkg0);


for(int i=0; i < 100; i++) {
    tree -> GetEntry(i);
    sum = sig0 + sig1 + bkg0;
    std::cout << sig0 << " " << sig1 << " " << bkg0 << " " << sum << std::endl;

}



 return 0;
 }
   
