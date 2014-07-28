#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <functional>
#include <algorithm>
#include "ArgParser.h"

using namespace std;

int ReadConfigurationFile(const string& filename);

int main(int argc, char** argv)
{	
	string configFilename = "../kevinproto/parsing_config_files/example.conf";
	ReadConfigurationFile(configFilename);
}

int ReadConfigurationFile(const string& filename)
{
	ifstream ifs(filename);
	bool parseError = false;
	map<string, function<void(const string&)> > dispatch; // Lambda expressions are used for parsing
	
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
			dispatch[key](trim(val));
			return true;
		}
		nextline = false; // The key didn't match and we might want to try another set of keys later, so don't read a new line the next time.
		return false; // Fail		
	};
	
	auto parseAll = [&](const vector<string>& expkeys)
	{
		while(parse(expkeys));
	};

	
	string  inputFilename;
	string outputFilename;
	
	// Define the interpreters
	dispatch["INPUT" ] = [&](const string& line){  inputFilename = line; };
	dispatch["OUTPUT"] = [&](const string& line){ outputFilename = line; };
	
	dispatch["KEY_A" ] = [&](const string& line)
	{
		int int_a=0, int_b=0;
		double double_a=0., double_b=0.;
		string string_a, string_b;
		
		dispatch["SUBKEY_A"] = [&](const string& line)
		{
			if(ArgParser(line).Scan(int_a, double_a, string_a))
			{
				parseError = true;
				cerr << "Parsing error" << endl;
				return;
			}
			cout << int_a << ", " << double_a << ", " << string_a << endl;
		};
		dispatch["SUBKEY_B"] = [&](const string& line)
		{
			if(ArgParser(line).Scan(int_b, double_b, string_b))
			{
				parseError = true;
				cerr << "Parsing error" << endl;
				return;
			}
			cout << int_b << ", " << double_b << ", " << string_b << endl;
		};
				
		parseAll({"SUBKEY_A", "SUBKEY_B"});
	};
	
	dispatch["KEY_B" ] = [&](const string& line)
	{ 
		ArgParser ap(line);
		double x = ap.Get<double>();
		int y = ap.Get<int>();
		string z = ap.Get<string>();
		
		cout << x << ", " << y << ", " << z << endl;
	};
	
	if(!ifs){ cerr << "Error opening " << filename << endl; return 1; }
	parseAll({"INPUT", "OUTPUT"});
	
	if( inputFilename.empty()){ cerr << "No data ROOT file specified in "   <<  inputFilename << endl; return 1; }
	if(outputFilename.empty()){ cerr << "No output ROOT file specified in " << outputFilename << endl; return 1; }
	
	parseAll({"KEY_A", "KEY_B"});
	
	// If we did not reach the EOF, then the configuration file contains gibberish
	if(parseError){ return 1; }
	if(ifs){ cerr << "Error parsing " << filename << endl; return 1; }
	
	return 0;
}





































