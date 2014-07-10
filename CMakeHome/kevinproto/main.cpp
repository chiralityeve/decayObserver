#include <iostream>
#include <cstdio>
#include "TROOT.h"
#include "TChain.h"
#include "TCanvas.h"
//#include "TLorentzVector.h"

using namespace std;

int main(int argc, char** argv)
{
	TChain* data = new TChain();
	TCanvas* canvas = new TCanvas("canvas");
		
	data->Add("/afs/cern.ch/work/n/nfomin/public/bs2jpsiphi/2011MagDown/bs2jpsiphifinal.root/Bs2phimumuTuple/DecayTree");
	
	// Todo: set branch addresses for variables I would like to read and loop through the events
	//data->SetBranchAddress("B0_ETA",&B0_ETA);
	//data->SetBranchStatus("*", false);
		
	
	data->Draw("B0_M");
	canvas->Print("../plots/Plot.pdf");
	
	delete data;
	
	
	return 0;
}