#ifndef TREE2HIST_H
#define TREE2HIST_H

#include "Plotvariable.h"


TH1D* tree2hist(TTree* tree, std::string branch, std::string histtitel, int nbins, double xlow, double xup, std::string cuts = "1", Color_t color = kBlue,  Style_t style = 1,  std::string options = "");


TH1D* tree2hist(TTree* tree, Plotvariable plotvariable, Color_t color = kBlue, Style_t style = 1, std::string options = "");   //Version with Plotvariable (ownclass)



#endif //TREE2HIST_H
