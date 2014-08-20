#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <tuple>

#include "Fitter.h"
#include "Interval.h"
#include "ArgParser.h"

using namespace std;

void PrintUsage();

struct KeyArgPair { KeyArgPair(){} KeyArgPair(const string& key, const string& arg):key(key),arg(arg){} string key, arg; };
string Trim(const string& str);
queue<KeyArgPair> CreateKeyLineQueue(const string& filename);
int ReadConfigurationFile(const string& filename, vector<Fitter>& fitters);

int main(int argc, char** argv)
{
	bool argErr = argc == 1 || argc > 3;
	bool showRooFitOutput = false;
	string configFilename;
	
	if(argc == 2) configFilename = argv[1];
	if(argc == 3)
	{
		if(argv[1] != string("-v")) argErr = true;
		showRooFitOutput = true;
		configFilename = argv[2];
	}
	
	if(argErr)
	{
		PrintUsage();
		return 0;
	}


	vector<Fitter> fitters;
	
	if(ReadConfigurationFile(configFilename, fitters)) return 0;
	
	for(auto& fitter : fitters)
	{
		cout << fitter;
		fitter.Run(showRooFitOutput);
	}
	
	return 0;
}

void PrintUsage()
{
	cerr << "Usage: fitter [-v] config_file                                                  " << endl \
	     << "Fits signal and background pdfs to unbinned data using RooFit.                  " << endl \
		 << "                                                                                " << endl \
	     << "  -v          : Enables the RooFit output                                       " << endl \
	     << "  config_file : Location where to find the configuration file                   " << endl;
}

string Trim(const string& str)
{
	string trimmed_str;
	try
	{
		const string whiteSpace = " \t\r";
		size_t a = str.find_first_not_of(whiteSpace);
		size_t b = str.find_last_not_of(whiteSpace);
		return str.substr(a, b-a+1);
	}
	catch(out_of_range& e){ return ""; }		
}

queue<KeyArgPair> CreateKeyLineQueue(const string& filename)
{
	ifstream ifs(filename);
	queue<KeyArgPair> kaps;
	string key, line;
	
	if(!ifs){ cerr << "Error opening " << filename << endl; return kaps; }
	
	while(ifs >> key && getline(ifs, line))
	{
		if(key[0] == '#') continue;
		kaps.emplace(key, Trim(line));
	}
	
	return kaps;
}



int ReadConfigurationFile(const string& filename, vector<Fitter>& fitters)
{
	queue<KeyArgPair> lines = CreateKeyLineQueue(filename);
	Fitter baseFitter;
	Fitter* pFitter = NULL;
	
	for(; !lines.empty(); lines.pop())
	{
		const string& key = lines.front().key; 
		const string& arg = lines.front().arg; 
		
		if(key == "DATA")
		{
			ArgParser ap(arg);
			if(pFitter){ cerr << "Unexpected " << key << ' ' << arg << endl; return 1; }
			if(ap.NumArgs() != 2){ cerr << "Wrong number of arguments in " << key << ' ' << arg << endl; return 1; }
			baseFitter.AddData(ap.Get<string>(0), ap.Get<string>(1));
			continue;
		}
		
		if(key == "OUTPUT")
		{
			ArgParser ap(arg);
			if(pFitter){ cerr << "Unexpected " << key << ' ' << arg << endl; return 1; }
			if(ap.NumArgs() != 1){ cerr << "Wrong number of arguments in " << key << ' ' << arg << endl; return 1; }
			baseFitter.SetResultsFilename(ap.Get<string>(0));
			
			TFile* pFile = new TFile(ap.Get<string>(0).c_str(), "RECREATE");
			if(pFile) delete pFile;
			
			continue;
		}
		
		if(key == "FIT")
		{
			ArgParser ap(arg);
			tuple<string, double, double, string, string, string> argVals; //branchname range_min range_max unit plot_title x_axis_label
			
			fitters.push_back(baseFitter);
			pFitter = &fitters.back();
			if(ap.NumArgs() != 6){ cerr << "Wrong number of arguments in " << key << ' ' << arg << endl; return 1; }
			if(ap.Scan(get<0>(argVals), get<1>(argVals), get<2>(argVals), get<3>(argVals), get<4>(argVals), get<5>(argVals)))
			{ 
				cerr << "Error parsing " << key << ' ' << arg << endl; 
				return 1; 
			}
			pFitter->SetBranch(get<0>(argVals), get<3>(argVals), Interval(get<1>(argVals), get<2>(argVals)));
			pFitter->SetPlot(get<4>(argVals), get<5>(argVals));
			continue;
		}
		
		if(key == "SWEIGHTS")
		{
			ArgParser ap(arg);
			if(!pFitter){ cerr << "Expecting FIT before SWEIGHTS" << endl; return 1; }
			if(ap.NumArgs() != 1){ cerr << "Wrong number of arguments in " << key << ' ' << arg << endl; return 1; }
			pFitter->SetWeightsFilename(ap.Get<string>(0));
			continue;
		}
		
		if(key == "SIGNAL" || key == "BACKGROUND")
		{
			string pdf;
			vector<FitterParam> params;
			int err = 0;
			
			if(!pFitter){ cerr << "Expecting FIT before " << key << ' ' << arg << endl; return 1; }
			if(!(istringstream(arg) >> pdf)){ cerr << "Wrong number of arguments in " << key << ' ' << arg << endl; return 1; }
			if(FitterParam::ParseArgs(arg.substr(pdf.size()), params)){cerr << "Error parsing " << key << ' ' << arg << endl; return 1; }
			
			err = key == "SIGNAL"? pFitter->AddSignal(pdf, params) : pFitter->AddBackground(pdf, params);
			switch(err)
			{
				case 0: break;
				case 1: cerr << "Unknown pdf in " << key << ' ' << arg << endl; return 1;
				case 2: cerr << "Wrong number of arguments in " << key << ' ' << arg << endl; return 1;
				default: break;
			}

			continue;
		}
	}
	
	return 0;
}







































