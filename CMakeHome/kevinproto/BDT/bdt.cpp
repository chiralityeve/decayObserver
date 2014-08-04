#include <iostream>
#include <string>
#include <vector>
//#include <tuple>
//#include <cstdlib>
//#include <ctime> 
//#include <vector>
//#include <cmath>
//#include <iomanip>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#include "TMVA/Tools.h"
#include "TMVA/Factory.h"
#include "TMVA/Reader.h"
#include "TMVA/Config.h"

using namespace std;

class BDTAnalyser
{
public:
	BDTAnalyser(){ TMVA::Tools::Instance(); }
	~BDTAnalyser(){}
	
	void AddSignal(const string& filename, const string& tree, const string& weight=""){ sigInputs.push_back({filename, tree, weight}); }
	void AddBackground(const string& filename, const string& tree, const string& weight=""){ bkgInputs.push_back({filename, tree, weight}); }
	void AddVariable(const string& variable){ variables.push_back(variable); }
	void SetTrainOutput(const string& filename){ trainFilename = filename; }
	void SetApplyOutput(const string& filename){ applyFilename = filename; }
	void SetFactoryOptions(const string& options){ factoryOptions = options; }
	void SetBdtOptions(const string& options){ bdtOptions = options; }
	void SetReaderOptions(const string& options){ readerOptions = options; }
	
	void Train();
	
private:
	struct FileTreeWeight {string filename, tree, weight; };
	vector<FileTreeWeight> sigInputs;
	vector<FileTreeWeight> bkgInputs;
	vector<string> variables;
	string trainFilename;
	string applyFilename;
	string factoryOptions;
	string bdtOptions;
	string readerOptions;
		
	string GetFactoryID() const
	{
		size_t a = trainFilename.rfind('/');
		size_t b = trainFilename.rfind('.');
		
		if(a == string::npos) a = 0;
		if(b == string::npos || b < a) b = trainFilename.size() - a;
		
		return trainFilename.substr(a, b);
	}
	
	string GetWeightFileDirectory() const
	{
		return "../plots"; // Temporary
		size_t p = trainFilename.rfind('/');
		if(p == string::npos) return "./";
		return trainFilename.substr(0, p + 1);
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
};

void BDTAnalyser::Train()
{
	TFile* pFileOut = new TFile(trainFilename.c_str(), "RECREATE");
	TMVA::Factory* pFactory = new TMVA::Factory(GetFactoryID(), pFileOut, factoryOptions);
	map<string, TFile*> files;
	TTree* pFirstTree = NULL;
	
	// Why is this a global variable?
	TMVA::gConfig().GetIONames().fWeightFileDir = GetWeightFileDirectory();
	
	// Add signal and background trees
	for(auto& sig : {true, false})
	for(auto& input : sig?sigInputs:bkgInputs)
	{
		TFile*& pFile = files[input.filename]? files[input.filename] : (files[input.filename] = new TFile(input.filename.c_str()));
		TTree*  pTree = pFile? (TTree*) pFile->Get(input.tree.c_str()) : NULL;
		
		if(!pFile){ cerr << "Error opening file " << input.filename << endl; return; }
		if(!pTree){ cerr << "Error opening tree " << input.tree << " in " << input.filename << endl; return; }
		
		if(!pFirstTree) pFirstTree = pTree;
		
		if(sig) pFactory->AddSignalTree(pTree); else pFactory->AddBackgroundTree(pTree);		
		if(input.weight.empty()) continue;
		if(sig) pFactory->SetSignalWeightExpression(input.weight); else pFactory->SetBackgroundWeightExpression(input.weight);
	}
	
	// Generate training and testing samples
	pFactory->PrepareTrainingAndTestTree("", "SplitMode=random:!V");
	
	// Add variables
	for(auto& var : variables)
		pFactory->AddVariable(var, GetBranchType(pFirstTree, var)?'I':'F');
	
	// Run the training and testing
	pFactory->BookMethod(TMVA::Types::kBDT, "BDT", bdtOptions);
	pFactory->TrainAllMethods();
	pFactory->TestAllMethods();
	pFactory->EvaluateAllMethods();	
	
	delete pFileOut;
	for(auto& file : files) delete file.second;
	delete pFactory;
}


int main(int argc, char** argv)
{
	bool argTrain = false;
	bool argApply = false;
	string configFilename;
	
	if(argc == 3) 
	{
		argTrain = argv[1] == string("-t") || argv[1] == string("-train");
		argApply = argv[1] == string("-a") || argv[1] == string("-apply");
		configFilename = argv[2];
	}
	
	if(argTrain == argApply)
		return 0;
		
	
	// Underneath here is all temporary and for testing the BDTAnalyser class
	BDTAnalyser bdt;
	
	bdt.SetTrainOutput("./BDT_training_test.root");
	bdt.AddSignal("/afs/cern.ch/work/d/dberning/public/BDT_Signal.root", "/Bs2phimumuTuple/DecayTree");
	bdt.AddBackground("/afs/cern.ch/work/d/dberning/public/BDT_Background.root", "/Bs2phimumuTuple/DecayTree");
	bdt.SetFactoryOptions("!V:!Silent:Color:DrawProgressBar:Transformations=I;G:AnalysisType=Classification");
	bdt.SetBdtOptions("!H:!V:NTrees=800:MinNodeSize=2.5%:MaxDepth=2:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=150");
	
	vector<string> vars = {"B0_PT", "B0_IPCHI2_OWNPV", "B0_FDCHI2_OWNPV", "B0_DIRA_OWNPV", "B0_ENDVERTEX_CHI2", "Kplus_IPCHI2_OWNPV", "Kminus_IPCHI2_OWNPV", "muminus_IPCHI2_OWNPV", "muplus_IPCHI2_OWNPV", "Kplus_PIDK", "Kminus_PIDK", "muminus_PIDmu", "muplus_PIDmu", "Kminus_PT", "Kplus_PT", "phi_1020_ENDVERTEX_CHI2", "Kplus_PIDp", "Kminus_PIDp", "Kplus_TRACK_CHI2NDOF", "Kminus_TRACK_CHI2NDOF", "muplus_TRACK_CHI2NDOF", "muminus_TRACK_CHI2NDOF"};
	
	for(auto& var : vars) bdt.AddVariable(var);
	
	if(argTrain) bdt.Train();
	
	return 0;
}










































