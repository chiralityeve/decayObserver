#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include "math.h"
#include <TTree.h>
#include <TH1D.h>
#include <TH1.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <vector>
#include <cstdlib>
#include "Plotvariable.h"
#include "Plotvariable_2D.h"
//#include "tree2hist.h"
#include <algorithm>

#include <ctime>
#include <cstdlib>





// -----------------------------------
// Implementation of Plotvariable
// -----------------------------------





//--------------------------------------------------------------------------------------->
//2D Konstruktor without CUTS
Plotvariable_2D::Plotvariable_2D(std::string name, TTree* tree, std::string title, int nbins, double xlow, double xup, double xlow_2, double xup_2, std::string xlabel, std::string unit, std::string ylabel, std::string y_unit, std::vector<Plotvariable*> *Plotvector, std::string options /*= ""*/) : Plotvariable(name, tree, title, "", nbins, xlow, xup, xlabel, unit, Plotvector, options),  xlow_2_(xlow_2), xup_2_(xup_2), ylabel_(ylabel), y_unit_(y_unit) { }
 

//2D Konstruktor with CUTS
Plotvariable_2D::Plotvariable_2D(std::string name, TTree* tree, std::string title, int nbins, double xlow, double xup, double xlow_2, double xup_2, std::string xlabel, std::string unit, std::string ylabel, std::string y_unit, std::string cuts, std::vector<Plotvariable*> *Plotvector, std::string options /*= ""*/) : Plotvariable(name, tree, title, "", nbins, xlow, xup, xlabel, unit, cuts, Plotvector, options), xlow_2_(xlow_2), xup_2_(xup_2), ylabel_(ylabel), y_unit_(y_unit) { }








//--------------------------------------------------------------------------------------->
//Member-Functions


double Plotvariable_2D::Getxlow_2() { return xlow_2_; }
double Plotvariable_2D::Getxup_2() { return xup_2_; }
std::string Plotvariable_2D::Getylabel() { return ylabel_; }
std::string Plotvariable_2D::Gety_unit() { return y_unit_; }




// Plotfunction
TH1* Plotvariable_2D::plot() {

    clock_t clocktime = clock();
    std::stringstream ss;
    int randnr = rand() % 100;
    ss << savename_ << "_2D_" << clocktime << "_" << randnr;
    std::string histid;
    histid = ss.str();


    TH2D* htemp = new TH2D(histid.c_str(), title_.c_str(), nbins_, xlow_, xup_, nbins_, xlow_2_, xup_2_);

    std::string str_tree2hist = name_ + " >> " + histid;   //Add prepared strings together

    tree_-> Draw(str_tree2hist.c_str(), cuts_.c_str(), options_.c_str());                                                //Write Branch from tree
    
                                                                                                    //Edit color and style of histogram
    //htemp = (TH1D*)tree_->GetHistogram();
    //std::string histtitel = title_;
    //htemp->SetTitle(histtitel.c_str());
    //htemp->SetLineColor(color);
    //htemp->SetLineStyle(style);

    
   
    
    
    
    
    std::string xlabel, ylabel, xlabel_2, ylabel_2;

    if(unit_ != "") {
        xlabel =  xlabel_ + " (" + unit_ + ")"; 
    }
    else {
        xlabel = xlabel_;
    }

    if(y_unit_ != "") {
        ylabel =  ylabel_ + " (" + y_unit_ + ")"; 
    }
    else {
        ylabel = ylabel_;
    }

    
    htemp -> GetXaxis() -> SetTitle(xlabel.c_str());
    htemp -> GetXaxis() -> SetTitleOffset(1.18);

    htemp -> GetYaxis() -> SetTitle(ylabel.c_str());
    htemp -> GetYaxis() -> SetTitleOffset(1.48);

    TH1* htemp_return = (TH1*)htemp;


    
    
    return htemp_return;

    
}


//------------------------------------
