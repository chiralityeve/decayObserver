#include <iostream>
#include <string>
#include "math.h"
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include "Plotvariable.h"
#include "tree2hist.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------
// This Function creates a histogram from a TTree for a specified variable, plots it onto the current TCanvas and returns pointer to the histogram
// --------------------------------------------------------------------------------------------------------------------------------------------------

TH1D* tree2hist(TTree* tree, std::string branch, std::string histtitel, int nbins, double xlow, double xup,
        std::string cuts /*= "1"*/, Color_t color /*= kBlue*/, Style_t style /*= 1*/, std::string options /*= ""*/) {

    static int globalidnr = 0;
    std::string histid = "histogram_" +  std::to_string(globalidnr);                                    //Histogram needs ID => get it from globalidnr and increase globalidnr by 1
    globalidnr += 1;
                                                                                        
    std::string klammer_auf = "(";                                                     //Prepare strings so that it can be read from TTree
    std::string klammer_zu  = ")";
    std::string arrows      = " >> ";
    std::string komma       = ",";
    std::string str_nbins   = std::to_string(nbins);
    std::string str_xlow    = std::to_string(xlow);
    std::string str_xup     = std::to_string(xup);  

    std::string str_tree2hist = branch+arrows+histid+klammer_auf+str_nbins+komma+str_xlow+komma+str_xup+klammer_zu;   //Add prepared strings together

    tree->Draw(str_tree2hist.c_str(), cuts.c_str(), options.c_str());                                                //Write Branch on tree
    
    TH1D* htemp;                                                                                                    //Edit color and style of histogram
    htemp = (TH1D*)tree->GetHistogram();
    htemp->SetTitle(histtitel.c_str());
    htemp->SetLineColor(color);
    htemp->SetLineStyle(style);
    
    
    return htemp;
}


// -----------------------------------
// Version with Plotvariable as argument
// -----------------------------------

TH1D* tree2hist(TTree* tree, Plotvariable plotvariable, Color_t color /*= kBlue*/, Style_t style /*= 1*/, std::string options /*= ""*/) {

    static int globalidnr = 0;
    std::string histid = "histogramplot_" +  std::to_string(globalidnr);                                    //Histogram needs ID => get it from globalidnr and increase globalid
    globalidnr += 1;
                                                                                        
    //Prepare strings so that branch can be read from TTree
    std::string str_nbins   = std::to_string(plotvariable.Getnbins());
    std::string str_xlow    = std::to_string(plotvariable.Getxlow());
    std::string str_xup     = std::to_string(plotvariable.Getxup());  
    std::string cuts        = plotvariable.Getcuts();
    
    std::string str_tree2hist = plotvariable.Getname() + " >> " +histid+ "(" + str_nbins + "," + str_xlow + "," +str_xup+ ")";   //Add prepared strings together

    tree->Draw(str_tree2hist.c_str(), cuts.c_str(), options.c_str());                                                //Write Branch on tree
    
    TH1D* htemp;                                                                                                    //Edit color and style of histogram
    htemp = (TH1D*)tree->GetHistogram();
    std::string histtitel = plotvariable.Gettitle();
    htemp->SetTitle(histtitel.c_str());
    htemp->SetLineColor(color);
    htemp->SetLineStyle(style);
    
    
    return htemp;
}



