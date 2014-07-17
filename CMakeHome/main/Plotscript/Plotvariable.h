#ifndef PLOTVARIABLE_H
#define PLOTVARIABLE_H

#include <vector>
#include <string>
#include <TTree.h>

// -----------------------------------
// Create Class for plotvariable
// -----------------------------------

class Plotvariable {
    public:
        //Constructor: Name = Branchname in TTree. Title = Title on canvas. After creation, a pointer to the Plotvariable will be pushed to the vector in the first argument
        Plotvariable(std::string name, TTree* tree, std::string title, std::string legendname, int nbins, double xlow, double xup, std::string xlabel, std::string unit, std::vector<Plotvariable*> *Plotvector, std::string options = "");                 //no cuts
        Plotvariable(std::string name, TTree* tree, std::string title, std::string legendname, int nbins, double xlow, double xup, std::string xlabel, std::string unit, std::string cuts, std::vector<Plotvariable*> *Plotvector, std::string options = "");  //with cuts


        //Constructor for Plotvariable which shall be plotted on the same canvas as the last one (the title is here equal to the legendname)
        Plotvariable(std::string name, TTree* tree, std::string title, std::vector<Plotvariable*> *Plotvector);                      //no cuts
        Plotvariable(std::string name, TTree* tree, std::string title, std::string cuts, std::vector<Plotvariable*> *Plotvector);    //with cuts
        
        std::string Getname();                          //Get variable
        std::string Gettitle();
        std::string Getlegendname();
        int Getnbins();
        double Getxlow();
        double Getxup();
        std::string Getcuts();
        std::string Getoptions();

        bool Getsamecanvas();

        TH1D* plot(Color_t color = kBlue, Style_t style = 1, std::string options = "");
    
    
    private:
        std::string name_;           //Name in TTree
         TTree* tree_;               //TTree where the variable comes from
        std::string title_;          //Title as to be shown on the TCanvas
        std::string legendname_;     //Name to be shown in the legend
        int nbins_;                  //number of bins 
        double xlow_;                //lower limit of x-axis range
        double xup_;                 //upper limit of x-axis range
        std::string xlabel_;         //X-label (Plotted variable?)
        std::string unit_;           //Unit of the variable
        std::string ylabel_;         //Y-label
        std::string cuts_;           //cuts (if applicable)
        bool show_on_same_canvas_;
        std::string options_;        //Options (eg. logy, logx etc.)

};





#endif //PLOTVARIABLE_H
