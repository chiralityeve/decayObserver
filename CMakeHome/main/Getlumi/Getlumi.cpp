#include <iostream>
#include <string>
#include "math.h"
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TPaveStats.h>
#include <TGaxis.h>


using std::cout;
using std::endl;

int main(int argc, const char* argv[]) {

    if(argc != 4) {
        std::cout << "3 options required (path to Root-file, path to TTree in the file and Branchname)" << std::endl;
        return 1;
    }
    
    std::string rootfile = argv[1];
    std::string treepath = argv[2];
    std::string branchname = argv[3];


    //    open the file
    TFile* file = TFile::Open(rootfile.c_str());
    if (file == 0) {
        // if we cannot open the file, print an error message and return immediatly
        printf("Error: cannot open the Root-file");
        return 1;
    }
    // get a pointer to the tree
    TTree *tree = dynamic_cast<TTree*>(file->Get(treepath.c_str()));



    Long64_t nentries = tree->GetEntries();


    TBranch *branch = tree->GetBranch(branchname.c_str());          //Program has to be extended if there will be other BrancName in the future
    

    double lumi_ges = 0;
    double lumi_temp;


    branch->SetAddress(&lumi_temp);
    

    for(int i = 0; i < nentries; i++) {
        branch->GetEvent(i);

        lumi_ges += lumi_temp;
    }

    double lumi_ges_femto = lumi_ges/1000;

    cout << endl << "Luminosity(inverse picobarn): " << lumi_ges << endl;
    cout << "Luminosity(inverse femtobarn): " << lumi_ges_femto << endl << endl;

    return 0;
}

