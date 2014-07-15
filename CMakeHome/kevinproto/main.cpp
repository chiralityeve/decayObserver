#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <utility>
#include "TROOT.h"
#include "TApplication.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
//#include "TLorentzVector.h"

using namespace std;

bool PrintProgress(Long64_t i, Long64_t n);

Double_t CalcDistSqrd(Double_t* a, Double_t* b)
{
	Double_t d = 0.;
	for(int i=0; i<2; ++i)
	{
		Double_t di = *(a+i) - *(b+i);
		d += di*di;
	}
	return d;
}

int main(int argc, char** argv)
{
	TApplication app("", &argc, argv);
	TChain* data = new TChain();
	TCanvas* canvas1 = new TCanvas("canvas1", "canvas1");
	TCanvas* canvas2 = new TCanvas("canvas2", "canvas2");
	TCanvas* canvas3 = new TCanvas("canvas3", "canvas3");
	const int n_bins_0 = 80;
	const int n_bins_5 = 80;
	const pair<double, double> range_0 = make_pair(5200., 6000.);
	const pair<double, double> range_5 = make_pair(1600., 2200.);
	TH1D* hist_0 = new TH1D("", "", n_bins_0, range_0.first, range_0.second);
	TH1D* hist_5 = new TH1D("", "", n_bins_5, range_5.first, range_5.second);
	TH2D* scat_05 = new TH2D("", "", n_bins_0 ,range_0.first, range_0.second, n_bins_5, range_5.first, range_5.second);

	Double_t lab0_MM, lab1_MM, lab5_MM;

	data->Add("/afs/cern.ch/work/k/kheijhof/public/Lb2LcKKpi/LambdaB2LcHHH.root/LambdaBOfflineTree/DecayTree");
	
	data->SetBranchStatus("*", false);
	data->SetBranchAddress("lab0_MM", &lab0_MM);
	data->SetBranchAddress("lab1_MM", &lab1_MM);
	data->SetBranchAddress("lab5_MM", &lab5_MM);
	data->SetBranchStatus("lab0_MM", true);
	data->SetBranchStatus("lab1_MM", true);
	data->SetBranchStatus("lab5_MM", true);
	
	Long64_t nData = data->GetEntries();
	for(Long64_t i=0; i<nData; ++i)
	{
		data->GetEntry(i);
		
		PrintProgress(i, nData);

		if(fabs(lab1_MM - 2286.46) > 10.) continue;
		if(fabs(lab5_MM - 1970.) < 20. ) continue;
		
		hist_0->Fill(lab0_MM);	
		hist_5->Fill(lab5_MM);	
		scat_05->Fill(lab0_MM, lab5_MM);		
	}
	
	canvas1->cd();
	hist_0->Draw();
	canvas1->Print("../plots/Plot_lab0_MM.pdf");
	
	canvas2->cd();
	hist_5->Draw();
	canvas2->Print("../plots/Plot_lab5_MM.pdf");
	
	canvas3->cd();
	scat_05->Draw("COLZ");
	canvas3->Print("../plots/Plot_lab0_MM_lab5_MM.pdf");
	
	
	scat_05->SaveAs("../plots/scat5.root");
	//app.Run();
	//cin.ignore();
	
	delete data;
	delete canvas1; delete canvas2; delete canvas3;
	delete hist_0; delete hist_5; delete scat_05;
	
	
	return 0;
}

bool PrintProgress(Long64_t i, Long64_t n)
{
	static clock_t t = clock();
	clock_t tNow = clock();
	ostringstream os;
	
	if(i!=0 && tNow - t < CLOCKS_PER_SEC/10 && i != n-1) return false;
	os << "\r" << fixed << setprecision(2) << setw(6) << i*100./n << "% ";
	cout << os.str() << flush;
	if(i == n-1) cout << endl;
	t = tNow;
	return true;
}