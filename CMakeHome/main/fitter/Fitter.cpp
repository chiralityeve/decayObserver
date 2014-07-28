#include "Fitter.h"
/*
Fitter::Fitter():
rootFilename(),treeName(),
branchName(),range(),branchUnit(),
plotTitle(),plotAxisLabel(),outputFilename(),
sigPdfs(),bkgPdfs(),

//pBkgPdf(NULL),
vec_pParam()//pBkgYield(NULL),
//pTotPdf(NULL)
{
	InitPointers();
}

Fitter::Fitter(const string& rootFilename, const string& treeName, 
               const string& branchName, const Interval& range, const string& branchUnit, 
			   const string& plotTitle, const string& plotAxisLabel, const string& outputFilename):
rootFilename(rootFilename),treeName(treeName),
branchName(branchName),range(range),branchUnit(branchUnit),
//sigTypes(),sigLocations(),sigBounds(),
//bkgType(bkgType),
plotTitle(plotTitle),plotAxisLabel(plotAxisLabel),outputFilename(outputFilename),

//pFile(NULL),pTree(NULL),

//pFitVar(),
//pData(NULL),
//vec_pSigPdf(),vec_pSigYield(),
sigPdfs(),bkgPdfs(),
//pBkgPdf(NULL),
vec_pParam()//pBkgYield(NULL),
//pTotPdf(NULL)
{
	InitPointers();
}*/

Fitter::Fitter(){ InitPointers(); }

Fitter::Fitter(const string& rootFilename, const string& treeName, 
               const string& branchName, const Interval& range, const string& branchUnit, 
			   const string& plotTitle, const string& plotAxisLabel, const string& outputFilename):
rootFilename(rootFilename),treeName(treeName),
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
	InitNULL(pFile, pTree, pFitVar, pData, pTotPdf, pResult);
}

void Fitter::Clean()
{
	Delete(pData);
	Delete(pFitVar);
	Delete(pFile);
	Delete(vec_pParam);
	Delete(pTotPdf);
	InitPointers();
}

Fitter& Fitter::operator =(const Fitter& other)
{
	Clean();
	
	rootFilename = other.rootFilename;
	treeName = other.treeName;
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
			
		oss << setw(w) << "Root file:" << rootFilename << endl;
		oss << setw(w) << "Tree name:" << treeName << endl;
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
		ms.addStream(RooFit::WARNING, RooFit::Prefix(false));
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
	auto int2str = [](int n){ ostringstream oss; oss << n; return oss.str(); };
	RooPlot* pFrame = pFitVar->frame(RooFit::Bins(50), RooFit::Name(branchName.c_str()), RooFit::Title(plotTitle.c_str()));
	//RooPlot* pFrame = pFitVar->frame(RooFit::Name(branchName.c_str()), RooFit::Title(plotTitle.c_str()));
	TFile* pFileOut = new TFile(outputFilename.c_str(), overwrite?"RECREATE":"UPDATE");
	TCanvas* pCanvas = new TCanvas(branchName.c_str(), branchName.c_str(), 1000, 600);
	RooArgList bkgComponent;
	
	for(auto& pdf : bkgPdfs)
		bkgComponent.add(pdf.GetPdf());

	pData->plotOn(pFrame, RooFit::MarkerSize(0.9));
	pTotPdf->paramOn(pFrame, RooFit::Format("NELU", RooFit::AutoPrecision(2)));
	pTotPdf->plotOn(pFrame, RooFit::Components(bkgComponent), RooFit::LineColor(kGreen) );
	pTotPdf->plotOn(pFrame, RooFit::LineColor(kRed));	
	pFrame->Draw();
	
	//RooPlot* frame2 = pFitVar->frame(RooFit::Bins(50), RooFit::Name(branchName.c_str()), RooFit::Title(plotTitle.c_str()));
	//pFrame->residHist()->Draw();
	//frame2->addObject(pFrame->residHist()) ;
	//frame2->SetMinimum(-5) ;
	//frame2->SetMaximum(+5) ;
	//frame2->Draw() ; 
	
	pCanvas->Write();
	
	delete pFileOut;
	delete pCanvas;
	Clean();
		
	return 0;	
}

int Fitter::LoadData()
{
	pFile = new TFile(rootFilename.c_str());
	if(!pFile){ cerr << "Error opening " << rootFilename << endl; return 1; }	
	
	pTree = (TTree*) pFile->Get(treeName.c_str());
	if(!pTree){ cerr << "Error opening " << rootFilename << ":" << treeName << endl; return 1; }

	cout << "Loading data" << endl;
	
	pTree->SetBranchStatus("*", 0);
	pTree->SetBranchStatus(branchName.c_str(), 1);
	
	pFitVar = new RooRealVar(branchName.c_str(), plotAxisLabel.c_str(), range.minimum, range.maximum, branchUnit.c_str());
	pData = new RooDataSet(pTree->GetName(), pTree->GetTitle(), pTree, RooArgSet(*pFitVar));
	
	return 0;
}

int Fitter::InitPdfs()
{
	auto AddSerial = [](const string& name)
	{
		static map<string, int> serials;
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
	







































