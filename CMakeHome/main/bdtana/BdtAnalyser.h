#ifndef _BDTANALYSER_H
#define _BDTANALYSER_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iomanip>
#include <ctime>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TBranch.h"

#include "TMVA/Tools.h"
#include "TMVA/Factory.h"
#include "TMVA/Reader.h"
#include "TMVA/Config.h"

class BdtAnalyser
{
	using string = std::string;
	template<class T> using vector = std::vector<T>;
public:
	BdtAnalyser(){ TMVA::Tools::Instance(); }
	~BdtAnalyser(){}
	
	void AddSignal    (const string& filename, const string& tree, const string& weight=""){ sigInputs.push_back({filename, tree, weight}); }
	void AddBackground(const string& filename, const string& tree, const string& weight=""){ bkgInputs.push_back({filename, tree, weight}); }
	void AddVariable  (const string& variable){ variables.push_back(variable); }
	void AddData      (const string& filename, const string& tree){ dataInputs.push_back({filename, tree}); }
	
	void SetTrainOutput   (const string& filename){ trainFilename = filename; }
	void SetApplyOutput   (const string& filename, const string& tree=""){ applyFilename = filename; applyTree = tree;}
	void SetFactoryOptions(const string& options){ factoryOptions = options; }
	void SetBdtOptions    (const string& options){ bdtOptions = options; }
	void SetReaderOptions (const string& options){ readerOptions = options; }
	void SetSampleOptions (const string& options){ sampleOptions = options; }
	
	void Train();
	void Apply();
	
private:
	struct FileTreeWeight {string filename, tree, weight; };
	struct FileTree {string filename, tree; };
	vector<FileTreeWeight> sigInputs;
	vector<FileTreeWeight> bkgInputs;
	vector<FileTree> dataInputs;
	vector<string> variables;
	string trainFilename;
	string applyFilename;
	string applyTree;
	string dataFilename;
	string factoryOptions;
	string bdtOptions;
	string readerOptions;
	string sampleOptions;
	
	TChain* CreateDataChain() const;
	TFile* CreateApplyOutput() const;
		
	string GetFactoryID() const
	{
		size_t a = trainFilename.rfind('/');
		size_t b = trainFilename.rfind('.');
		
		a = a==string::npos? 0 : a+1;
		if(b == string::npos || b < a) b = trainFilename.size();
		
		return trainFilename.substr(a, b-a);
	}
	
	string GetWeightFileDirectory() const
	{
		size_t p = trainFilename.rfind('/');
		if(p == string::npos) return ".";
		return trainFilename.substr(0, p);
	}	
	
	string GetWeightFilename() const
	{
		return GetWeightFileDirectory() + '/' + GetFactoryID() + "_BDT.weights.xml";
	}
	
	static int GetBranchType(TTree* pTree, const string& branch)
	{
		TBranch* pBranch = pTree->GetBranch(branch.c_str());
		string str=pBranch->GetTitle(); 
		size_t p=str.find_last_of('/');
		char c = p==string::npos? 'F' : str[p+1];
		switch(c)
		{
			case 'F': return 1;
			case 'D': return 2;
			default:  return 0;
		}
	}
	
	struct Timer
	{
		Timer():t0(0),c0(0){}
		time_t t0;
		clock_t c0;
		bool PrintProgress(Long64_t i, Long64_t n)
		{
			clock_t c = clock();
			if(c-c0 < CLOCKS_PER_SEC/5 && i && i!=n-1) return false;
			if(!i){ c0 = clock(); t0 = time(NULL); }
			
			std::ostringstream os;
			time_t t = time(NULL);
			time_t eta = (time_t) ((n-i)*(t - t0)*1./i + 0.5);
			double p = i*1./n;
			
			os << std::fixed << std::setprecision(2) << std::setw(6) << 100*p << '%';
			os << " [" << std::setfill('=') << std::setw(p*59) << "";
			os << std::setfill(' ') << std::setw((1.-p)*59) << "" << "] ";

			if(t-t0 < 1) os << std::setw(10) << "";
			else if(eta >= 3600) os << std::setw(4) << (time_t) (eta*1./3600 + 0.5) <<    " hours";
			else if(eta >= 60) os << std::setw(2) << (time_t) (eta*1./60 + 0.5) << " minutes";
			else os << std::setw(2) << eta << " seconds";
		
			std::cout << "\r" << os.str() << std::flush;
			
			if(i==n-1) std::cout << std::endl;
			c0 = c;
			return true;
		}
	};
};

#endif // _BDTANALYSER_H




































