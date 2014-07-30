#ifndef PLOTVARIABLE_2D_H
#define PLOTVARIABLE_2D_H

#include <vector>
#include <string>
#include <TTree.h>
#include <Plotvariable.h>

// -----------------------------------
// Create Class for plotvariable
// -----------------------------------

class Plotvariable_2D : public Plotvariable{
    public:
        //Constructor: Name = Branchname in TTree. Title = Title on canvas. After creation, a pointer to the Plotvariable will be pushed to the vector in the first argument
        Plotvariable_2D(std::string name, TTree* tree, std::string title, int nbins, double xlow, double xup, double xlow_2, double xup_2, std::string xlabel, std::string unit, std::string ylabel, std::string y_unit, std::vector<Plotvariable_2D*> *Plotvector, std::string options = "");                 //no cuts

        Plotvariable_2D(std::string name, TTree* tree, std::string title, int nbins, double xlow, double xup, double xlow_2, double xup_2, std::string xlabel, std::string unit, std::string ylabel, std::string y_unit, std::string cuts, std::vector<Plotvariable_2D*> *Plotvector, std::string options = "");                 //with cuts

        
        double Getxlow_2();
        double Getxup_2();
        std::string Getylabel();
        std::string Gety_unit();

        TH1* plot();
    
    
    protected:
        double xlow_2_;
        double xup_2_;
        std::string ylabel_;
        std::string y_unit_;

};





#endif //PLOTVARIABLE_2D_H
