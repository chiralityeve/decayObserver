#include "BdtAnalyser.h"

using namespace std;

void BdtAnalyser::Train()
{
	TFile* pFileOut = new TFile(trainFilename.c_str(), "RECREATE");
	TMVA::Factory* pFactory = pFileOut? new TMVA::Factory(GetFactoryID(), pFileOut, factoryOptions) : NULL;
	map<string, TFile*> files;
	TTree* pFirstTree = NULL;
	
	if(!pFileOut){ cerr << "Error opening " << trainFilename << endl; return; }
	
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
	{
		if(!pFirstTree->GetBranch(var.c_str())){ cerr << "No branch named " << var << endl; return; }
		pFactory->AddVariable(var, GetBranchType(pFirstTree, var)?'F':'I');
	}
	
	// Run the training and testing
	pFactory->BookMethod(TMVA::Types::kBDT, "BDT", bdtOptions);
	pFactory->TrainAllMethods();
	pFactory->TestAllMethods();
	pFactory->EvaluateAllMethods();	
	
	delete pFileOut;
	for(auto& file : files) delete file.second;
	delete pFactory;
}

TChain* BdtAnalyser::CreateDataChain() const
{
	TChain* pChain = new TChain;
	
	for(auto& d : dataInputs)
	{
		bool glue = !(d.filename.back() == '/' || d.tree.front() == '/');
		string path = d.filename + (glue?"/":"") + d.tree;
		
		if(pChain->Add(path.c_str())) continue;
		cerr << "Error adding " << path << " to chain" << endl; 
		delete pChain;
		return NULL; 
	}
	
	return pChain;
}

TFile* BdtAnalyser::CreateApplyOutput() const
{
	TFile* pFile = new TFile(applyFilename.c_str(), "RECREATE");
	string tree = applyTree.empty()? dataInputs.front().tree : applyTree;
	size_t a = tree.find_first_not_of('/');
	size_t b = tree.rfind('/');
	string dir;
	TDirectory* pDir = NULL;
	
	if(!pFile){ cerr << "Error opening " << applyFilename << endl; return NULL; }
	if(a == string::npos){ delete pFile; return NULL; }
	if(a == 0 && b == string::npos) return pFile;
	if(b < a) return pFile;
	dir = tree.substr(a, b-a);
	pDir = pFile->mkdir(dir.c_str());
	if(!pDir){ cerr << "Error creating directory " << dir << " in " << applyFilename << endl; delete pFile; return NULL; }
	pDir->cd();
	return pFile;
}

void BdtAnalyser::Apply()
{
	struct DFPair { Double_t* d; Float_t* f; };
	TChain* pChain = CreateDataChain();
	TFile* pFile = pChain? CreateApplyOutput() : NULL;
	TTree* pTree = pFile? pChain->CloneTree(0) : NULL;
	TMVA::Reader* pReader = pFile? new TMVA::Reader(readerOptions) : NULL;
	vector<DFPair> dfPairs;
	vector<Float_t*> pFloats;
	vector<Int_t*> pInts;
	Double_t bdtResponse=0.;
	
	if(!pChain || !pFile) return;
	
	pTree->Branch("TMVAResponse", &bdtResponse, "TMVAResponse/D");
	
	// Set variables
	for(auto& var : variables)
		switch(GetBranchType(pChain, var))
		{
		case 0: 
			pInts.push_back(new Int_t);
			pReader->AddVariable(var, pInts.back());
			pChain->SetBranchAddress(var.c_str(), pInts.back());
			break;
		case 1:
			pFloats.push_back(new Float_t);
			pReader->AddVariable(var, pFloats.back());
			pChain->SetBranchAddress(var.c_str(), pFloats.back());
			break;
		case 2:
			dfPairs.push_back({new Double_t, new Float_t});
			pReader->AddVariable(var, dfPairs.back().f);
			pChain->SetBranchAddress(var.c_str(), dfPairs.back().d);
			break;
		}
		
	pReader->BookMVA("BDT", GetWeightFilename());
	
	// Fill the output tree
	Long64_t nEvents = pChain->GetEntries();
	Timer timer;
	for(Long64_t i=0; i<nEvents; ++i)
	{
		timer.PrintProgress(i, nEvents);
		pChain->GetEntry(i);
		
		for(auto& df : dfPairs) *df.f = *df.d;
		bdtResponse = pReader->EvaluateMVA("BDT");
		pTree->Fill();
	}
	pTree->AutoSave();
	
	delete pChain;
	delete pFile;
	delete pReader;
}








































