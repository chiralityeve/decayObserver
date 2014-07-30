#include "Fitter.h"

Fitter::Fitter(){ InitPointers(); }

Fitter::Fitter(const string& rootFilename, const string& treeName, 
               const string& branchName, const Interval& range, const string& branchUnit, 
			   const string& plotTitle, const string& plotAxisLabel, const string& outputFilename):
rootFilenames({rootFilename}),treeNames({treeName}),
branchName(branchName),range(range),branchUnit(branchUnit),
plotTitle(plotTitle),plotAxisLabel(plotAxisLabel),outputFilename(outputFilename)
{
	InitPointers();
}

Fitter::Fitter(const vector<string>& rootFilenames, const vector<string>& treeNames, 
               const string& branchName, const Interval& range, const string& branchUnit, 
			   const string& plotTitle, const string& plotAxisLabel, const string& outputFilename):
rootFilenames(rootFilenames),treeNames(treeNames),
branchName(branchName),range(range),branchUnit(branchUnit),
plotTitle(plotTitle),plotAxisLabel(plotAxisLabel),outputFilename(outputFilename)
{
	InitPointers();
}

Fitter::Fitter(const Fitter& other)
{
	InitPointers();
	*this = other;
}

Fitter::~Fitter()
{
	Clean();
}

void Fitter::InitPointers()
{
	InitNULL(pFitVar, pData, pTotPdf, pResult);
}

void Fitter::Clean()
{
	Delete(pData);
	Delete(pFitVar);
	Delete(vec_pParam);
	Delete(pTotPdf);
	InitPointers();
}

Fitter& Fitter::operator =(const Fitter& other)
{
	Clean();
	
	rootFilenames = other.rootFilenames;
	treeNames = other.treeNames;
	branchName = other.branchName;
	range = other.range;
	branchUnit = other.branchUnit;
	plotTitle = other.plotTitle;
	plotAxisLabel = other.plotAxisLabel;
	outputFilename = other.outputFilename;
	sigPdfs = other.sigPdfs;
	bkgPdfs = other.bkgPdfs;
	
	return *this;
}

ostream& Fitter::Print(ostream& os) const
{
		int w = 17;
		ostringstream oss;
		
		oss << left << fixed << setprecision (2);
			
		oss << setw(w) << "Data:";
		int nFiles = min(rootFilenames.size(),treeNames.size());
		for(int i=0; i<nFiles; ++i)	oss << "" << rootFilenames[i] << ':' << treeNames[i] << endl << setw(w);
		//for(auto& filename : rootFilenames)	oss << "" << filename << endl << setw(w);
		//oss << setw(w) << "Tree name:";
		//for(auto& treeName : treeNames)	oss << "" << treeName << endl << setw(w);
		oss << setw(w) << "Branch name:" << branchName << " (" << branchUnit << ')' << endl;
		oss << setw(w) << "Fit interval:" << range.minimum << ' ' << branchUnit << " < " << branchName << " < " << range.maximum << ' ' << branchUnit << endl;
		oss << setw(w) << "Signals:";
		for(auto& sig : sigPdfs) oss << "" << sig << endl << setw(w);
		if(sigPdfs.empty()) oss << "None" << endl;
		oss << setw(w) << "Backgrounds:";
		for(auto& bkg : bkgPdfs) oss << "" << bkg << endl << setw(w);
		if(bkgPdfs.empty()) oss << "None" << endl;
		oss << setw(w) << "Output file:" << outputFilename << endl;
		oss << setw(w) << "Plot title:" << plotTitle << endl;
		oss << setw(w) << "Axis label:" << plotAxisLabel << endl;

		return os << oss.str();
}

int Fitter::Fit(bool showRooFitOutput, bool overwrite)
{
	RooMsgService& ms = RooMsgService::instance();
	
	Clean();
	ms.saveState();
	if(!showRooFitOutput)
	{
		for(int i=0; i<ms.numStreams(); ++i)
			ms.setStreamStatus(i, false);
		//ms.addStream(RooFit::WARNING, RooFit::Prefix(false));
	}

	if(LoadData()) return 1;
	if(InitPdfs()) return 1;
		
	cout << "Fitting pdf" << endl;
	
	pResult = pTotPdf->fitTo(*pData, RooFit::Extended(), RooFit::Timer(true), RooFit::Save(true), RooFit::PrintLevel(showRooFitOutput?1:-1));
	pResult->Print("v");
	
	if(Plot(overwrite)) return 1;
	ms.restoreState();	
	return 0;
}




int Fitter::Plot(bool overwrite)
{	
	// Open the output file and open or create sub directories
	TFile* pFile = new TFile(outputFilename.c_str(), overwrite?"RECREATE":"UPDATE");
	TDirectory* pPlotDir = pFile->GetDirectory("plots");
	TDirectory* pRawDir = pFile->GetDirectory("rawRooPlots");
	
	if(!pPlotDir) pPlotDir = pFile->mkdir("plots");
	if(!pRawDir) pRawDir = pFile->mkdir("rawRooPlots");	
	
	
	// Make the plots
	RooPlot* pFrame = NULL;
	RooPlot* pFramePull = NULL;
	
	MakePlots(pFrame, pFramePull);
	
	// Save the plots
	pRawDir->cd();
	pFrame->Write();
	pFramePull->Write();
	
	
	// Put Them in a canvas
	TCanvas* pCanvas = new TCanvas(branchName.c_str(), branchName.c_str(), 800, 600);
	TPad* pPadTop = new TPad("upperPad", "upperPad", 0., .2, 1., 1.);
	TPad* pPadBottom = new TPad("lowerPad", "lowerPad", 0., 0., 1., .2);
	TF1 *pFuncDummy = new TF1("dummyfunc", "0", range.minimum, range.maximum);
	
	pPadTop->Draw();
	pPadBottom->Draw();
	
	pPadTop->cd();
	pFrame->Draw();
	
	pPadBottom->cd();
	pPadBottom->SetTopMargin(0.);
	pPadBottom->SetTicks(1, 1);
	pFramePull->SetTitle("");
	pFramePull->GetYaxis()->SetNdivisions(205, true);	
	pFramePull->GetYaxis()->SetLabelSize(pFrame->GetYaxis()->GetLabelSize()*pPadTop->GetAbsHNDC()/pPadBottom->GetAbsHNDC());
	pFramePull->GetXaxis()->SetLabelSize(0.);
	pFramePull->GetXaxis()->SetTitleSize(0.);
	pFramePull->Draw();
	pFuncDummy->Draw("same");
	
	// Save the canvas
	pPlotDir->cd();
	pCanvas->Write();

	
	delete pFrame;
	delete pFramePull;
	delete pPadTop;
	delete pPadBottom;
	delete pCanvas;
	delete pFuncDummy;
	delete pFile;
	
	Clean();
		
	return 0;	
}

void Fitter::MakePlots(RooPlot*& pPlot1, RooPlot*& pPlot2)
{
	RooPlot* pFrame = pFitVar->frame(RooFit::Bins(50), RooFit::Name(branchName.c_str()), RooFit::Title(plotTitle.c_str()));
	RooPlot* pFramePull = pFitVar->frame(RooFit::Bins(50), RooFit::Name((branchName+"_Pull").c_str()), RooFit::Title((plotTitle+" Pull").c_str()));
	RooHist* pHisPull = NULL;
	RooArgList bkgComponent;
	double pullMax = 0.;
	
	for(auto& pdf : bkgPdfs) bkgComponent.add(pdf.GetPdf());
	pData->plotOn(pFrame, RooFit::MarkerSize(0.9));
	pTotPdf->plotOn(pFrame, RooFit::Components(bkgComponent), RooFit::LineColor(kGreen) );
	pTotPdf->plotOn(pFrame, RooFit::LineColor(kRed));	
	pTotPdf->paramOn(pFrame, RooFit::Format("NEU", RooFit::AutoPrecision(1)), RooFit::Layout(0.65, 0.99, 0.93));
	pFrame->getAttText()->SetTextSize(0.75*pFrame->GetYaxis()->GetLabelSize());
	
	pHisPull = pFrame->pullHist();
	pFramePull->addPlotable(pHisPull, "P");
	pullMax = max(fabs(pFramePull->GetMinimum()), pFramePull->GetMaximum());
	pFramePull->GetYaxis()->SetRangeUser(-pullMax,pullMax);
	
	pPlot1 = pFrame;
	pPlot2 = pFramePull;
}

int Fitter::LoadData()
{
	enum BranchType:int{ CHAR_T, UCHAR_T, SHORT_T, USHORT_T, INT_T, UINT_T, FLOAT_T, DOUBLE_T, LONG64_T, ULONG64_T, BOOL_T };
	auto getBranchType = [](TBranch* pBranch)
	{
		const unordered_map<char, BranchType> branchTypeMap = {{'C',CHAR_T},{'B',CHAR_T},{'b',UCHAR_T},{'S',SHORT_T},{'s',USHORT_T},{'I',INT_T},{'i',UINT_T},{'F',FLOAT_T},{'D',DOUBLE_T},{'L',LONG64_T},{'l',ULONG64_T},{'O',BOOL_T}};
		string str=pBranch->GetTitle(); 
		size_t p=str.find_last_of('/'); 
		return p==string::npos? FLOAT_T : branchTypeMap.at(str[p+1]); 
	};
	
	int nFiles = min(rootFilenames.size(), treeNames.size());
	
	cout << "Loading data" << endl;
	
	pFitVar = new RooRealVar(branchName.c_str(), plotAxisLabel.c_str(), range.minimum, range.maximum, branchUnit.c_str());
	pData = new RooDataSet("data", "data", *pFitVar);
	//pData = new RooDataSet(pTree->GetName(), pTree->GetTitle(), pTree, RooArgSet(*pFitVar));	
	
	for(int i=0; i<nFiles; ++i)
	{
		const string& rootFilename = rootFilenames[i];
		const string& treeName = treeNames[i];
		TFile* pFile = new TFile(rootFilename.c_str());
		TTree* pTree = pFile? (TTree*) pFile->Get(treeName.c_str()) : NULL;
		TBranch* pBranch = pTree? pTree->GetBranch(branchName.c_str()) : NULL;
		BranchType branchType = pBranch? getBranchType(pBranch) : (BranchType)0;
		bool isDouble = branchType == DOUBLE_T;
		bool isFloat = branchType == FLOAT_T;
		union { Double_t d; Float_t f; } val;
		
		if(!pFile){ cerr << "Error opening " << rootFilename << endl; return 1; }	
		if(!pTree){ cerr << "Error opening " << rootFilename << ":" << treeName << endl; return 1; }
		if(!pBranch){ cerr << "Branch " << branchName << " does not exist " << endl; return 1; }	
		if(!(isDouble || isFloat)){ cerr << branchName << " is not a double or float" << endl; return 1; }
				
		pTree->SetBranchStatus("*", 0);
		pTree->SetBranchStatus(branchName.c_str(), 1);
		pTree->SetBranchAddress(branchName.c_str(), (void*) &val);
		
		Long64_t nEntries = pTree->GetEntries();
		for(Long64_t j=0; j<nEntries; ++j)
		{
			pTree->GetEntry(j);
			if(isFloat) val.d = (double) val.f;
			if(!range.In(val.d)) continue;
			*pFitVar = val.d;
			pData->add(*pFitVar);
		}
		
		delete pFile;
	}
	
	return 0;
}

int Fitter::InitPdfs()
{
	map<string, int> serials;
	auto AddSerial = [&serials](const string& name)
	{
		ostringstream oss;
		
		oss << name << serials[name]++;
		return oss.str();
	};
	
	const string sigName = "sig";
	const string bkgName = "bkg";
	int nSigs = sigPdfs.size();
	int nBkgs = bkgPdfs.size();
	int nData = pData->numEntries();
	RooArgList pdfs;
	RooArgList ylds;
		
	for(int i=0; i<2; ++i)
	{
		vector<FitterPdf>& vec_pdf = i? bkgPdfs : sigPdfs;
		const string& name = i? bkgName : sigName;
		
		for(auto& pdf : vec_pdf)
		{
			if(pdf.Init(AddSerial(name), *pFitVar, nSigs + nBkgs, nData)) return 1;
			pdfs.add(pdf.GetPdf());
			ylds.add(pdf.GetYld());
		}
	}
	
	pTotPdf = new RooAddPdf("pdf", "pdf", pdfs, ylds);
		
	return 0;
}
	







































