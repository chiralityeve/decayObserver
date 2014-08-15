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

int main() {

    //       ----------------------
    //       Open File
    //       ----------------------

    TFile* file = TFile::Open("/afs/cern.ch/work/d/dberning/private/Pruned/MC_Truthmatched/MC_merged_pruned_truthmatched.root");
    if (file == 0) {
        // if we cannot open the file, print an error message and return immediatly
        // printf("Error: cannot open
        //       http://lcg-heppkg.web.cern.ch/lcg-heppkg/ROOT/eventdata.root!\n");
        return 1;
    }


    

    // get a pointer to the tree
    TTree *tree = dynamic_cast<TTree*>(file->Get("Bs2phimumuTuple/DecayTree"));


    




    // ----------------------
    // Effizienzen berechnen
    // ----------------------
    
    std::string preselection = "phi_1020_M < 1800 && phi_1020_M  > 1300 && Kplus_PIDK > -3 && Kminus_PIDK > -3";
    
    std::string cutminus3 = "(Kplus_PIDK - Kplus_PIDp) > -3 && (Kminus_PIDK - Kminus_PIDp) > -3";
    std::string cutplus0 = "(Kplus_PIDK - Kplus_PIDp) > 0 && (Kminus_PIDK - Kminus_PIDp) > 0";

    double n_events = tree -> GetEntries();
    double n_minus3 = tree -> GetEntries(cutminus3.c_str());
    double n_plus0 = tree -> GetEntries(cutplus0.c_str());

  
    cout <<"Events vor allen Schnitten:  " << n_events << endl;
    cout <<"Events nach Kaon_DLL(K-p) > -3: " << n_minus3 << "\tEffizienz:" << n_minus3 / n_events << endl;
    cout <<"Events nach Kaon_DLL(K-p) > 0: " << n_plus0 << "\tEffizienz:" << n_plus0 / n_events << endl;



    //Efficiency after Preselection

    cutminus3 += "&&" + preselection;
    cutplus0 += "&&" + preselection;

    n_events = tree -> GetEntries(preselection.c_str());
    n_minus3 = tree -> GetEntries(cutminus3.c_str());
    n_plus0 = tree -> GetEntries(cutplus0.c_str());


    cout << endl << "Das ganze schon mit Preselection vorher:" << endl;
    cout <<"Events vor allen Schnitten:  " << n_events << endl;
    cout <<"Events nach Kaon_DLL(K-p) > -3: " << n_minus3 << "\tEffizienz:" << n_minus3 / n_events << endl;
    cout <<"Events nach Kaon_DLL(K-p) > 0: " << n_plus0 << "\tEffizienz:" << n_plus0 / n_events << endl;
   

   }

