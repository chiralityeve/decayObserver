#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <functional>
#include <algorithm>
#include "Fitter.h"
#include "ArgParser.h"
#include "TFile.h"

using namespace std;

void PrintUsage();
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
		cout << fitter << endl;
		fitter.Fit(showRooFitOutput);
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

int ReadConfigurationFile(const string& filename, vector<Fitter>& fitters)
{
	ifstream ifs(filename);
	bool parseError = false;
	map<string, function<void(const string&)> > keyfnc; // Lambda expressions are used for parsing
	
	// Trims leading and trailing white space of str
	auto trim = [](const string& str) -> string
	{
		try
		{
			const string whiteSpace = " \t\r";
			size_t a = str.find_first_not_of(whiteSpace);
			size_t b = str.find_last_not_of(whiteSpace);
			return str.substr(a, b-a+1);
		}
		catch(out_of_range& e){ return ""; }	
	};
	
	// Checks if strs contains str
	auto contains = [](const vector<string>& strs, const string& str){ return find(strs.begin(), strs.end(), str) != strs.end(); };
	
	// Searches for the next key value pair and calls their interpreters if they are in the expected keys vector (expkeys)
	// Return true when the key matched and false otherwise.
	auto parse = [&](const vector<string>& expkeys)
	{
		static bool nextline = true;
		static string key;
		static string val;
		
		if(parseError) return false;

		while(ifs && (!nextline || (ifs>>key && getline(ifs, val))))
		{
			nextline = true; 					// Make sure that a new line will be read when we continue.
			if(key[0] == '#') continue;	 		// This is a comment, ignore it.
			if(!contains(expkeys, key)) break; 	// The key does not match any of the expected ones
			keyfnc[key](trim(val));
			return true;
		}
		nextline = false; // The key didn't match and we might want to try another set of keys later, so don't read a new line the next time.
		return false; // Fail		
	};
	
	auto parseAll = [&](const vector<string>& expkeys)
	{
		while(parse(expkeys));
	};


	string dataFilename, treeName, outputFilename;
	Fitter f;
	
	// Define the interpreters
	keyfnc["DATA"  ] = [&](const string& line)
	{
		if(ArgParser(line).Scan(f.rootFilename, f.treeName))
		{
			cerr << "Not enough arguments for DATA " << endl;
			parseError = true;
			return;
		}
	};
	keyfnc["OUTPUT"] = [&](const string& line)
	{ 
		f.outputFilename = line; 
		
		// Open and close with RECREATE to make an empty file
		TFile* pFile = new TFile(f.outputFilename.c_str(), "RECREATE");
		if(!pFile){ cerr << "Error opening " << f.outputFilename << endl; parseError = true; return; }	
		delete pFile;
	};
	keyfnc["FIT"   ] = [&](const string& line)
	{
		keyfnc["SIGNAL"] = [&](const string& line){ parseError = f.AddSignal(line); };
		keyfnc["BACKGROUND"] = [&](const string& line){ parseError = f.AddBackground(line); };
		
		if(ArgParser(line).Scan(f.branchName, f.range.minimum, f.range.maximum, f.branchUnit, f.plotTitle, f.plotAxisLabel))
		{
			cerr << "Not enough arguments for FIT " << f.branchName << " ..." << endl;
			parseError = true;
			return;
		}
		
		parseAll({"SIGNAL", "BACKGROUND"});
		
		fitters.push_back(f);
	};
	
	if(!ifs){ cerr << "Error opening " << filename << endl; return 1; }
	
	parseAll({"DATA", "OUTPUT"});
	
	if(  f.rootFilename.empty()){ cerr << "No data ROOT file specified in "   << filename << endl; return 1; }
	if(f.outputFilename.empty()){ cerr << "No output ROOT file specified in " << filename << endl; return 1; }
	
	parseAll({"DATA", "OUTPUT", "FIT"});
	
	// If we did not reach the EOF, then the configuration file contains gibberish
	if(parseError){ return 1; }
	if(ifs){ cerr << "Error parsing " << filename << endl; return 1; }
	
	return 0;
}




































