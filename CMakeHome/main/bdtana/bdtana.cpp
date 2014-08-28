#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "BdtAnalyser.h"
#include "ArgParser.h"

using namespace std;

void PrintUsage();
BdtAnalyser* CreateBdtAnalyser(const string& configFilename);

int main(int argc, char** argv)
{
	bool argTrain = false;
	bool argApply = false;
	string configFilename;
	BdtAnalyser* pBdt = NULL;
	
	if(argc == 3) 
	{
		argTrain = argv[1] == string("-t") || argv[1] == string("-train");
		argApply = argv[1] == string("-a") || argv[1] == string("-apply");
		configFilename = argv[2];
	}
	
	if(argTrain == argApply)
	{
		PrintUsage();
		return 0;
	}
	
	pBdt = CreateBdtAnalyser(configFilename);
	if(!pBdt) return 0;
	
	if(argTrain) pBdt->Train();
	if(argApply) pBdt->Apply();
	
	
	return 0;
}

void PrintUsage()
{
	cerr << "Usage: bdtana [-t|-a] config_file                                               " << endl \
	     << "Trains and applies a BDT with the settings in config_file                       " << endl \
		 << "                                                                                " << endl \
	     << "  -t          : Train the BDT                                                   " << endl \
	     << "  -a          : Apply the BDT                                                   " << endl \
	     << "  config_file : Location where to find the configuration file                   " << endl;
}

BdtAnalyser* CreateBdtAnalyser(const string& configFilename)
{
	BdtAnalyser* pBdt = new BdtAnalyser;
	ifstream ifs(configFilename);
	string line;
	
	while(getline(ifs, line))
	{
		ArgParser ap(line);
		int nArgs = ap.NumArgs();
		string key = nArgs? ap.Get<string>(0) : "";
		auto nArgErrorMsg = [&key,&line](){ cerr << "Wrong number of arguments for BDT: " << line << endl; };
		
		if(key[0] == '#' || !nArgs--) continue;
		
		if(key == "SIGNAL")
		{
			if(nArgs < 2 || nArgs > 3){ nArgErrorMsg(); break; }
			pBdt->AddSignal(ap.Get<string>(1), ap.Get<string>(2), nArgs==3? ap.Get<string>(3) : "");
			continue;
		}
		
		if(key == "BACKGROUND")
		{
			if(nArgs < 2 || nArgs > 3){ nArgErrorMsg(); break; }
			pBdt->AddBackground(ap.Get<string>(1), ap.Get<string>(2), nArgs==3? ap.Get<string>(3) : "");
			continue;
		}
		
		if(key == "DATA")
		{
			if(nArgs != 2){ nArgErrorMsg(); break; }
			pBdt->AddData(ap.Get<string>(1), ap.Get<string>(2));
			continue;
		}
		
		if(key == "TRAIN")
		{
			if(nArgs != 1){ nArgErrorMsg(); break; }
			pBdt->SetTrainOutput(ap.Get<string>(1));
			continue;
		}
		
		if(key == "APPLY")
		{
			if(!nArgs || nArgs > 2){ nArgErrorMsg(); break; }
			pBdt->SetApplyOutput(ap.Get<string>(1), nArgs==2? ap.Get<string>(2) : "");
			continue;
		}
		
		if(key == "FACTORY")
		{
			if(nArgs != 1){ nArgErrorMsg(); break; }
			pBdt->SetFactoryOptions(ap.Get<string>(1));
			continue;
		}
		
		if(key == "BDT")
		{
			if(nArgs != 1){ nArgErrorMsg(); break; }
			pBdt->SetBdtOptions(ap.Get<string>(1));
			continue;
		}
		
		if(key == "READER")
		{
			if(nArgs != 1){ nArgErrorMsg(); break; }
			pBdt->SetReaderOptions(ap.Get<string>(1));
			continue;
		}
		
		if(key == "SAMPLES")
		{
			if(nArgs != 1){ nArgErrorMsg(); break; }
			pBdt->SetSampleOptions(ap.Get<string>(1));
			continue;
		}
		
		if(key == "VAR" || key == "BRANCH")
		{
			if(nArgs != 1){ nArgErrorMsg(); break; }
			pBdt->AddVariable(ap.Get<string>(1));
			continue;
		}
	}
	
	if(ifs)
	{
		delete pBdt;
		return NULL;
	}
	
	return pBdt;
}







































