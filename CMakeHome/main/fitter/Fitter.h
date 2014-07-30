#ifndef _FITTER_H
#define _FITTER_H

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include <unordered_map>

#include "Interval.h"
#include "FitterPdf.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TText.h"
#include "TCanvas.h"
#include "TBranch.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooPolynomial.h"
#include "RooAddPdf.h"
#include "RooPlot.h"
#include "RooMsgService.h"
#include "RooFitResult.h"
#include "RooExponential.h"

#include "TAxis.h"
#include "TApplication.h"
#include "TStyle.h"
#include "TPad.h"
#include "TF1.h"


using namespace std;

class Fitter
{
public:
	Fitter();
	Fitter(const string& rootFilename, const string& treeName, 
		   const string& branchName, const Interval& range, const string& branchUnit,  
		   const string& plotTitle, const string& plotAxisLabel, const string& outputFilename);
	Fitter(const vector<string>& rootFilenames, const vector<string>& treeNames, 
		   const string& branchName, const Interval& range, const string& branchUnit,  
		   const string& plotTitle, const string& plotAxisLabel, const string& outputFilename);
	Fitter(const Fitter& other);
	~Fitter();
	
	Fitter& operator =(const Fitter& other);
	ostream& Print(ostream& os) const;
	friend ostream& operator<<(ostream& os, const Fitter& f){ return f.Print(os); }
	
	int AddSignal(const string& args)
	{
		sigPdfs.emplace_back(args);
		return sigPdfs.back().Check();
	}
	int AddBackground(const string& args)
	{
		bkgPdfs.emplace_back(args);
		return bkgPdfs.back().Check();
	}
	
	int Fit(bool showRooFitOutput=false, bool overwrite=false);
	
	vector<string> rootFilenames;
	vector<string> treeNames;
	string branchName;
	Interval range;
	string branchUnit;
	string plotTitle;
	string plotAxisLabel;
	string outputFilename;
	
private:	
	//TFile* pFile;
	//TTree* pTree;
	RooRealVar* pFitVar;
	RooDataSet* pData;
	
	vector<FitterPdf> sigPdfs;
	vector<FitterPdf> bkgPdfs;
	vector<RooAbsReal*> vec_pParam;
	RooAddPdf* pTotPdf;
	RooFitResult* pResult;
	
	void AddParams(const vector<RooAbsReal*>& vec){ vec_pParam.insert(vec_pParam.end(), vec.begin(), vec.end()); }	
	int LoadData();
	int InitPdfs();
	int Plot(bool overwrite);
	
	void MakePlots(RooPlot*& pPlot1, RooPlot*& pPlot2);
	
	void InitPointers();
	void Clean();
	
	// Set a bunch of pointers to NULL;
	template<class T1> static void InitNULL(T1& pT1){ pT1=NULL; }
	template<class T1, class... Tn> static void InitNULL(T1& pT1, Tn&... pTn){ pT1=NULL; InitNULL(pTn...); }
	
	template<class T> static void Delete(T& pointer){ if(pointer != NULL) delete pointer; pointer = NULL; }
	template<class T> static void Delete(vector<T>& pointers){ for(T& p : pointers) Delete(p); pointers.clear(); }
};

#endif // _FITTER_H







































