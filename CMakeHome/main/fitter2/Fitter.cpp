#include "Fitter.h"

using namespace std;

ostream& FitterParam::Print(ostream& os) const
{
	ostringstream oss;
	
	if(id.empty())
		oss << '(' << initial << ", " << minimum << ", " << maximum << ')';
	else if(only_id)
		oss << id;
	else
		oss << '(' << id << ", " << initial << ", " << minimum << ", " << maximum << ')';
	
	return os << oss.str();
}

int FitterParam::ParseArgs(const string& line, vector<FitterParam>& params)
{
	const string ws = " \t\r";
	ostringstream oss;
//	string line = _line + " "
	int n = line.size();
	vector<string> args;
	bool inbrackets = false;
	
	//cout << line << endl;
	
	for(int i=0; i<n; ++i)
	{
		const char& c = line[i];
		
		if(c == '(')
		{ 
			if(!oss.str().empty()) return 1;
			inbrackets = true;
			continue; 
		}
		if(c == ',')
		{
			if(!inbrackets) return 1;
			args.push_back(oss.str());
			oss.clear();
			oss.str("");
			continue;
		}
		if(c == ')')
		{
			if(!inbrackets) return 1;
			inbrackets = false;
			args.push_back(oss.str());
			oss.clear();
			oss.str("");
			
			int nArgs = args.size();
			vector<double> vals;
			auto iArg = args.begin();
			string id;
			
			if(nArgs == 2 || nArgs == 4)
			{
				id = *iArg;
				++iArg;
				--nArgs;
			}
			
			if(nArgs != 1 && nArgs != 3) return 1;
			for(; iArg!=args.end(); ++iArg)
			{
				istringstream iss(*iArg);
				double val = 0.;
				if(!(iss >> val)) return 1;
				vals.push_back(val);
			}
			
			if(nArgs == 1) params.emplace_back(id, vals[0], vals[0], vals[0]);
			if(nArgs == 3) params.emplace_back(id, vals[0], vals[1], vals[2]);	
			args.clear();
			continue;			
		}
		if(!inbrackets && (ws.find(c) != string::npos || i==n-1))
		{
			if(oss.str().empty()) continue;
			if(ws.find(c) == string::npos) oss << c;
			params.emplace_back(oss.str());
			oss.clear();
			oss.str("");
			continue; 
		}
		if(!inbrackets && oss.str().empty() && c >= '0' && c <= '9') return 1;
	
		oss << c;
	}
	
	return 0;
}








FitterPdf::FitterPdf(){ InitPointers(); }
FitterPdf::FitterPdf(const FitterPdf& other){ InitPointers(); *this = other; }
FitterPdf::FitterPdf(Fitter* pFitter, const string& name, const string& pdf, const vector<FitterParam>& params):
name(name),pdf(pdf),params(params)
{
	InitPointers();
	this->pFitter = pFitter;
}
FitterPdf::~FitterPdf(){ Clean(); }

ostream& FitterPdf::Print(ostream& os) const
{
	ostringstream oss;
	
	oss << left << fixed << setprecision (2);
	oss << setw(7) << (name + ": ") << pdf;
	for(auto& param : params) oss << ' ' << param;
	
	return os << oss.str();
}

int FitterPdf::Check()
{
	const map<string, int> pdf_nArg = {{"Gaussian", 3}, {"Polynomial", -1}, {"Exponential", 2}, {"CrystalBall", 5}};
	int nParams = params.size();
	
	if(!pdf_nArg.count(pdf)) return 1;
	if(pdf_nArg.at(pdf)!=-1 && pdf_nArg.at(pdf) != nParams) return 2;
	
	if(pdf == "Polynomial" && nParams < 1) return 2;
	
	return 0;
}
int FitterPdf::Init()
{
	if(Check()) return 1;
	pYld = NewVar(params[0], "yield", "");
	
	if(pdf == "Gaussian")
	{
		RooAbsReal* pMean = NewVar(params[1], "mean", pFitter->unit);
		RooAbsReal* pSigma = NewVar(params[2], "sigma", pFitter->unit);
				
		NewPdf<RooGaussian>(*pFitter->pFitVar, *pMean, *pSigma);
		return 0;
	}
	
	if(pdf == "Polynomial")
	{
		auto int2str = [](int n){ ostringstream oss; oss << n; return oss.str(); };
		RooArgList coefficients;
		
		RooAbsReal* pShift = NewVar(FitterParam(pFitter->range.Center()), "shift", pFitter->unit);
		RooAbsReal* pShiftedFitVar = NewFormulaVar("shiftedFitVar", pFitter->pFitVar->GetName() + string(" - ") + pShift->GetName(), {pFitter->pFitVar, pShift}, pFitter->unit);
		
		size_t degree = params.size() - 1;
		for(size_t i=1; i<=degree; ++i)
			coefficients.add(*NewVar(params[i], "a"+int2str(i), "("+pFitter->unit+")^{-"+int2str(i)+"}"));
		
		NewPdf<RooPolynomial>(*pShiftedFitVar, coefficients);
		return 0;
	}
	
	if(pdf == "Exponential")
	{
		RooAbsReal* pLambda = NewVar(params[1], "lambda", "("+pFitter->unit+")^{-1}");
		
		NewPdf<RooExponential>(*pFitter->pFitVar, *pLambda);
		
		return 0;
	}
	
	if(pdf == "CrystalBall")
	{
		RooAbsReal* pM0 = NewVar(params[1], "m0", pFitter->unit);
		RooAbsReal* pSigma = NewVar(params[2], "sigma", pFitter->unit);
		RooAbsReal* pAlpha = NewVar(params[3], "alpha", "");
		RooAbsReal* pN = NewVar(params[4], "n", "");
		
		NewPdf<RooCBShape>(*pFitter->pFitVar, *pM0, *pSigma, *pAlpha, *pN);
		
		return 0;
	}

	return 1;
}

RooAbsReal* FitterPdf::NewVar(const FitterParam& param, const string& parname, const string& unit)
{
	if(!param.id.empty() && pFitter->sharedParams.count(param.id)) return pFitter->sharedParams[param.id];

	string fullName = name + '_' + parname;
	const char* cFullName = fullName.c_str();
	RooRealVar* pVar = new RooRealVar(cFullName, cFullName, param.initial, param.minimum, param.maximum, unit.c_str());

	pFitter->params.push_back(pVar);
	if(!param.id.empty()) pFitter->sharedParams[param.id] = pVar;

	return pVar;
}
RooAbsReal* FitterPdf::NewFormulaVar(const string& parname, const string& formula, const vector<RooAbsReal*>& dependents, const string& unit)
{
	string fullName = name + '_' + parname;
	const char* cFullName = fullName.c_str();
	RooArgList argList;
	RooAbsReal* pVar = NULL;
	
	for(auto& dep : dependents) argList.add(*dep);
	pVar = new RooFormulaVar(cFullName, cFullName, formula.c_str(), argList);

	pFitter->params.push_back(pVar);

	return pVar;	
}

FitterPdf& FitterPdf::operator=(const FitterPdf& other)
{
	Clean();
	InitPointers();
	
	pFitter = other.pFitter;
	name = other.name;
	pdf = other.pdf;
	params = other.params;
	
	return *this;
}
void FitterPdf::InitPointers()
{
	Fitter::InitNULL(pFitter, pPdf, pYld);
}
void FitterPdf::Clean()
{
	Fitter::Delete(pPdf);
	InitPointers();
}






Fitter::Fitter(){ InitPointers(); }
Fitter::Fitter(const Fitter& other){ InitPointers(); *this = other;}
Fitter::~Fitter(){ Clean(); }

Fitter& Fitter::operator =(const Fitter& other)
{
	Clean();
	InitPointers();
 
	trees = other.trees;
	branch = other.branch;
	unit = other.unit;
	range = other.range;
	plotTitle = other.plotTitle;
	axisLabel = other.axisLabel;
	resultsFilename = other.resultsFilename;
	weightsFilename = other.weightsFilename;
	sharedParams = other.sharedParams;
	sigPdfs = other.sigPdfs;
	bkgPdfs = other.bkgPdfs;
	
	for(auto& sig : {true, false})
	for(auto& pdf : sig?sigPdfs:bkgPdfs)
		pdf.pFitter = this;

	return *this;
}

void Fitter::InitPointers()
{
	InitNULL(pFitVar, pData, pTotPdf, pSPlot, pResult);
}

void Fitter::Clean()
{
	Delete(pFitVar, pData, pTotPdf, pSPlot);
	Delete(params);
}

ostream& Fitter::Print(ostream& os) const
{
	int w = 17;
	ostringstream oss;
	
	oss << left << fixed << setprecision (2);
		
	oss << setw(w) << "Data:";
	for(auto& tree : trees) oss << "" << tree << endl << setw(w);
	oss << setw(w) << "Branch name:" << branch << " (" << unit << ')' << endl;
	oss << setw(w) << "Fit interval:" << range.minimum << ' ' << unit << " < " << branch << " < " << range.maximum << ' ' << unit << endl;
	oss << setw(w) << "Signals:";
	for(auto& sig : sigPdfs) oss << "" << sig << endl << setw(w);
	if(sigPdfs.empty()) oss << "None" << endl;
	oss << setw(w) << "Backgrounds:";
	for(auto& bkg : bkgPdfs) oss << "" << bkg << endl << setw(w);
	if(bkgPdfs.empty()) oss << "None" << endl;
	oss << setw(w) << "Output file:" << resultsFilename << endl;
	oss << setw(w) << "sWeight file:" << (weightsFilename.empty()? "None" : weightsFilename) << endl;
	oss << setw(w) << "Plot title:" << plotTitle << endl;
	oss << setw(w) << "Axis label:" << axisLabel << endl;

	return os << oss.str();
}

int Fitter::AddData(const string& filename, const string& tree)
{
	TFile* pFile = new TFile(filename.c_str());
	TTree* pTree = pFile? (TTree*) pFile->Get(tree.c_str()) : NULL;
	string path = filename + (filename.back()!='/' && tree.front()!='/' ? "/" : "") + tree;
	
	if(pFile) delete pFile;	
	if(!pFile){ cerr << "Error opening " << filename << endl; return 1; }
	if(!pTree){ cerr << "Error opening tree " << tree << " in " << filename << endl; return 1; }
	
	trees.push_back(path);
	return 0;
}

void Fitter::SetBranch(const string& branch, const string& unit, const Interval& range)
{
	this->branch = branch;
	this->unit = unit;
	this->range = range;
}

void Fitter::SetPlot(const string& plotTitle, const string& axisLabel)
{
	this->plotTitle = plotTitle;
	this->axisLabel = axisLabel;
}

void Fitter::SetResultsFilename(const string& resultsFilename){ this->resultsFilename = resultsFilename; }
void Fitter::SetWeightsFilename(const string& weightsFilename){ this->weightsFilename = weightsFilename; }


int Fitter::AddSignal(const string& pdfName, const vector<FitterParam>& params)
{
	FitterPdf pdf(this, "sig" + Int2String(sigPdfs.size()), pdfName, params);
	int err = pdf.Check();
	
	if(err) return err;
	sigPdfs.push_back(pdf);
	return 0;
}

int Fitter::AddBackground(const string& pdfName, const vector<FitterParam>& params)
{
	FitterPdf pdf(this, "bkg" + Int2String(bkgPdfs.size()), pdfName, params);
	int err = pdf.Check();
	
	if(err) return err;
	bkgPdfs.push_back(pdf);
	return 0;
}


int Fitter::Run(bool verbose, bool recreate)
{
	RooMsgService& ms = RooMsgService::instance();
	
	Clean();
	if(LoadData() || InitPdfs()) return 1;
	
	ms.saveState();
	if(!verbose)
	{
		for(int i=0; i<ms.numStreams(); ++i)
			ms.setStreamStatus(i, false);
		ms.setSilentMode(true);
	}
	
	Fit(verbose);
	SaveResults(recreate);
	
	if(!weightsFilename.empty())
	{
		CalculateWeights();
		SaveWeights();
	}
	
	ms.restoreState();	
	
	return 0;
}

// This function returns a pointer to a tree that contains all the input trees
TChain* Fitter::CreateDataChain()
{
	TChain* pChain = new TChain();
	
	for(auto& tree : trees)
	{
		if(pChain->Add(tree.c_str(), -1)) continue;
		
		cerr << "Error adding " << tree << " to TChain" << endl;
		delete pChain;
		return NULL;
	}
	
	return pChain;
}

// This function loads the data into pData
int Fitter::LoadData()
{
	TChain* pChain = CreateDataChain();
	Long64_t nEntries = pChain? pChain->GetEntries() : 0;
	BranchProxy<double> bp;
	
	if(!pChain) return 1;
	
	pFitVar = new RooRealVar(branch.c_str(), axisLabel.c_str(), range.minimum, range.maximum, unit.c_str());
	pData = new RooDataSet("data", "data", *pFitVar);
	pChain->SetBranchStatus("*", 0);
	bp.Connect(pChain, branch.c_str());
	
	for(Long64_t i=0; i<nEntries; ++i)
	{
		pChain->GetEntry(i);
		if(!range.In(bp)) continue;
		*pFitVar = bp;
		pData->add(*pFitVar);
	}
	
	delete pChain;	
	return 0;
}


int Fitter::InitPdfs()
{
	RooArgList pdfs;
	RooArgList ylds;
	
	for(auto& sig : {true, false})
	for(auto& pdf : sig?sigPdfs:bkgPdfs)
	{
		if(pdf.Init()) return 1;
		pdfs.add(pdf.GetPdf());
		ylds.add(pdf.GetYld());
	}
	pTotPdf = new RooAddPdf("pdf", "pdf", pdfs, ylds);

	return 0;
}

void Fitter::Fit(bool verbose)
{
	pResult = pTotPdf->fitTo(*pData, RooFit::Extended(), RooFit::Timer(false), RooFit::Save(true), RooFit::PrintLevel(verbose?1:-1));
	pResult->Print("v");
	cout << endl;
}

void Fitter::SaveResults(bool recreate)
{
	TFile* pFile = new TFile(resultsFilename.c_str(), recreate?"RECREATE":"UPDATE");
	
	// Create the plots
	RooPlot* pFrame = pFitVar->frame(RooFit::Bins(50), RooFit::Name(branch.c_str()), RooFit::Title(plotTitle.c_str()));
	RooPlot* pFramePull = pFitVar->frame(RooFit::Bins(50), RooFit::Name((branch+"_Pull").c_str()), RooFit::Title((plotTitle+" Pull").c_str()));
	RooHist* pHisPull = NULL;
	RooArgList bkgComponent;
	double pullMax = 0.;
	
	pData->plotOn(pFrame, RooFit::MarkerSize(0.9));
	for(auto& pdf : bkgPdfs) bkgComponent.add(pdf.GetPdf());
	pTotPdf->plotOn(pFrame, RooFit::Components(bkgComponent), RooFit::LineColor(kGreen) );
	
	pTotPdf->plotOn(pFrame, RooFit::LineColor(kRed));	
	pTotPdf->paramOn(pFrame, RooFit::Format("NEU", RooFit::AutoPrecision(1)), RooFit::Layout(0.65, 0.99, 0.93));
	pFrame->getAttText()->SetTextSize(0.75*pFrame->GetYaxis()->GetLabelSize());
	
	pHisPull = pFrame->pullHist();
	pFramePull->addPlotable(pHisPull, "P");
	pullMax = max(fabs(pFramePull->GetMinimum()), pFramePull->GetMaximum());
	pFramePull->GetYaxis()->SetRangeUser(-pullMax,pullMax);
	
	
	// Combine the plots
	TCanvas* pCanvas;
	TPad* pPadTop;
	TPad* pPadBottom;
	TF1* pFuncDummy;
	pCanvas = new TCanvas(branch.c_str(), branch.c_str(), 800, 600);
	pPadTop = new TPad("upperPad", "upperPad", 0., .2, 1., 1.);
	pPadBottom = new TPad("lowerPad", "lowerPad", 0., 0., 1., .2);
	pFuncDummy = new TF1("dummyfunc", "0", range.minimum, range.maximum);
		
	pPadTop->Draw();
	pPadBottom->Draw();
	
	pPadTop->cd();
	pFrame->Draw();
	
	pPadBottom->cd();
	pPadBottom->SetTopMargin(0.5*pPadBottom->GetBottomMargin());
	pPadBottom->SetTicks(1, 1);
	pFramePull->SetTitle("");
	pFramePull->GetYaxis()->SetNdivisions(205, true);	
	pFramePull->GetYaxis()->SetLabelSize(pFrame->GetYaxis()->GetLabelSize()*pPadTop->GetAbsHNDC()/pPadBottom->GetAbsHNDC());
	pFramePull->GetXaxis()->SetLabelSize(0.);
	pFramePull->GetXaxis()->SetTitleSize(0.);
	pFramePull->Draw();
	pFuncDummy->Draw("same");
	
	pCanvas->Write();
	
	delete pFrame;
	delete pFramePull;
	delete pFile;
	delete pFuncDummy;
	delete pPadBottom;
	delete pPadTop;
	delete pCanvas;
}

void Fitter::CalculateWeights()
{
	FixParameters(true);
	pSPlot = new RooStats::SPlot("SPlot object", "SPlot Object", *pData, pTotPdf, GetYieldsList());
	FixParameters(false);
}

int Fitter::SaveWeights()
{
	struct NameValTuple {string name; double val;};
	TFile* pFile = new TFile(weightsFilename.c_str(), "RECREATE");
	TChain* pChain = pFile? CreateDataChain() : NULL;
	TTree* pTree = NULL;
	Long64_t nEntries = 0, iEntry=0;
	BranchProxy<double> bp;
	vector<NameValTuple> nameValTuples;
	
	if(!pFile){ cerr << "Error creating " << weightsFilename << endl; return 1; }
	if(!pChain) return 1;
	
	// Deactivate branches that would have the same name as the added branches
	for(auto& sig : {true, false})
	for(auto& pdf : sig?sigPdfs:bkgPdfs)
	{
		NameValTuple nv = {pdf.GetYld().GetName()};
		string swBranchName = nv.name+"_sw";
		if(pChain->GetBranch(swBranchName.c_str()))
			pChain->SetBranchStatus(swBranchName.c_str(), 0);
		nameValTuples.push_back(nv);
	}
	
	pTree = pChain->CloneTree(0);
	bp.Connect(pChain, branch.c_str());
	nEntries = pChain->GetEntries();
	
	for(auto& nv : nameValTuples)
	{
		string swBranchName = nv.name + "_sw";
		string branchTitle = swBranchName + "/D";
		
		pTree->Branch(swBranchName.c_str(), &nv.val, branchTitle.c_str());	
	}
	
	for(Long64_t i=0; i<nEntries; ++i)
	{
		pChain->GetEntry(i);
		if(!range.In(bp)) continue;
		
		for(auto& nv : nameValTuples) 
			nv.val = pSPlot->GetSWeight(iEntry, (nv.name+"_sw").c_str());
		
		++iEntry;
		pTree->Fill();
	}
	pTree->AutoSave();
	
	delete pFile;
	delete pChain;
	
	return 0;
}

void Fitter::FixParameters(bool fix)
{
	for(auto& p : params)
	{
		string name = p->GetName();
		RooRealVar* pVar = dynamic_cast<RooRealVar*>(p);
		
		if(name.substr(name.size()-6) == "_yield") continue;
		if(pVar == NULL) continue;
		pVar->setConstant(fix);
	}
}

RooArgList Fitter::GetYieldsList()
{
	RooArgList yldList;
	
	for(auto& sig : {true, false})
	for(auto& pdf : sig?sigPdfs:bkgPdfs)
		yldList.add(pdf.GetYld());
	
	return yldList;
}









































