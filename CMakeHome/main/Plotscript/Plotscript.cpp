#include <iostream>
#include <string>
#include <algorithm>
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
#include <vector>
#include "Plotvariable.h"
#include "tree2hist.h"


using std::cout;
using std::endl;




// --------------------------------------------------------
// This Script creates Histograms from TTree - Variables
// --------------------------------------------------------

//Define setting-functions
void daniel_current(std::vector<Plotvariable*> *vecp, bool &normalized_plots, int &nbins, std::string &saveto);
void kevin_current(std::vector<Plotvariable*> *vecp, bool &normalized_plots, int &nbins, std::string &saveto);
void vitali_current(std::vector<Plotvariable*> *vecp, bool &normalized_plots, int &nbins, std::string &saveto);


int main(int argc, char **argv) {
    gStyle->SetOptStat(0);          //Do not plot infobox in histogram

    
    std::vector<Plotvariable*> vec;
    std::vector<Plotvariable*> *vecp = &vec;

     bool normalized_plots;
    int nbins;
    std::string saveto;

    if(argc == 1) {
        cout << endl << "No option given" << endl;
        return 1;
    }


    //Settings are loaded here (depending on the main parameter given)
    std::string compare = argv[1];
    if(compare == "daniel") daniel_current(vecp, normalized_plots, nbins, saveto);
    else if(compare == "kevin") kevin_current(vecp, normalized_plots, nbins, saveto);
    else if(compare == "vitali") vitali_current(vecp, normalized_plots, nbins, saveto);
    else{
        cout << endl << "Wrong option given" << endl;
        return 1;
    }





    // -----------------------------------
    // Plotprocess
    // -----------------------------------

    TCanvas c("canvas", "dasvancas", 800, 600);         //Create canvas
    TCanvas* cp = &c;

    TLegend legend(0.8, 0.85, 0.99, 0.99);               //Create legend
    TLegend* legendp = &legend;
    legendp -> SetFillColor(kWhite);
    legendp -> Draw();


    TH1D motherhist;                                  
    TH1D* motherhistp = &motherhist;                  //Pointer to the first histogram in the canvas (which inherits several properties)

    TH1D temphist;                                     //temporary histogram
    TH1D* temphistp = &temphist;

    unsigned int vector_size = vecp -> size();

    double maxbincontent = 0;                           //maxbincontent (needed if more plots are made in the same canvas)
    unsigned int nplots = 0;                            //number of plots on the same canvas (counter)

    std::string savepath;
    unsigned int savepathnr = 0;
    std::string str_savepathnr;
    std::string savename;

    int pdfpage = 0;                           //number of pages in pdf
    std::string pdfname = saveto + (vec[0] -> Getname()) + "_etc.pdf";

    std::string norm = "";
    std::string normsame = "same";
    std::string normierte = "";
    if(normalized_plots) {
        norm = "norm";
        normsame = "normsame";
        normierte = "Normierte";
    }

    cp -> SaveAs((pdfname + "[").c_str());                          //Begin plotting on pdf

    for(unsigned int i = 0; i < vector_size; i++) {
        if((vec[i] -> Getsamecanvas()) == false) {                  //Plot it on a new canvas
            //Plot plots also in a single PDF-File
            if(pdfpage > 0) cp -> SaveAs(pdfname.c_str());
            pdfpage += 1;                                           //Note: Plot of last canvas is done after this loops
            
            
            maxbincontent = 0;                                      //Reset counters
            nplots = 0;
            savepathnr += 1;
            str_savepathnr = std::to_string(savepathnr);
            legendp -> Clear();                                     //Clear legend in case of new canvas

            motherhistp = vec[i] -> plot(kBlue, 1, norm); 
            maxbincontent = motherhistp -> GetMaximum();

            

            savename = vec[i] -> Getname();                                                                 //Remove illegal characters in savename
            savename.erase(std::remove(savename.begin(), savename.end(), '/'), savename.end());
            savename.erase(std::remove(savename.begin(), savename.end(), ':'), savename.end());
            savename.erase(std::remove(savename.begin(), savename.end(), ' '), savename.end());
            
            savepath = saveto + str_savepathnr + "_" + savename + ".png";

            cp -> SaveAs(savepath.c_str());


        }
        else{                                                                                       //Plot it on the same canvas  
            nplots += 1;                                                                            //(number+1) of plots on same canvas (raise by one)

            if(nplots == 1) {
                temphistp = vec[i] -> plot(kRed, 1, normsame);                                      //<----------Edit the layout here
                legendp -> AddEntry(motherhistp, vec[i-1]->Getlegendname().c_str(), "l");
                legendp -> AddEntry(temphistp, vec[i]->Getlegendname().c_str(), "l");
            }
            if(nplots == 2) {
                temphistp = vec[i] -> plot(kGreen+2, 1, normsame);
                legendp -> AddEntry(temphistp, vec[i]->Getlegendname().c_str(), "l");
            }
            if(nplots > 2) {
                temphistp = vec[i] -> plot(kOrange, 1, normsame);
                legendp -> AddEntry(temphistp, vec[i]->Getlegendname().c_str(), "l");
            }

            if(maxbincontent < temphistp -> GetBinContent(temphistp->GetMaximumBin())) {
                maxbincontent = temphistp -> GetMaximum();
                motherhistp -> SetMaximum(maxbincontent + maxbincontent/10);
            }

            legendp->Draw();
            cp -> SaveAs(savepath.c_str());
        }
    }
    cp -> SaveAs(pdfname.c_str());                                                         //Plot last canvas
    cp -> SaveAs( (pdfname + "]").c_str());                                                         //finish PDF plotting










    return 0;
}

