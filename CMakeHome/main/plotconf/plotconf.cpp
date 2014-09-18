#include <iostream>
#include <string>
#include <algorithm>
#include "math.h"
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TPaveStats.h>
#include <TGaxis.h>
#include <TChain.h>
#include <TLegend.h>
#include <TStyle.h>
#include <vector>
#include "Plotvariable.h"
#include "Plotvariable_2D.h"

#include <fstream>
#include <sstream>
#include <map>
#include <functional>
#include "ArgParser.h"



using std::cout;
using std::endl;
using std::string;
using std::map;

// --------------------------------------------------------
// This Script creates Histograms from TTree - Variables
// --------------------------------------------------------



struct VariableInfo{
    string name;
    string fromtree;
    string title;
    string legend;
    int nbins;
    double range_min;
    double range_max;
    string axislabel;
    string unit;
    string cuts;
    string options;
    
    //for 2D:
    double yrange_min;
    double yrange_max;
    string yaxislabel;
    string yunit;

    int type;  //Types: 0 = Plot on new canvas, 1 = Plot on same canvas, 2 = 2D-plot
};




//Forward declaration of ReadConfigfile
int ReadConfigurationFile(const string &filename, string& outputFilename, vector<VariableInfo>& variablevector, map<string, TChain*>& treemap, map<string, string>&cutmap, 
        int& enable_infobox);




//Main-function
int main(int argc, char **argv) {
    int enable_infobox = 0;
    

    std::vector<Plotvariable*> vec;
    std::vector<Plotvariable*> *vecp = &vec;

    bool normalized_plots;
    std::string saveto;

    if(argc == 1) {
        cout << endl << "Please give the config file" << endl;
        return 1;
    }

    //Settings are loaded here (depending on the main parameter given)
    std::string configFilename = argv[1];


    vector<VariableInfo> variablevector;    //Vector that saves the infos for the variables to plot
    map<string, TChain*> treemap;           //Map that saves the used trees with a given name
    map<string, string> cutmap;              //Map that has the defined cuts in it with a given name
    



    string outputFilename;

    //Read Configfile here: variablevector is filled, same as inputFilename, decayChain and outputFilename
    ReadConfigurationFile(configFilename, outputFilename, variablevector, treemap, cutmap, enable_infobox);

    gStyle->SetOptStat(enable_infobox);          //Whether to plot the infobox (default: 0)


    //Create new Plotvariables
    for(auto& obj:variablevector) {
        normalized_plots = false;                  //<-------- Normalized plots? Standard: false
        saveto = outputFilename;                   //<-------- Path to save it

        if(obj.type == 0) new Plotvariable(obj.name, treemap[obj.fromtree], obj.title, obj.legend, obj.nbins, obj.range_min, obj.range_max, 
                obj.axislabel, obj.unit, obj.cuts, vecp, obj.options);  //on new canvas
        else if(obj.type == 1) new Plotvariable(obj.name, treemap[obj.fromtree], obj.legend, obj.cuts, vecp);   //on same canvas
        else if(obj.type == 2) new Plotvariable_2D(obj.name, treemap[obj.fromtree], obj.title, obj.nbins, obj.range_min, obj.range_max,
                obj.yrange_min, obj.yrange_max, obj.axislabel, obj.unit, obj.yaxislabel, obj.yunit, obj.cuts, vecp, obj.options);
    }












    // -----------------------------------------------
    // Plotprocess (from here same as the old script)
    // ----------------------------------------------

    TCanvas c("canvas", "dasvancas", 800, 600);         //Create canvas
    TCanvas* cp = &c;
    cp -> SetLeftMargin(0.108);
    cp -> SetRightMargin(0.05);


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












//---------------------------------
//Implementation of ReadConfigfile
//---------------------------------
int ReadConfigurationFile(const string &filename, string& outputFilename, vector<VariableInfo>& variablevector, map<string, TChain*>& treemap, map<string, string>& cutmap,
        int& enable_infobox)
{
    VariableInfo info;

    //Information for the current tree
    string treename;    //Reference to the curren tree
    string inputFilename;
    string decayChain;

    //Information for the cut(map)
    string cutname;
    string cutdefinition;

    ifstream ifs(filename);
    bool parseError = false;
    map<string, function<void(const string&)> > dispatch; // Lambda expressions are used for parsing

    // Trims leading and trailing white space of str
    auto trim = [](const string& str) -> string
    {
        try
        {
            const string whiteSpace = " \t\r";
            size_t a = str.find_first_not_of(whiteSpace);
            size_t b = str.find_last_not_of(whiteSpace);
            return str.substr(a, b-a+1);
        }
        catch(out_of_range& e){ return ""; }	
    };

    // Checks if strs contains str
    auto contains = [](const vector<string>& strs, const string& str){ return find(strs.begin(), strs.end(), str) != strs.end(); };

    // Searches for the next key value pair and calls their interpreters if they are in the expected keys vector (expkeys)
    // Return true when the key matched and false otherwise.
    auto parse = [&](const vector<string>& expkeys)
    {
        static bool nextline = true;
        static string key;
        static string val;

        if(parseError) return false;

        while(ifs && (!nextline || (ifs>>key && getline(ifs, val))))
        {
            nextline = true; 					// Make sure that a new line will be read when we continue.
            if(key[0] == '#') continue;	 		// This is a comment, ignore it.
            if(!contains(expkeys, key)) break; 	// The key does not match any of the expected ones
            dispatch[key](trim(val));
            return true;
        }
        nextline = false; // The key didn't match and we might want to try another set of keys later, so don't read a new line the next time.
        return false; // Fail		
    };

    auto parseAll = [&](const vector<string>& expkeys)
    {
        while(parse(expkeys));
    };

    //Enable infobox?
    dispatch["INFOBOX"] = [&](const string& line){ ArgParser(line).Scan(enable_infobox); };

    //Where to save the histograms
    dispatch["OUTPUT"] = [&](const string& line){ outputFilename = line; };

    //Inputfile(as TCHain) + tree
    dispatch["INPUT"] = [&](const string& line)
    {
        if(ArgParser(line).Scan(treename, inputFilename, decayChain))
        {
            parseError = true;
            cerr << "Parsing error" << endl;
            return;
        }

        treemap[treename] = new TChain(decayChain.c_str());
        treemap[treename] -> Add(inputFilename.c_str());


    };

    //Tree to add to the previous Inputfile
    dispatch["INPUT_ADD"] = [&](const string& line)
    {
        if(ArgParser(line).Scan(inputFilename))
        {
            parseError = true;
            cerr << "Parsing error" << endl;
            return;
        }

        treemap[treename] -> Add(inputFilename.c_str());    //treename is the name of the current tree
    };

    //Define Cuts
    dispatch["CUT"] = [&](const string& line)
    {
        if(ArgParser(line).Scan(cutname, cutdefinition))
        {
            parseError = true;
            cerr << "Parsing error" << endl;
            return;
        }

        cutmap[cutname] = cutdefinition;
    };

    //Normal 1D Plotting
    dispatch["PLOT"] = [&](const string& line)
    {

        dispatch["PLOTVAR"] = [&](const string& line)
        {
            if(ArgParser(line).NumArgs() == 11) {     //Check if there are 11 entries => with options
                if(ArgParser(line).Scan(info.name, info.fromtree, info.title, info.legend, info.nbins, info.range_min, info.range_max, 
                            info.axislabel, info.unit, info.cuts, info.options))
                {
                    parseError = true;
                    cerr << "Parsing error" << endl;
                    return;
                }
            }

            else{       //Else Only 10 => without options
                if(ArgParser(line).Scan(info.name, info.fromtree, info.title, info.legend, info.nbins, info.range_min, info.range_max, 
                            info.axislabel, info.unit, info.cuts))
                {
                    parseError = true;
                    cerr << "Parsing error" << endl;
                    return;
                }
                info.options = "";
            }
            //Check if the cuts are "normal" cuts or an alias which refers to cuts in the cutmap
            info.cuts = cutmap.count(info.cuts) ? info.cuts = cutmap[info.cuts] : info.cuts;     //if alias is found, replace info.cuts with the entry belonging to it 

            info.type = 0;  //Type 2 = Plot on new canvas
            variablevector.push_back(info);

        };

        dispatch["PLOTVAR_SAME"] = [&](const string& line) 
        {
            if(ArgParser(line).Scan(info.name, info.fromtree, info.legend, info.cuts))
            {
                parseError = true;
                cerr << "parsing error" << endl;
                return;
            }
            //Check if the cuts are "normal" cuts or an alias which refers to cuts in the cutmap
            info.cuts = cutmap.count(info.cuts) ? info.cuts = cutmap[info.cuts] : info.cuts;     //if alias is found, replace info.cuts with the entry belonging to it 


            info.type = 1;  //Type 1 = Plot on same canvas as the one before
            variablevector.push_back(info);
        };

        parseAll({"PLOTVAR", "PLOTVAR_SAME"});
    };


    //2D-plotting
    dispatch["PLOT_2D"] = [&](const string& line)
    {

        dispatch["PLOTVAR"] = [&](const string& line)
        {
            if(ArgParser(line).NumArgs() == 14) {     //Check if there are 14 entries => with options
                if(ArgParser(line).Scan(info.name, info.fromtree, info.title,  info.nbins, info.range_min, info.range_max, info.yrange_min, info.yrange_max,
                            info.axislabel, info.unit, info.yaxislabel, info.yunit, info.cuts, info.options))
                {
                    parseError = true;
                    cerr << "Parsing error" << endl;
                    return;
                }
            }

            else{       //Else Only 13 => without options
                if(ArgParser(line).Scan(info.name, info.fromtree, info.title,  info.nbins, info.range_min, info.range_max, info.yrange_min, info.yrange_max,
                            info.axislabel, info.unit, info.yaxislabel, info.yunit, info.cuts))
                {
                    parseError = true;
                    cerr << "Parsing error" << endl;
                    return;
                }
                info.options = "";
            }
            //Check if the cuts are "normal" cuts or an alias which refers to cuts in the cutmap
            info.cuts = cutmap.count(info.cuts) ? info.cuts = cutmap[info.cuts] : info.cuts;     //if alias is found, replace info.cuts with the entry belonging to it 

            info.type = 2;  //Type 2 = 2D-plot
            variablevector.push_back(info);

        };



        parseAll({"PLOTVAR"});
    };





    if(!ifs){ cerr << "Error opening " << filename << endl; return 1; }
    parseAll({"OUTPUT"}); //"INPUT",
    if(outputFilename.empty()){ cerr << "No output ROOT file specified in " << outputFilename << endl; return 1; }

    parseAll({"INFOBOX"});
    parseAll({"INPUT"});
    parseAll({"INPUT_ADD"});

    parseAll({"CUT"});

    parseAll({"PLOT"});
    parseAll({"PLOT_2D"});

    if(inputFilename.empty()){ cerr << "No data ROOT file specified in "   <<  inputFilename << endl; return 1; }

    // If we did not reach the EOF, then the configuration file contains gibberish
    if(parseError){ return 1; }
    if(ifs){ cerr << "Error parsing " << filename << endl; return 1; }

    return 0;
}
