#include <iostream>
#include <string>
#include <algorithm>
#include "math.h"
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2D.h>
#include <TPaveStats.h>
#include <TGaxis.h>
#include <TChain.h>
#include <TLegend.h>
#include <TStyle.h>
#include <vector>
#include "Plotvariable.h"
#include "Plotvariable_2D.h"
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



    TLegend legend(0.7, 0.75, 0.89, 0.89);               //Create legend
    TLegend* legendp = &legend;
    legendp -> SetFillColor(kWhite);
    legendp -> Draw();
    legendp -> SetBorderSize(4);    


    TH1D motherhist;                                  
    TH1D* motherhistp = &motherhist;                  //Pointer to the first histogram in the canvas (which inherits several properties)

    TH1D temphist;                                     //temporary histogram (for daughter histograms = histograms plotted on same canvas as motherhist)
    TH1D* temphistp = &temphist;

    Plotvariable_2D* plotv_2Dtemp;
    TH2D* hist_2D;                                       

    unsigned int vector_size = vecp -> size();


    double maxbincontent = 0;                           //maxbincontent (needed if more plots are made in the same canvas) 
    double minbincontent = 0;
    unsigned int nplots = 0;                            //number of plots on the same canvas (counter)

    std::string savepath;
    
    unsigned int savepathnr = 0;
    std::string str_savepathnr;
    std::string savename;

    int pdfpage = 0;                           //number of pages in pdf

    std::string pdfname;
    if(vector_size > 0) pdfname = saveto + (vec[0] -> Getsavename()) + "_etc.pdf"; 
    else {
        std::cout << "No histograms to plot" << std::endl;
        return 1;
    }

    std::string norm, normsame;                                     //Prepare norm(same) strings (plotoptions)
    std::string defnorm = "";
    std::string defnormsame = "same";
    if(normalized_plots) {
        defnorm = "norm";
        defnormsame = "normsame";
    }



    //Sebastians Code (save all created plots into a .root file)
    std::string rootname = saveto + (vec[0] -> Getname()) + "_etc.root";
    TFile* outfile = TFile::Open(rootname.c_str(),"RECREATE");
    std::string temptitle_before, temptitle_root;
    //---------------------------------------------------------



    cp -> SaveAs((pdfname + "[").c_str());                          //Begin plotting on pdf
    std::cout << "Info in <TCanvas::Print>: pdf file opened" << std::endl << std::endl;

    //Plotting for 1D - Histograms
    for(unsigned int i = 0; i < vector_size; i++) {
        if((vec[i] -> Getsamecanvas()) == false) {                  //Plot it on a new canvas
            //Plot plots also in a single PDF-File
            if(pdfpage > 0) {
                cp -> SaveAs(savepath.c_str());
                cp -> SaveAs(pdfname.c_str());



            }
            pdfpage += 1;                                           //Note: Plot of last canvas is done after this loops


            maxbincontent = 0;                                      //Reset counters

            nplots = 0;
            savepathnr += 1;
            str_savepathnr = std::to_string(savepathnr);
            legendp -> Clear();                                     //Clear legend in case of new canvas

            norm = defnorm;                                         //Restore default settings for norm(same)
            normsame = defnormsame;

            savepath = saveto + str_savepathnr + "_" + vec[i]->Getsavename() + ".png";          //note: savepath is used within the next loop iteration
            

            //Only things that need to be done for 1-D histograms
            if((vec[i] -> Getname()).find(":") == std::string::npos) {
                if((vec[i] -> Getoptions()).find("norm") != std::string::npos) {        //Find if canvas should be normalised
                    norm = "norm";
                    normsame = "normsame";
                }

                motherhistp = vec[i] -> plot(kBlue, 1, norm); 
                maxbincontent = motherhistp -> GetMaximum();
                
                //Check if it is a normal histogram which means bin contents of 0 or above
                minbincontent = motherhistp -> GetMinimum();
                if(minbincontent < 0) motherhistp -> SetMinimum(minbincontent + minbincontent/10);
                else motherhistp -> SetMinimum(0);

                //Set logy or logx Scale
                if((vec[i] -> Getoptions()).find("logx") != std::string::npos) cp -> SetLogx(1);
                else cp -> SetLogx(0);
                if((vec[i] -> Getoptions()).find("logy") != std::string::npos) {
                    motherhistp -> SetMinimum(0.5);
                    cp -> SetLogy(1);
                }
                else cp -> SetLogy(0);


                //Save to ROOT-File (therefore add to title the legendname for distinguishing purposes)
                if(vec[i]->Getlegendname() != "") {
                    temptitle_before = motherhistp->GetTitle();
                    temptitle_root = temptitle_before + " | " + vec[i] -> Getlegendname();
                    motherhistp-> SetTitle(temptitle_root.c_str());
                    motherhistp -> Write();      
                    std::cout << "Info in <TCanvas::Print>: Current histogram "  << vec[i] -> Getsavename() << " added to .ROOT-File" << std::endl;
                    motherhistp -> SetTitle(temptitle_before.c_str());
                }
                else { 
                    motherhistp -> Write();
                    std::cout << "Info in <TCanvas::Print>: Current histogram "  << vec[i] -> Getsavename() << " added to .ROOT-File" << std::endl;
                }
            }       //end: things to be done for 1-D histograms
            else {  //things to be done for 2-D histograms
                cp -> SetRightMargin(0.13);

                plotv_2Dtemp = (Plotvariable_2D*)vec[i];        //Cast vec[i] to pointer to type Plotvariable_2D so that right method is used for plot();

                hist_2D = (TH2D*)plotv_2Dtemp -> plot();


                savepath = saveto + str_savepathnr + "_" + vec[i]->Getsavename() + ".png";
                hist_2D -> Write();
                std::cout << "Info in <TCanvas::Print>: Current 2D-histogram " << vec[i] -> Getsavename() << " added to .ROOT-File" << std::endl;
            }       //end: things to be done for 2-D histograms



        }
        else{                                                                                       //Plot it on the same canvas  (only relevant for 1-D histos)
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

            //Adjust the y-Scale according to the histograms in the Canvas
            if(maxbincontent < temphistp -> GetBinContent(temphistp->GetMaximumBin())) {
                maxbincontent = temphistp -> GetMaximum();
                motherhistp -> SetMaximum(maxbincontent + maxbincontent/10);
            }
            
            //minbincontent
            if(minbincontent > temphistp -> GetMinimum()) {
                minbincontent = temphistp -> GetMinimum();
                if(minbincontent < 0) motherhistp -> SetMinimum(minbincontent + minbincontent/10);
            }
    



            legendp->Draw();

            //Save to ROOT-File (adding legendname to the title before)
            temptitle_before = temphistp->GetTitle();
            temptitle_root = temptitle_before + " | " + vec[i] -> Getlegendname();
            temphistp-> SetTitle(temptitle_root.c_str());
            temphistp -> Write();      
            std::cout << "Info in <TCanvas::Print>: Current histogram "  << vec[i] -> Getsavename() << "_subplot" << nplots << " added to .ROOT-File" << std::endl;
            temphistp -> SetTitle(temptitle_before.c_str());
            
        }
    }
    cp -> SaveAs(savepath.c_str());                                                                 //Plot last canvas (.PNG)
    
    cp -> SaveAs(pdfname.c_str());                                                                  //Plot last canvas (.PDF)




    std::cout << std:: endl <<"Info in <TCanvas::Print>: Close pdf file:" << std::endl;



    cp -> SaveAs( (pdfname + "]").c_str());                                                         //finish PDF plotting




    outfile->Close();           //Close ROOT-File








    return 0;
}

