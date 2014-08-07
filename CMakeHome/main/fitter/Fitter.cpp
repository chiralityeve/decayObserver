#include "Fitter.h"

using namespace std;

Fitter::Fitter(){ InitPointers(); }

Fitter::Fitter(const string& rootFilename, const string& treeName, 
               const string& branchName, const Interval& range, const string& branchUnit, 
			   const string& plotTitle, const string& plotAxisLabel, const string& outputFilename):
//rootFilenames({rootFilename}),treeNames({treeName}),
branchName(branchName),range(range),branchUnit(branchUnit),
plotTitle(plotTitle),plotAxisLabel(plotAxisLabel),outputFilename(outputFilename),
fileTreePairs({{rootFilename, treeName}})
{
	InitPointers();
}

Fitter::Fitter(const vector<string>& rootFilenames, const vector<string>& treeNames, 
               const string& branchName, const Interval& range, const string& branchUnit, 
			   const string& plotTitle, const string& plotAxisLabel, const string& outputFilename):
//rootFilenames(rootFilenames),treeNames(treeNames),
branchName(branchName),range(range),branchUnit(branchUnit),
plotTitle(plotTitle),plotAxisLabel(plotAxisLabel),outputFilename(outputFilename)
{
	InitPointers();
	AddData(rootFilenames, treeNames);
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
	InitNULL(pFitVar, pData, pTotPdf, pResult, pSPlot, pFileOut, pBaseDir);
}

void Fitter::Clean()
{
	Delete(pFileOut);
	Delete(pData);
	Delete(pFitVar);
	Delete(vec_pParam);
	Delete(pTotPdf);
	Delete(pSPlot);
	InitPointers();
}

Fitter& Fitter::operator =(const Fitter& other)
{
	Clean();
	
	fileTreePairs = other.fileTreePairs;
	//rootFilenames = other.rootFilenames;
	//treeNames = other.treeNames;
	branchName = other.branchName;
	range = other.range;
	branchUnit = other.branchUnit;
	plotTitle = other.plotTitle;
	plotAxisLabel = other.plotAxisLabel;
	outputFilename = other.outputFilename;
	sigPdfs = other.sigPdfs;
	bkgPdfs = other.bkgPdfs;
	outputSweights = other.outputSweights;
	
	return *this;
}

ostream& Fitter::Print(ostream& os) const
{
		int w = 17;
		ostringstream oss;
		
		oss << left << fixed << setprecision (2);
			
		oss << setw(w) << "Data:";
		//int nFiles = min(rootFilenames.size(),treeNames.size());
		//for(int i=0; i<nFiles; ++i)	oss << "" << rootFilenames[i] << ':' << treeNames[i] << endl << setw(w);
		for(auto& ft : fileTreePairs) oss << "" << ft.filename << ':' << ft.treeName << endl << setw(w);
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

int Fitter::OpenOutputFile(bool overwrite)
{
	pFileOut = new TFile(outputFilename.c_str(), overwrite?"RECREATE":"UPDATE");
	if(!pFileOut){ cerr << "Error opening " << pFileOut << endl; return 1; }
	pBaseDir = pFileOut->mkdir(branchName.c_str());
	pBaseDir->cd();
	
	return 0;
}
void Fitter::CloseOutputFile()
{
	if(!pFileOut) return;
	pFileOut->Delete("ProcessID*;1");
	delete pFileOut;
	pFileOut = NULL;
	pBaseDir = NULL;
}

void Fitter::ChangeDirectory(const string& name)
{
	if(name.empty()){ pBaseDir->cd(); return; }
	if(!pBaseDir->GetDirectory(name.c_str())) pBaseDir->mkdir(name.c_str());
	pBaseDir->cd(name.c_str());
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
		ms.setSilentMode(true);
	}

	if(LoadData()) return 1;
	if(InitPdfs()) return 1;
		
	cout << "Fitting pdf" << endl;
	
	pResult = pTotPdf->fitTo(*pData, RooFit::Extended(), RooFit::Timer(false), RooFit::Save(true), RooFit::PrintLevel(showRooFitOutput?1:-1));
	pResult->Print("v");
	
	OpenOutputFile(overwrite);
	Plot();
	
	if(!outputSweights.empty())
	{
		Calculate_sWeights();
		Plot_sWeights();
		Plot_sWeighted();
		if(Save_sWeights()) return 1;
	}
	
	CloseOutputFile();
	ms.restoreState();	
	return 0;
}

void Fitter::Plot()
{	
	// Make the plots
	RooPlot* pFrame = NULL;
	RooPlot* pFramePull = NULL;
	
	MakePlots("tot_pdf", plotTitle, pFrame, pFramePull);
	
	// Save the plots
	ChangeDirectory("raw");
	pFrame->Write();
	pFramePull->Write();
	
	
	// Put Them in a canvas
	PlotCanvas pc;
	CombinePlots("tot_pdf", pc, pFrame, pFramePull);
	
	// Save the canvas
	ChangeDirectory();
	pc.pCanvas->Write();
	
	delete pFrame;
	delete pFramePull;
		
	return;	
}

void Fitter::MakePlots(const string& name, const string& title, RooPlot*& pPlot1, RooPlot*& pPlot2, RooAbsPdf* pPdf, RooDataSet* pData)
{
	bool plotbkg = false;
	RooPlot* pFrame = pPlot1? pPlot1 : pFitVar->frame(RooFit::Bins(50), RooFit::Name(name.c_str()), RooFit::Title(title.c_str()));
	RooPlot* pFramePull = pPlot2? pPlot2 : pFitVar->frame(RooFit::Bins(50), RooFit::Name((name+"_Pull").c_str()), RooFit::Title((title+" Pull").c_str()));
	RooHist* pHisPull = NULL;
	double pullMax = 0.;
	
	if(!pPdf && !pData) plotbkg = true;
	if(!pPdf) pPdf = pTotPdf;
	if(!pData) pData = this->pData;
	
	pData->plotOn(pFrame, RooFit::MarkerSize(0.9));
	
	if(plotbkg)
	{
		RooArgList bkgComponent;
		for(auto& pdf : bkgPdfs) bkgComponent.add(pdf.GetPdf());
		pTotPdf->plotOn(pFrame, RooFit::Components(bkgComponent), RooFit::LineColor(kGreen) );
	}
	
	pPdf->plotOn(pFrame, RooFit::LineColor(kRed));	
	pPdf->paramOn(pFrame, RooFit::Format("NEU", RooFit::AutoPrecision(1)), RooFit::Layout(0.65, 0.99, 0.93));
	pFrame->getAttText()->SetTextSize(0.75*pFrame->GetYaxis()->GetLabelSize());
	
	pHisPull = pFrame->pullHist();
	pFramePull->addPlotable(pHisPull, "P");
	pullMax = max(fabs(pFramePull->GetMinimum()), pFramePull->GetMaximum());
	pFramePull->GetYaxis()->SetRangeUser(-pullMax,pullMax);
	
	pPlot1 = pFrame;
	pPlot2 = pFramePull;
}


void Fitter::CombinePlots(const string& name, PlotCanvas& pc, RooPlot*& pPlot1, RooPlot*& pPlot2)
{
	pc.pCanvas = new TCanvas(name.c_str(), name.c_str(), 800, 600);
	pc.pPadTop = new TPad("upperPad", "upperPad", 0., .2, 1., 1.);
	pc.pPadBottom = new TPad("lowerPad", "lowerPad", 0., 0., 1., .2);
	pc.pFuncDummy = new TF1("dummyfunc", "0", range.minimum, range.maximum);
		
	pc.pPadTop->Draw();
	pc.pPadBottom->Draw();
	
	pc.pPadTop->cd();
	pPlot1->Draw();
	
	pc.pPadBottom->cd();
	pc.pPadBottom->SetTopMargin(0.5*pc.pPadBottom->GetBottomMargin());
	pc.pPadBottom->SetTicks(1, 1);
	pPlot2->SetTitle("");
	pPlot2->GetYaxis()->SetNdivisions(205, true);	
	pPlot2->GetYaxis()->SetLabelSize(pPlot1->GetYaxis()->GetLabelSize()*pc.pPadTop->GetAbsHNDC()/pc.pPadBottom->GetAbsHNDC());
	pPlot2->GetXaxis()->SetLabelSize(0.);
	pPlot2->GetXaxis()->SetTitleSize(0.);
	pPlot2->Draw();
	pc.pFuncDummy->Draw("same");
}

int Fitter::InitChain(TChain* pChain)
{
	for(auto& ft : fileTreePairs)
		if(!pChain->Add(ft.GetPath().c_str())){ cerr << "Error adding " << ft.GetPath() << " to TChain" << endl; return 1; }

	return 0;
}

int Fitter::LoadData()
{
	TChain* pChain = new TChain;
	Long64_t nEntries = 0;
	BranchProxy<double> bp;

	pFitVar = new RooRealVar(branchName.c_str(), plotAxisLabel.c_str(), range.minimum, range.maximum, branchUnit.c_str());
	pData = new RooDataSet("data", "data", *pFitVar);
	
	cout << "Loading data" << endl;
	
	if(InitChain(pChain)) return 1;	
	pChain->SetBranchStatus("*", 0);
	bp.Connect(pChain, branchName.c_str());
	nEntries = pChain->GetEntries();
	
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
/*
int Fitter::LoadData()
{
	LoadData2(); return 1;
	enum BranchType:int{ CHAR_T, UCHAR_T, SHORT_T, USHORT_T, INT_T, UINT_T, FLOAT_T, DOUBLE_T, LONG64_T, ULONG64_T, BOOL_T };
	auto getBranchType = [](TBranch* pBranch)
	{
		const unordered_map<char, BranchType> branchTypeMap = {{'C',CHAR_T},{'B',CHAR_T},{'b',UCHAR_T},{'S',SHORT_T},{'s',USHORT_T},{'I',INT_T},{'i',UINT_T},{'F',FLOAT_T},{'D',DOUBLE_T},{'L',LONG64_T},{'l',ULONG64_T},{'O',BOOL_T}};
		string str=pBranch->GetTitle(); 
		size_t p=str.find_last_of('/'); 
		return p==string::npos? FLOAT_T : branchTypeMap.at(str[p+1]); 
	};
	
	//int nFiles = min(rootFilenames.size(), treeNames.size());
	cout << "Loading data" << endl;
	
	pFitVar = new RooRealVar(branchName.c_str(), plotAxisLabel.c_str(), range.minimum, range.maximum, branchUnit.c_str());
	pData = new RooDataSet("data", "data", *pFitVar);
	//pData = new RooDataSet(pTree->GetName(), pTree->GetTitle(), pTree, RooArgSet(*pFitVar));	
	
	//for(int i=0; i<nFiles; ++i)
	for(auto& ft : fileTreePairs)
	{
		const string& rootFilename = ft.filename; //rootFilenames[i];
		const string& treeName = ft.treeName; //treeNames[i];
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
			ft.entriesInRange.push_back(j);
		}
		
		delete pFile;
	}
	
	return 0;
}*/

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
	
void Fitter::Calculate_sWeights()
{	
	FixParameters();	
	pSPlot = new RooStats::SPlot("SPlot object", "SPlot Object", *pData, pTotPdf, GetYieldsList());
	FixParameters(false);
	
	return;
	
	/*for(auto& pVec_pdfs : {&sigPdfs, &bkgPdfs})
	for(auto& pdf : *pVec_pdfs)
	{
		RooAbsReal& yield = pdf.GetYld();
		
		cout << yield.GetName() << endl;
		cout << "            Yield: " << yield.getVal() << endl;
		cout << "  sWeighted Yield: " << pSPlot->GetYieldFromSWeight(yield.GetName()) << endl << endl;
		cout << "       Difference: " << pSPlot->GetYieldFromSWeight(yield.GetName()) - yield.getVal() << endl;
	}*/
}

void Fitter::FixParameters(bool fix)
{
	for(auto& pVec_pdfs : {&sigPdfs, &bkgPdfs})
	for(auto& pdf : *pVec_pdfs)
		pdf.FixParameters(fix);
}

RooArgList Fitter::GetYieldsList()
{
	RooArgList yldList;
	for(auto& pVec_pdfs : {&sigPdfs, &bkgPdfs})
	for(auto& pdf : *pVec_pdfs)
		yldList.add(pdf.GetYld());
		
	return yldList;
}

void Fitter::Plot_sWeighted()
{
	for(auto& pVec_pdfs : {&sigPdfs, &bkgPdfs})
	for(auto& pdf : *pVec_pdfs)
	{
		//TFile* pFile = new TFile(outputFilename.c_str(), "UPDATE");
		RooDataSet* pDataSig = new RooDataSet("data_sWeighted", "data_sWeighted", pData, *pData->get(), 0, (pdf.GetYld().GetName() + string("_sw")).c_str());
		//RooDataSet* pDataBkg = new RooDataSet("data_bkg", "data_bkg", pData, *pData->get(), 0, "bkg0_yield_sw");
		
		RooPlot* pFrameData = NULL;
		RooPlot* pFramePull = NULL;
		//TCanvas* pCanvas = NULL;
		
		MakePlots(pdf.GetPdf().GetName() + string("_sWeighted"), plotTitle+" ("+pdf.GetPdf().GetName()+" sWeighted)", pFrameData, pFramePull, &(pdf.GetPdf()), pDataSig);	
		//MakePlots("_weighted", plotTitle+" weighted", pFrameData, pFramePull, &(bkgPdfs[0].GetPdf()), pDataBkg);
		
		ChangeDirectory("raw");
		pFrameData->Write();
		pFramePull->Write();
		
		ChangeDirectory();
		PlotCanvas pc;
		CombinePlots(pdf.GetPdf().GetName() + string("_sWeighted"), pc, pFrameData, pFramePull);
		pc.pCanvas->Write();
		
		//CombinePlots(branchName+"_weighted", pCanvas, pFrameData, pFramePull);
		

		//TDirectory* pPlotDir = pFile->GetDirectory("plots_sWeighted");
		//if(!pPlotDir) pPlotDir = pFile->mkdir("plots_sWeighted");
		
		//pPlotDir->cd();
		//pCanvas->Write();
			
		
		//delete pCanvas;
		//delete pFrameData;
		//delete pFramePull;
		delete pDataSig;
		//delete pDataBkg;
	}
}

void Fitter::Plot_sWeights()
{
	vector<TGraph*> vec_pGraph;
	vector<vector<double>* > vec_pVec_values;
	Interval yRange(0., 0.);
	
	for(auto& pVec_pdfs : {&sigPdfs, &bkgPdfs})
	for(auto& pdf : *pVec_pdfs)
	{
		RooAbsReal& yield = pdf.GetYld();	
		int nEvents = pData->numEntries();
		vector<double>* px = new vector<double>;
		vector<double>* py = new vector<double>;
		TGraph* pGraph = NULL;
		vector<tuple<double, double> > points(nEvents, make_tuple(0., 0.));
				
		for(int i=0; i<nEvents; ++i) //nSWeights
		{
			double x = pData->get(i)->getRealValue(pFitVar->GetName());
			double y = pSPlot->GetSWeight(i, yield.GetName());
			
			points[i] = make_tuple(x, y);
			
			if(y > yRange.maximum) yRange.maximum = y;
			if(y < yRange.minimum) yRange.minimum = y;
		}
		
		sort(points.begin(), points.end(), [](const tuple<double, double>& p, const tuple<double, double>& q){ return get<0>(p) < get<0>(q); });
		for(auto& p : points)
		{
			px->push_back(get<0>(p));
			py->push_back(get<1>(p));
		}
		
		pGraph = new TGraph(nEvents, &px->at(0), &py->at(0));		
		
		vec_pGraph.push_back(pGraph);
		vec_pVec_values.push_back(px);
		vec_pVec_values.push_back(py);
	}
	
	yRange = GetNiceLimits(yRange);
	
	
	//TFile* pFile = new TFile(outputFilename.c_str(), "UPDATE");
	//TDirectory* pSPlotDir = pFile->GetDirectory("sWeightPlots");
	TCanvas* pCanvas = new TCanvas("sWeight_plot", "sWeight_plot", 800, 600);
	TLegend* pLegend = new TLegend(0.65, 0.65, 0.99, 0.93);
	
	//if(!pSPlotDir) pSPlotDir = pFile->mkdir("sWeightPlots");
	
	pLegend->SetTextSize(0.035);
	for(size_t i=0; i<vec_pGraph.size(); ++i)
	{
		TGraph* pGraph = vec_pGraph[i];
		RooAbsReal& yield = (i<sigPdfs.size()?sigPdfs[i]:bkgPdfs[i-sigPdfs.size()]).GetYld();
		pGraph->SetLineWidth(2);
		//pGraph->GetHistogram()->SetMarkerColorAlpha(kWhite, 1.);
		pGraph->GetHistogram()->SetFillColor(10);
		pGraph->SetFillColor(10);
		pGraph->GetHistogram()->SetTitle(("sWeight vs " + plotAxisLabel).c_str());
		pGraph->SetLineColor(i+1);
		pGraph->GetXaxis()->SetTitle(plotAxisLabel.c_str());
		pGraph->GetHistogram()->SetMinimum(yRange.minimum);
		pGraph->GetHistogram()->SetMaximum(yRange.maximum);
		pGraph->GetHistogram()->GetYaxis()->SetTitle("sWeight");
		pGraph->Draw(i?"same":"");
		pLegend->AddEntry(pGraph, (yield.GetName()+string("_sw")).c_str());
		pLegend->Draw("L");
	}
	
	ChangeDirectory();
	pCanvas->Write();
	
	delete pLegend;
	delete pCanvas;
	//delete pFile;
	for(auto& pVec : vec_pVec_values) delete pVec;
	for(auto& pGraph : vec_pGraph) delete pGraph;
}

Interval Fitter::GetNiceLimits(const Interval& r, const int nd)
{
	auto getScExp = [](double x){ return (int) floor(log10(fabs(x))); };
	int n = max(getScExp(r.minimum), getScExp(r.maximum));
	double s = pow(10., nd - n - 1);
	double t = 1./s;
	
	return Interval(floor(r.minimum*s - 0.5)*t, ceil(r.maximum*s + 0.5)*t);
}

int Fitter::Save_sWeights()
{
	struct NameValTuple {string name; double val;};
	TFile* pFile = new TFile(outputSweights.c_str(), "RECREATE");
	TChain* pChain = new TChain;
	TTree* pTree = NULL;
	Long64_t nEntries = 0, iEntry=0;
	BranchProxy<double> bp;
	vector<NameValTuple> nameValTuples;
	
	if(!pFile){ cerr << "Error creating file" << endl; return 1; }
	if(InitChain(pChain)) return 1;	
	
	// Deactivate branches that would have the same name as the added branches
	for(auto& pVec_pdfs : {&sigPdfs, &bkgPdfs})
	for(auto& pdf : *pVec_pdfs)
	{
		NameValTuple nv = {pdf.GetYld().GetName()};
		string swBranchName = nv.name+"_sw";
		if(pChain->GetBranch(swBranchName.c_str()))
			pChain->SetBranchStatus(swBranchName.c_str(), 0);
		nameValTuples.push_back(nv);
	}
	
	pTree = pChain->CloneTree(0);
	bp.Connect(pChain, branchName.c_str());
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

































