#ifndef _FITTER_H
#define _FITTER_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iomanip>
//#include <cstdlib>
/*#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include <unordered_map>

#include "Interval.h"
#include "FitterPdf.h"
#include "BranchProxy.h"*/

#include "Interval.h"
#include "BranchProxy.h"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TAxis.h"
//#include "TApplication.h"
#include "TStyle.h"
#include "TPad.h"
#include "TF1.h"
#include "TGraph.h"
#include "TH1.h"
#include "TLegend.h"
#include "TCanvas.h"

#include "RooAbsReal.h"
#include "RooAbsPdf.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooArgList.h"
#include "RooFormulaVar.h"
#include "RooGaussian.h"
#include "RooPolynomial.h"
#include "RooExponential.h"
#include "RooFitResult.h"
#include "RooAddPdf.h"
#include "RooMsgService.h"
#include "RooPlot.h"
#include "RooCBShape.h"

#include "RooStats/SPlot.h"

class Fitter;

class FitterParam
{
	using string = std::string;
	template<class T> using vector = std::vector<T>;
public:
	FitterParam(const string& id):id(id),only_id(true),initial(0.),minimum(0.),maximum(0.){}
	FitterParam(const string& id, double initial):id(id),only_id(false),initial(initial),minimum(initial),maximum(initial){}
	FitterParam(const string& id, double initial, double minimum, double maximum):id(id),only_id(false),initial(initial),minimum(minimum),maximum(maximum){}
	FitterParam(double initial, double minimum, double maximum):id(),only_id(false),initial(initial),minimum(minimum),maximum(maximum){}
	FitterParam(double initial):id(),only_id(false),initial(initial),minimum(initial),maximum(initial){}
	~FitterParam(){}
	
	ostream& Print(ostream& os) const;
	friend ostream& operator<<(ostream& os, const FitterParam& f){ return f.Print(os); }
	
	string id;
	bool only_id;
	double initial;
	double minimum;
	double maximum;
	
	static int ParseArgs(const string& line, vector<FitterParam>& params);
};

class FitterPdf
{
	using string = std::string;
	template<class T> using vector = std::vector<T>;
	friend class Fitter;
public:
	FitterPdf();
	FitterPdf(const FitterPdf& other);
	FitterPdf(Fitter* pFitter, const string& name, const string& pdf, const vector<FitterParam>& params);
	~FitterPdf();
	
	ostream& Print(ostream& os) const;
	
	FitterPdf& operator=(const FitterPdf& other);
	friend ostream& operator<<(ostream& os, const FitterPdf& f){ return f.Print(os); }
	
	int Check();
	int Init();
	
	RooAbsPdf& GetPdf(){ return *pPdf; }
	RooAbsReal& GetYld(){ return *pYld; }
	
private:
	void InitPointers();
	void Clean();
	
	Fitter* pFitter;
	string name;
	string pdf;
	vector<FitterParam> params;
	RooAbsPdf* pPdf;
	RooAbsReal* pYld;
	
	RooAbsReal* NewVar(const FitterParam& param, const string& parname, const string& unit);
	RooAbsReal* NewFormulaVar(const string& parname, const string& formula, const vector<RooAbsReal*>& dependents, const string& unit);
	template<class T, class... Ts> void NewPdf(Ts&... vs)
	{
		pPdf = new T(name.c_str(), name.c_str(), vs...);
	}

};

class Fitter
{
	using string = std::string;
	template<class T> using vector = std::vector<T>;
	template<class T, class S> using map = std::map<T, S>;
	friend class FitterPdf;
public:
	Fitter();
	Fitter(const Fitter& other);
	~Fitter();
	
	Fitter& operator =(const Fitter& other);
	
	ostream& Print(ostream& os) const;
	friend ostream& operator<<(ostream& os, const Fitter& f){ return f.Print(os); }
	
	int AddData(const string& filename, const string& tree);
	void SetBranch(const string& branch, const string& unit, const Interval& range);
	void SetPlot(const string& plotTitle, const string& axisLabel);
	void SetResultsFilename(const string& resultsFilename);
	void SetWeightsFilename(const string& weightsFilename);
	int AddSignal(const string& pdf, const vector<FitterParam>& params);
	int AddBackground(const string& pdf, const vector<FitterParam>& params);
	
	int Run(bool verbose=false, bool recreate=false);
	

private:	
	vector<string> trees;
	string branch;
	string unit;
	Interval range;
	string plotTitle;
	string axisLabel;
	string resultsFilename;
	string weightsFilename;
	map<string, RooAbsReal*> sharedParams;
	vector<RooAbsReal*> params;
	vector<FitterPdf> sigPdfs;
	vector<FitterPdf> bkgPdfs;
	
	RooRealVar* pFitVar;
	RooDataSet* pData;
	RooAddPdf* pTotPdf;
	RooStats::SPlot* pSPlot;
	
	RooFitResult* pResult;
	
	TChain* CreateDataChain();
	int LoadData();
	int InitPdfs();
	void Fit(bool verbose);
	void SaveResults(bool recreate);
	void CalculateWeights();
	int SaveWeights();
	
	void FixParameters(bool fix);
	RooArgList GetYieldsList();
	
	void InitPointers();
	void Clean();
	
	static string Int2String(int n){ std::ostringstream oss; oss << n; return oss.str(); }
	
	
	template<class T1> static void InitNULL(T1& pT1){ pT1=NULL; }
	template<class T1, class... Tn> static void InitNULL(T1& pT1, Tn&... pTn){ pT1=NULL; InitNULL(pTn...); }
	template<class T> static void Delete(T& pointer){ if(pointer != NULL) delete pointer; pointer = NULL; }
	template<class T, class... Ts> static void Delete(T& pointer, Ts... ps){ if(pointer != NULL) delete pointer; pointer = NULL; Delete(ps...); }
	template<class T> static void Delete(vector<T>& pointers){ for(T& p : pointers) Delete(p); pointers.clear(); }
};



#endif // _FITTER_H







































