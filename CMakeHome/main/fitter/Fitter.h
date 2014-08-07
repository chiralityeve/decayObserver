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
#include "BranchProxy.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
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
#include "RooStats/SPlot.h"

#include "TAxis.h"
#include "TApplication.h"
#include "TStyle.h"
#include "TPad.h"
#include "TF1.h"
#include "TGraph.h"
#include "TH1.h"
#include "TLegend.h"

class Fitter
{
	using string = std::string;
	using ostream = std::ostream;
	using SPlot = RooStats::SPlot;
	template<class T> using vector = std::vector<T>;
	template<class T, class S> using map = std::map<T, S>;
	struct FileTree
	{ 
		FileTree(const string& filename, const string& treeName):filename(filename),treeName(treeName){}
		string filename, treeName; 
		vector<Long64_t> entriesInRange; 
		string GetPath() const
		{
			bool glue = filename.back() != '/' && treeName.front() != '/';
			return filename + (glue?"/":"") + treeName;
		}
	};
	struct PlotCanvas
	{
		PlotCanvas():pCanvas(NULL),pPadTop(NULL),pPadBottom(NULL),pFuncDummy(NULL){}
		~PlotCanvas(){ Delete(pPadTop, pPadBottom, pCanvas, pFuncDummy); }
		TCanvas* pCanvas;
		TPad* pPadTop;
		TPad* pPadBottom;
		TF1* pFuncDummy;
	};
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
	
	int AddData(const string& filename, const string& treeName)
	{
		fileTreePairs.push_back({filename, treeName});
		return 0;
	}
	int AddData(const vector<string>& filenames, const vector<string>& treeNames)
	{
		auto iFile = filenames.begin();
		auto iTree = treeNames.begin();
		
		while(iFile != filenames.end() && iTree != treeNames.end())
			fileTreePairs.push_back({*iFile++, *iTree++});
		return 0;
	}
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
	
	
	string branchName;
	Interval range;
	string branchUnit;
	string plotTitle;
	string plotAxisLabel;
	string outputFilename;
	string outputSweights;
	
private:	
	vector<FileTree> fileTreePairs;
	
	RooRealVar* pFitVar;
	RooDataSet* pData;
	
	vector<FitterPdf> sigPdfs;
	vector<FitterPdf> bkgPdfs;
	vector<RooAbsReal*> vec_pParam;
	RooAddPdf* pTotPdf;
	RooFitResult* pResult;
	SPlot* pSPlot;
	
	void AddParams(const vector<RooAbsReal*>& vec){ vec_pParam.insert(vec_pParam.end(), vec.begin(), vec.end()); }
	int InitChain(TChain* pChain);
	int LoadData();
	int InitPdfs();
	void Plot();
	
	void MakePlots(const string& name, const string& title, RooPlot*& pPlot1, RooPlot*& pPlot2, RooAbsPdf* pPdf=NULL, RooDataSet* pData=NULL);
	void CombinePlots(const string& name, PlotCanvas& pc, RooPlot*& pPlot1, RooPlot*& pPlot2);
	
	void Calculate_sWeights();
	void FixParameters(bool fix=true);
	RooArgList GetYieldsList();
	void Plot_sWeighted();
	void Plot_sWeights();
	int Save_sWeights();
	
	TFile* pFileOut;
	TDirectory* pBaseDir;
		
	int OpenOutputFile(bool overwrite);
	void CloseOutputFile();
	void ChangeDirectory(const string& name="");
	
	
	void InitPointers();
	void Clean();
	
	static Interval GetNiceLimits(const Interval& r, const int nd=2);
	
	
	
	// Set a bunch of pointers to NULL;
	template<class T1> static void InitNULL(T1& pT1){ pT1=NULL; }
	template<class T1, class... Tn> static void InitNULL(T1& pT1, Tn&... pTn){ pT1=NULL; InitNULL(pTn...); }
	
	template<class T> static void Delete(T& pointer){ if(pointer != NULL) delete pointer; pointer = NULL; }
	template<class T1, class... Tn> static void Delete(T1& pt1, Tn&... pn){ if(pt1 != NULL) delete pt1; pt1 = NULL; Delete(pn...); }
	template<class T> static void Delete(vector<T>& pointers){ for(T& p : pointers) Delete(p); pointers.clear(); }
};

#endif // _FITTER_H







































