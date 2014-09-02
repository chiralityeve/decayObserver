#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include "math.h"
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <vector>
#include <cstdlib>
#include "Plotvariable.h"
#include "tree2hist.h"
#include <algorithm>

#include <ctime>
#include <cstdlib>





// -----------------------------------
// Implementation of Plotvariable
// -----------------------------------





//--------------------------------------------------------------------------------------->
//Constructor of Plotvariable which shall be plotted on a new canvas
Plotvariable::Plotvariable(std::string name, TTree* tree,  std::string title, std::string legendname, int nbins, double xlow, double xup, std::string xlabel, std::string unit, std::vector<Plotvariable*> *Plotvector, std::string options /*= "" */)                   //Variant without cuts
    : name_(name), tree_(tree), title_(title), legendname_(legendname), nbins_(nbins), xlow_(xlow), xup_(xup), xlabel_(xlabel), unit_(unit), cuts_(""), show_on_same_canvas_(false), options_(options) { 
        savename_ = name;
        savename_.erase(std::remove(savename_.begin(), savename_.end(), '/'), savename_.end());        //Delete nasty letters
        savename_.erase(std::remove(savename_.begin(), savename_.end(), ':'), savename_.end());
        savename_.erase(std::remove(savename_.begin(), savename_.end(), ' '), savename_.end());

        Plotvector-> push_back(this); 
    }

Plotvariable::Plotvariable(std::string name, TTree* tree, std::string title, std::string legendname, int nbins, double xlow, double xup, std::string xlabel, std::string unit,  std::string cuts, std::vector<Plotvariable*> *Plotvector, std::string options /*= "" */)  //Variant with cuts
    : name_(name), tree_(tree), title_(title), legendname_(legendname), nbins_(nbins), xlow_(xlow), xup_(xup), xlabel_(xlabel), unit_(unit), cuts_(cuts), show_on_same_canvas_(false), options_(options) {
        savename_ = name;
        savename_.erase(std::remove(savename_.begin(), savename_.end(), '/'), savename_.end());
        savename_.erase(std::remove(savename_.begin(), savename_.end(), ':'), savename_.end());
        savename_.erase(std::remove(savename_.begin(), savename_.end(), ' '), savename_.end());
        
        Plotvector -> push_back(this);
    }  






//--------------------------------------------------------------------------------------->
//Constructor of Plotvariable which shall be plotted on the same canvas as the previous one
Plotvariable::Plotvariable(std::string name, TTree* tree,  std::string legendname, std::vector<Plotvariable*> *Plotvector)                   //Variant without cuts
    : name_(name), tree_(tree), legendname_(legendname), cuts_(""), show_on_same_canvas_(true) {
        savename_ = name;
        savename_.erase(std::remove(savename_.begin(), savename_.end(), '/'), savename_.end());
        savename_.erase(std::remove(savename_.begin(), savename_.end(), ':'), savename_.end());
        savename_.erase(std::remove(savename_.begin(), savename_.end(), ' '), savename_.end());
        
        //get nbins, xlow and xup from previous Plotvariable in the vector
        Plotvariable* motherpv = Plotvector -> back();
        nbins_ = motherpv -> Getnbins();
        xlow_ = motherpv -> Getxlow();
        xup_ = motherpv -> Getxup();
        options_ = motherpv -> Getoptions();
        xlabel_ = motherpv -> Getxlabel();
        unit_ = motherpv -> Getunit();
        title_ = motherpv -> Gettitle();
        title_ = title_ + " | " + legendname;

        Plotvector-> push_back(this); 
    }

Plotvariable::Plotvariable(std::string name, TTree* tree, std::string legendname, std::string cuts, std::vector<Plotvariable*> *Plotvector)  //Variant with cuts
    : name_(name), tree_(tree), legendname_(legendname), cuts_(cuts), show_on_same_canvas_("true") {
        savename_ = name;
        savename_.erase(std::remove(savename_.begin(), savename_.end(), '/'), savename_.end());
        savename_.erase(std::remove(savename_.begin(), savename_.end(), ':'), savename_.end());
        savename_.erase(std::remove(savename_.begin(), savename_.end(), ' '), savename_.end());
        

        //get nbins, xlow and xup from previous Plotvariable in the vector
        Plotvariable* motherpv = Plotvector -> back();
        nbins_ = motherpv -> Getnbins();
        xlow_ = motherpv -> Getxlow();
        xup_ = motherpv -> Getxup();
        options_ = motherpv -> Getoptions();
        xlabel_ = motherpv -> Getxlabel();
        unit_ = motherpv -> Getunit();
        title_ = motherpv -> Gettitle();
        


        Plotvector-> push_back(this); 
    }  








//--------------------------------------------------------------------------------------->
//Member-Functions
std::string Plotvariable::Getname() { return name_; }
std::string Plotvariable::Getsavename() { return savename_; }
std::string Plotvariable::Gettitle() { return title_; }
std::string Plotvariable::Getlegendname() { return legendname_; }
int Plotvariable::Getnbins() { return nbins_; }
double Plotvariable::Getxlow() { return xlow_; }
double Plotvariable::Getxup() { return xup_; }
std::string Plotvariable::Getxlabel() { return xlabel_; }
std::string Plotvariable::Getunit() { return unit_; }
std::string Plotvariable::Getcuts() { return cuts_; }
bool Plotvariable::Getsamecanvas() { return show_on_same_canvas_; }
std::string Plotvariable::Getoptions() { return options_; }




// Plotfunction
TH1D* Plotvariable::plot(Color_t color /*= kBlue*/, Style_t style/* = 1*/, std::string options/* = ""*/) {

    clock_t clocktime = clock();
    std::stringstream ss;
    int randnr = rand() % 100;
    ss << savename_ << "_" << clocktime << "_" << randnr;
    std::string histid;
    histid = ss.str();



    /*
    static int globalidnr = 0;
    std::string histid = "Plotvariablehist_" +  std::to_string(globalidnr);                              //Histogram needs ID => get it from globalidnr and increase globalid
    globalidnr += 1;
    */

    //Prepare strings so that branch can be read from TTree
    std::string str_nbins   = std::to_string(nbins_);
    std::string str_xlow    = std::to_string(xlow_);
    std::string str_xup     = std::to_string(xup_);  
    std::string cuts        = cuts_;

    std::string str_tree2hist = name_ + " >> " +histid+ "(" + str_nbins + "," + str_xlow + "," +str_xup+ ")";   //Add prepared strings together

    tree_->Draw(str_tree2hist.c_str(), cuts.c_str(), options.c_str());                                                //Write Branch from tree

    TH1D* htemp;                                                                                                    //Edit color and style of histogram
    htemp = (TH1D*)tree_->GetHistogram();
    std::string histtitel = title_;
    htemp->SetTitle(histtitel.c_str());
    htemp->SetLineColor(color);
    htemp->SetLineStyle(style);

    //Edit axis labels
    double binwidth = (xup_ - xlow_) / nbins_;
    std::ostringstream out;                               //Set precision of binwidth
    out << std::setprecision(2) << binwidth;
    std::string str_binwidth = out.str();
    
    std::string xlabel, ylabel;

    if(unit_ != "" && unit_ != " ") {
        xlabel =  xlabel_ + " (" + unit_ + ")";
        ylabel = "Events / (" + str_binwidth + " " + unit_ + ")";
    }
    else {
        xlabel = xlabel_;
        ylabel = "Events / (" + str_binwidth + ")";
    }



    
    htemp -> GetXaxis() -> SetTitle(xlabel.c_str());
    htemp -> GetXaxis() -> SetTitleOffset(1.18);

    htemp -> GetYaxis() -> SetTitle(ylabel.c_str());
    htemp -> GetYaxis() -> SetTitleOffset(1.48);

    return htemp;
}


//------------------------------------
