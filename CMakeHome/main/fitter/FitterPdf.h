#ifndef _FITTERPDF_H
#define _FITTERPDF_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include "ArgParser.h"
#include "Interval.h"
#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooPolynomial.h"
#include "RooExponential.h"
#include "RooFormulaVar.h"
#include "RooHist.h"

using namespace std;

class FitterPdf
{
public:
	FitterPdf();
	FitterPdf(const FitterPdf& other);
	//FitterPdf(const string& type, vector<double> args);
	FitterPdf(const string& args);
	~FitterPdf();
	
	void SetArgs(const string& args);
	
	FitterPdf& operator =(const FitterPdf& other);
	ostream& Print(ostream& os) const;
	friend ostream& operator<<(ostream& os, const FitterPdf& fp){ return fp.Print(os); }
	
	//operator RooAbsPdf*(){ return pPdf; }
	RooAbsPdf& GetPdf(){ return *pPdf; }
	RooAbsReal& GetYld(){ return *pYld; }
	
	int Check();	
	int Init(const string& name, RooRealVar& fitVar, int nPdfs, int nEvents);

private:
	string type;
	ArgParser args;
	int nArgs;
	RooAbsPdf* pPdf;
	RooRealVar* pYld;
	vector<RooAbsReal*> vec_pParam;
	RooRealVar* pFitVar;
	string basename;
	string latestname;
	string fitUnit;
	Interval fitRange;
	
	void AddParams(const vector<RooAbsReal*>& vec){ vec_pParam.insert(vec_pParam.end(), vec.begin(), vec.end()); }
	
	const char* Name(const string& name){ latestname = basename + ' ' + name; return latestname.c_str(); }
	const char* Name(){ return latestname.c_str(); }
	
	int CheckGaussian();
	int CheckPolynomial();
	int CheckExponential();
	
	int InitGaussian();
	int InitPolynomial();
	int InitExponential();
	
	void InitPointers();
	void Clean();
	
	static const map<string, vector<function<int(FitterPdf*)> > > dispatch;
	
	template<class... Ts> RooRealVar* NewVar(const string& name, const string& unit, Ts... vs)
	{
		string fullName = basename + '_' + name;
		const char* cFullName = fullName.c_str();
		RooRealVar* pVar = new RooRealVar(cFullName, cFullName, vs..., unit.c_str());
		
		vec_pParam.push_back(pVar);
		return pVar;
	}
	
	template<class T, class... Ts> void NewPdf(Ts&... vs)
	{
		pPdf = new T(basename.c_str(), basename.c_str(), vs...);
	}
	
	// Set a bunch of pointers to NULL;
	template<class T1> static void InitNULL(T1& pT1){ pT1=NULL; }
	template<class T1, class... Tn> static void InitNULL(T1& pT1, Tn&... pTn){ pT1=NULL; InitNULL(pTn...); }
	
	template<class T> static void Delete(T& pointer){ if(pointer != NULL) delete pointer; pointer = NULL; }
	template<class T> static void Delete(vector<T>& pointers){ for(T& p : pointers) Delete(p); pointers.clear(); }
};

#endif // _FITTERPDF_H







































