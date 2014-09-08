/// @file secateur.cpp
/// @author Kevin Heijhoff
/// @date   July 2014
/// @brief This program prunes trees in ROOT files and copies selected histograms

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <utility>
#include <map>
#include <functional>
#include <cstring>
#include <deque>
#include <tuple>
#include <algorithm>
#include <stdexcept> 
#include <stack> 

#include "SelectDialog.h"

#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TH1.h"
#include "TKey.h"

#include "TGFileDialog.h"

using namespace std;

// Todo: Reimplement the BranchInfo and Object classes sometime in the future. They got a little out of hand.

/// \class BranchInfo
/// This class contains the name and title of a branch. The title names the leafs and their types. Most trees have branches with only one leaf.
struct BranchInfo
{
	BranchInfo(const string& name, const string& title):name(name),title(title){}
	
	friend ostream& operator<<(ostream& os, const BranchInfo& b);
	
	/// This function is to cast a BranchInfo object to a string. This is needed for the SelectDialogFilter function in ItemSelector.h.
	operator string(){ ostringstream oss; oss << *this; return oss.str(); }
	
	bool operator==(const BranchInfo& br) const { return name==br.name; }
	
	string name, title;
	static map<char, string> types;
};
map<char, string> BranchInfo::types = {{'C',"Char_t*"},{'B',"Char_t"},{'b',"UChar_t"},{'S',"Short_t"},{'s',"UShort_t"},{'I',"Int_t"},{'i',"UInt_t"},{'F',"Float_t"},{'D',"Double_t"},{'L',"Long64_t"},{'l',"ULong64_t"},{'O',"Bool_t"}};

/// This functions lets us insert BranchInfo objects to a output stream. It will use the title to deduce its type (This currently only works for branches with one leaf).
ostream& operator<<(ostream& os, const BranchInfo& b)
{
	size_t i = b.title.find_first_of('[');
	size_t j = b.title.find_last_of(']');
	size_t k = b.title.find_last_of('/');
	char typeKey = k==string::npos?'F':b.title[k+1];
	return os << left << setw(10) << b.types[typeKey] << b.name << (i==j?"":b.title.substr(i, j-i+1));
}


struct TreeInfo
{
	TreeInfo(const vector<BranchInfo>& branches=vector<BranchInfo>(), const vector<string>& selects=vector<string>()):branches(branches),selects(selects){}

	string GetSelection()
	{		
		auto glue = [](const vector<string>& strs, const string& glue)
		{
			int i=0;
			ostringstream oss;
			for(auto& str : strs)
			{
				if(str.empty()) continue;
				oss << (i++?glue:string()) << "(" << str << ")";
			}
			return oss.str();
		};
		
		return glue(selects, " && ");
	}
	
	vector<BranchInfo> branches;
	vector<string> selects;
};


/// \class Object 
/// This class stores the path and type of either a tree or histogram.
struct Object
{
	/// Copy constructor, because we allocate some memory for the branches.
	Object(const Object& obj):
	path(obj.path),
	type(obj.type),
	//pBranches(obj.pBranches?new vector<BranchInfo>(*obj.pBranches):NULL),
	pTreeInfo(obj.pTreeInfo?new TreeInfo(*obj.pTreeInfo):NULL)
	{}
	
	/// Assignment operator, because we allocate some memory for the branches.
	Object& operator=(const Object& obj)
	{ 
		//if(pBranches){ delete pBranches; pBranches=NULL; } 
		if(pTreeInfo){ delete pTreeInfo; pTreeInfo=NULL; } 
		path=obj.path; 
		type=obj.type; 
		//if(obj.pBranches) pBranches = new vector<BranchInfo>(*obj.pBranches); 
		if(obj.pTreeInfo) pTreeInfo = new TreeInfo(*obj.pTreeInfo); 
		return *this; 
	}
	
	bool operator==(const Object& obj) const { return path == obj.path && type == obj.type; }
	
	/// Constructor for a histogram (pBranches will be NULL).
	Object(const string& path, const string& type):path(path),type(type),pTreeInfo(NULL){}
	
	/// Constructors for a tree
	Object(const string& path, const string& type, const vector<BranchInfo>& branches):
	path(path),type(type),pTreeInfo(new TreeInfo(branches)){}

	Object(const string& path, const string& type, const vector<BranchInfo>& branches, const vector<string>& selects):
	path(path),type(type),pTreeInfo(new TreeInfo(branches, selects)){}

	~Object()
	{ 
		//if(pBranches) delete pBranches; pBranches=NULL;
		if(pTreeInfo) delete pTreeInfo; pTreeInfo=NULL;
	}

	/// Check if the object is a tree.
	bool IsTree() const { return pTreeInfo!=NULL; }

	/// This functions lets us insert BranchInfo objects to a output stream. It will use the title to deduce its type (This currently only works for branches with one leaf).
	friend ostream& operator<<(ostream& os, const Object& obj){ return os << '(' << obj.type << ") " << obj.path; }

	/// This function is to cast an Object object to a string. This is needed for the SelectDialogFilter function in ItemSelector.h.
	operator string(){ ostringstream oss; oss << *this; return oss.str(); }

	/// Extracts the name from the path
	string Name() const { return string(&path[path.find_last_of('/')+1]); }

	/// Splits the path at '/' and returns all substrings in a vector, except for the last one.
	vector<string> GetDirectories() const
	{
		vector<string> dirs;
		if(!path.size()) return dirs;
		
		
		size_t a = path[0]=='/'?1:0;
		size_t b;
		
		while(true)
		{
			b = path.find('/',a);
			if(b == string::npos) return dirs;
			dirs.push_back(path.substr(a, b==string::npos?b:b-a));
			//if(b==path.find_last_of('/') || b == string::npos) return dirs;
			a = b+1;
		}
	}
	
	string GetPath() const
	{
		if(GetDirectories().size() == 0 && path[0]=='/') return path.substr(1);
		return path;
	}

	string path, type;
	//vector<BranchInfo>* pBranches;
	//vector<string>* pSelects;
	TreeInfo* pTreeInfo;
};




void PrintUsage();
vector<Object> CrawlDirectory(TDirectory* pDir);
vector<BranchInfo> GetVarInfo(TTree* pt);
template<class T> int SelectDialogFilter(const string& title, vector<T>& objects, const vector<T>& preselected_objects);
void CreateConfigFile(const string& configFilename, const string& inputFilename, const string& outputFilename, const vector<Object>& sel_objects);
int ReadConfigurationFile(const string& configFilename, string& inputFilename, string& outputFilename, vector<Object>& objects);
void Prune(const string& inputFilename, const string& outputFilename, const vector<Object>& objects);


int main(int argc, char** argv)
{	
	new TApplication("Secateur", NULL, NULL);
	gROOT->ProcessLine("gErrorIgnoreLevel = 3001;");
	
	
	// Interpret the command line arguments
	map<string, function<int(void)> > flagInterpreters;
	bool argErr = argc < 2;
	deque<string> args = argErr? deque<string>() : deque<string>(argv+1, argv+argc-1);
	
	string configFilename = argErr? "" : argv[argc-1];
	string 	inputFilename;
	string outputFilename;
	bool flag_modify = false;
	bool flag_create = false;
	
	auto readArgStr = [&args](string& str)
	{
		if(args.empty()) return 1;
		str = args.front();
		args.pop_front();
		return 0;
	};
	
	flagInterpreters["-c"] = [&](){ flag_create = true; return 0; };
	flagInterpreters["-m"] = [&](){ flag_modify = true; return 0; };
	flagInterpreters["-i"] = [&](){ return readArgStr( inputFilename); };
	flagInterpreters["-o"] = [&](){ return readArgStr(outputFilename); };
	
	while(!argErr && !args.empty())
	{
		string arg = args.front(); 
		args.pop_front();
		argErr = !flagInterpreters.count(arg) || flagInterpreters[arg]() != 0;
	}
	
	if(flag_create && flag_modify) argErr = true;
	if(argErr)
	{
		PrintUsage();
		return 0;
	}
	
	
	vector<Object> objects;
	
	// Read the configuration for pruning or modifying
	if(!flag_create && ReadConfigurationFile(configFilename, inputFilename, outputFilename, objects))
		return 0;
	
	// Create or modify the configuration file
	if(flag_create || flag_modify)
	{
		CreateConfigFile(configFilename, inputFilename, outputFilename, objects);
		return 0;
	}
		
	// Prune the ROOT file
	if(!flag_create && !flag_modify)
	{
		Prune(inputFilename, outputFilename, objects);
		return 0;
	}
		
	
	return 0;
}


void PrintUsage()
{
	cerr << "Usage: secateur [-c|-m] [-i input_file] [-o output_file] config_file            " << endl \
	     << "Copy selected histograms and subsets of trees from a root file to a new root    " << endl \
	     << "file. The configuration file specifies the input and output files as well as the" << endl \
	     << "histograms, trees and branches.                                                 " << endl \
		 << "                                                                                " << endl \
	     << "  -c             : Interactively generate config_file                           " << endl \
	     << "  -m             : Interactively modify config_file                             " << endl \
	     << "  -i input_file  : Overwrite the input file value in config_file                " << endl \
	     << "  -o output_file : Overwrite the output file value in config_file               " << endl \
	     << "  config_file    : Location where to find or create the configuration file      " << endl;
}


/// This function recursively finds all trees and histograms in a directory and its sub directories and returns their paths, types and branches in a vector of Objects.
///
/// @param pDir Directory in which to find all trees and histograms.
/// @return Vector of Objects. A Object is just a struct with some functionality that contains a path, type and possibly a vector of BranchInfos.
vector<Object> CrawlDirectory(TDirectory* pDir)
{
	static vector<string> pathVec;
	static vector<Object> objects; if(!pathVec.size()) objects.clear(); // If pathVec is empty, then this function is not called recursively
	
	// This function uses the strings in pathVec to make a path string: "/pathVec[0]/pathVec[1]/.../name"
	auto path = [](const string& name){
		ostringstream oss;
		//for_each(pathVec.begin(), pathVec.end(), [&oss](const string& dir){oss << '/' << dir;});
		for(auto& dir : pathVec) oss << '/' << dir;
		oss << '/' << name;
		return oss.str();
	};
	
	// Loop over all objects in the directory
	TIter itKey(pDir->GetListOfKeys());
	TKey* pKey = NULL;
	
	while((pKey = (TKey*) itKey()))
	{
		TObject* pObj = pKey->ReadObj();
		string name = pKey->GetName();
		
		// If this object is a sub directory, we also need to call this function for it
		if(pObj->IsA()->InheritsFrom("TDirectory"))
		{
			pathVec.emplace_back(name);
			CrawlDirectory(pDir->GetDirectory(name.c_str()));
			pathVec.pop_back();
			continue;
		}
		
		// Store the object's name and title if it is a tree or histogram.
		// If it is a tree, also store a vector of names and titles of its branches
		if(pObj->IsA()->InheritsFrom("TH1")) objects.emplace_back(path(name), pObj->ClassName());
		if(pObj->IsA()->InheritsFrom("TTree")) objects.emplace_back(path(name), pObj->ClassName(), GetVarInfo((TTree*) pObj));
	}
	
	return objects;
}


/// Parses all branches of a given tree and stores their names and titles into BranchInfo objects which are returned in a vector.
///
/// @param pt The Tree for which to get info on their branches.
/// @return Vector of BranchInfo objects. A BranchInfo is just a struct with some functionality that contains a name and title.
vector<BranchInfo> GetVarInfo(TTree* pt)
{
	TObjArray* branches = pt->GetListOfBranches();
	Int_t branches_n = branches->GetEntries();
	vector<BranchInfo> branchInfo;
	
	for(Int_t i=0; i<branches_n; ++i)
	{
		TBranch* pBranch = (TBranch*) branches->At(i);
		string name = pBranch->GetName();
		string title = pBranch->GetTitle();
		
		branchInfo.emplace_back(name, title);
	}
	
	return branchInfo;
}


/// Uses the SelectDialog class to let the user select object and then remove the unselected objects from the vector.
///
/// @param title 				Title of the dialog window.
/// @param objects 				The vector of object that will be shown in the list. Class  must implement the equal to operator and a conversion operator to a string.
/// @param preselected_objects 	The objects in this vector will be selected when the dialog window opens.
/// @return 					Return zero on success and nonzero if the user cancelled or closed the window.
template<class T> 
int SelectDialogFilter(const string& title, vector<T>& objects, const vector<T>& preselected_objects)
{
	vector<SelectDialogEntry> entries;
	bool cancelled = false;
	vector<T> objects_out;
	int id = 0;
	auto isSelected = [&](const T& obj)
	{
		for(auto& sel_obj : preselected_objects)
			if(sel_obj == obj) 
				return true;
		return false;
	};
	
	for(auto& obj : objects) 
		entries.emplace_back(obj, isSelected(obj), id++);
	
	SelectDialog(title, entries, cancelled);
	if(cancelled) return 1;
	
	for(auto& entry : entries)
		if(entry.selected)
			objects_out.emplace_back(objects[entry.id]);
			
	objects = objects_out;
	
	return 0;
}

/// Creates a new configuration file which specifies what histograms, trees and branches to copy from a ROOT file into a new ROOT file.
///
/// This function lists the objects and branches and asks the user to select which ones to keep. 
///
/// @param configFilename 	Where to store the generated configuration file.
/// @param inputFilename 	The ROOT file that contains the trees that need to be pruned.
/// @param outputFilename 	The ROOT file that will be created when the generated configuration file is run.
void CreateConfigFile(const string& configFilename, const string& inputFilename, const string& outputFilename, const vector<Object>& sel_objects)
{
	// Let the user select input and output ROOT files if they are not specified
	string  inFilename =  inputFilename;
	string outFilename = outputFilename;
	TGFileInfo fileInfo;
	const char* fileTypes[] = {"ROOT files", "*.root", 0, 0};
	string current_dir = gSystem->pwd(); // The file dialog can change this, so we need to remember it
	
	fileInfo.fFileTypes = fileTypes;
	fileInfo.fIniDir = NULL;
	
	if(inFilename.empty())
	{
		new TGFileDialog(gClient->GetDefaultRoot(), gClient->GetDefaultRoot(), kFDOpen, &fileInfo);
		if(!fileInfo.fFilename)
		{
			if(fileInfo.fMultipleSelection) cerr << "Multiple files are not supported" << endl;
			else cout << "Canceled by user" << endl;
			return;
		}
		inFilename = fileInfo.fFilename;
	}
	
	if(fileInfo.fFilename)
	{	
		delete fileInfo.fFilename;
		fileInfo.fFilename = NULL;
	}
	
	if(outFilename.empty())
	{
		new TGFileDialog(gClient->GetDefaultRoot(), gClient->GetDefaultRoot(), kFDSave, &fileInfo);
		if(!fileInfo.fFilename)
		{
			cout << "Canceled by user" << endl;
			return;
		}
		outFilename = fileInfo.fFilename;
		if(outFilename.size() < 5 || outFilename.substr(outFilename.size()-5) != ".root") 
			outFilename += ".root";
	}
	
	gSystem->cd(current_dir.c_str());
	

	// Open the input ROOT file and ask the user which objects and branches to keep
	TFile fileIn(inFilename.c_str());
	ostringstream oss_tree;
	ostringstream oss_hist;	
	vector<Object> objects;

	if(!fileIn.IsOpen())
	{
		cerr << "Error opening " << inFilename << endl;
		return;
	}
	
	objects = CrawlDirectory(&fileIn); // This gets a vector of info on all trees, histograms and branches	

	// This function pops up a listbox and lets the user select entries
	if(SelectDialogFilter("Secateur - Select objects", objects, sel_objects))
	{
		cout << "Canceled by user" << endl;
		return;
	}
	
	// Loop over all selected objects
	for(auto& obj : objects)
	{
		// If this object is a histogram, output a configuration line to oss_hist
		if(!obj.IsTree())
		{
			oss_hist << "HIST " << obj.path << endl;
			continue;
		}
		
		// This is a tree. Let the user select which branches to keep
		const auto& iSel_obj = find(sel_objects.begin(), sel_objects.end(), obj);
		vector<BranchInfo> dummy_branches;
		bool preselected_branches = iSel_obj != sel_objects.end() && iSel_obj->pTreeInfo!=NULL;
				
		if(SelectDialogFilter("Secateur - Select branches in " + obj.Name(), obj.pTreeInfo->branches, preselected_branches? iSel_obj->pTreeInfo->branches : dummy_branches))
		{
			cout << "Canceled by user" << endl;
			return;
		}
		
		if(obj.pTreeInfo->branches.empty()) continue; // No branches were selected, so skip this tree
		
	
		// Output the configuration lines for this tree
		oss_tree << "TREE " << obj.path << endl;
		
		// Also keep the selection fields (the cuts)
		if(preselected_branches)
			for(auto& select : iSel_obj->pTreeInfo->selects)
				oss_tree << "\tSELECT " << select << endl;		
		
		for(auto& br : obj.pTreeInfo->branches)
			oss_tree << "\tBRANCH " << br.name << endl;
		oss_tree << endl;
	}
	
	// Stop if the user didn't select any trees or histograms
	if(oss_tree.str().empty() && oss_hist.str().empty())
	{
		cerr << "Nothing selected" << endl;
		return;
	}
	
	
	// Write the configuration file
	ofstream of(configFilename);
	char date[32];
	time_t rawtime;
	
	if(!of)
	{
		cerr << "Error opening " << configFilename << endl;
		return;
	}
	
	time(&rawtime);
	strftime(date, 32, "%FT%T%z", localtime(&rawtime));
	
	of << "# Secateur configuration file generated on " << date << endl << endl;
	of << "INPUT  " << inFilename << endl;
	of << "OUTPUT " << outFilename << endl << endl;
	if(oss_hist.str().size()) of << oss_hist.str() << endl;
	if(oss_tree.str().size()) of << oss_tree.str() << endl;
	
	of.close();	
}


/// Reads the configuration file
///
/// @param configFilename 	The filename of the configuration file to read
/// @param inputFilename 	This will contain the input ROOT filename specified in the configuration file
/// @param outputFilename 	This will contain the output ROOT filename specified in the configuration file
/// @param objects 			This will contain the paths to tree and histogram and names of branches to copy
/// @return 				returns 0 on success and a nonzero value otherwise.
int ReadConfigurationFile(const string& configFilename, string& inputFilename, string& outputFilename, vector<Object>& objects)
{
	ifstream ifs(configFilename);
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

	// Define the interpreters
	keyfnc["INPUT" ] = [&](const string& str){ if( inputFilename.empty())  inputFilename = str; };
	keyfnc["OUTPUT"] = [&](const string& str){ if(outputFilename.empty()) outputFilename = str; };
	keyfnc["HIST"  ] = [&](const string& histPath){ objects.emplace_back(histPath, "TH1"); };
	keyfnc["TREE"  ] = [&](const string& treePath)
	{ 
		vector<BranchInfo> branches;
		vector<string> selects;
		
		keyfnc["BRANCH"] = [&](const string& branchName){ branches.emplace_back(branchName, ""); };
		keyfnc["SELECT"] = [&](const string& select    ){  selects.emplace_back(select);         };
						
		while(parse({"BRANCH", "SELECT"}));		
		if(branches.empty()) return;
		objects.emplace_back(treePath, "TTree", branches, selects);
	};
	
	if(!ifs){ cerr << "Error opening " << configFilename << endl; return 1; }
	
	while(parse({"INPUT", "OUTPUT", "TREE"}));
	
	if( inputFilename.empty()){ cerr << "No input ROOT file specified in "  << configFilename << endl; return 1; }
	if(outputFilename.empty()){ cerr << "No output ROOT file specified in " << configFilename << endl; return 1; }
	
	// If we did not reach the EOF, then the configuration file contains gibberish
	if(ifs){ cerr << "Error parsing " << configFilename << endl; return 1; }
	
	return 0;
}

/// Prunes the specified ROOT file
///
/// @param configFilename Filename of the configuration file
void Prune(const string& inputFilename, const string& outputFilename, const vector<Object>& objects)
{
	// Prune the ROOT file
	TFile fileIn(inputFilename.c_str());
	TFile fileOut(outputFilename.c_str(), "RECREATE");
	TDirectory* pDirIn = &fileIn;
	TDirectory* pDirOut = &fileOut;
	
	for(auto& obj : objects)
	{
		// First create the directories in which the current object (obj) is stored
		string fullPath = "/";
		
		for(auto& dirname : obj.GetDirectories())
		{
			TDirectory* pDirTemp = NULL;
			
			pDirIn = pDirIn->GetDirectory(dirname.c_str());
			if(!pDirIn){ cerr << "Error opening " << inputFilename << ":" << fullPath << dirname << endl; return; }
			pDirTemp = pDirOut->GetDirectory(pDirIn->GetName());
			pDirOut = pDirTemp? pDirTemp : pDirOut->mkdir(pDirIn->GetName(), pDirIn->GetTitle());	
			fullPath += dirname + "/";
		}
		pDirOut->cd(); // Make sure the new objects will be stored in the correct place
		
		if(obj.IsTree())
		{
			TTree* pTreeIn = (TTree*) fileIn.Get(obj.GetPath().c_str());
			TTree* pTreeOut = NULL;
			string select = obj.pTreeInfo->GetSelection();
			
			if(!pTreeIn){ cerr << "Error opening " << inputFilename << ":" << obj.GetPath() << endl; return; }	
			
			// Activate selected branches - if branches consist of only one and this is named ALL activate every branch, else only the selected
            if(obj.pTreeInfo->branches.size() == 1 && obj.pTreeInfo->branches.back().name == "ALL") pTreeIn->SetBranchStatus("*", 1);
            else{
			pTreeIn->SetBranchStatus("*", 0);
			for(auto& br : obj.pTreeInfo->branches)
				pTreeIn->SetBranchStatus(br.name.c_str(), 1);
            }
			
			if(select.empty()) // No selections
			{
				pTreeOut = pTreeIn->CloneTree(0);
				pTreeOut->SetAutoSave(); // Auto save every 300MB
				pTreeOut->CopyEntries(pTreeIn, -1, "fast");	// "fast" means direct copy of data
			}
			else
			{
				pTreeOut = pTreeIn->CopyTree(select.c_str(), "fast");
				if(pTreeOut == NULL)
				{
					cerr << "Error in selection " << select << endl;
					return;
				}
			}
			pTreeOut->AutoSave(); // Save any unsaved data
		}
		else
		{
			TH1* pHisIn = (TH1*) fileIn.Get(obj.GetPath().c_str());
			TH1* pHisOut = NULL;
			
			if(!pHisIn){ cerr << "Error opening " << inputFilename << ":" << obj.GetPath() << endl; return; }
			pHisOut = (TH1*) pHisIn->Clone(pHisIn->GetName());
			pHisOut->Write();
			delete pHisOut;
		}
		
		// Set pDirs back to the root of the ROOT files	
		pDirIn = &fileIn;
		pDirOut = &fileOut;
	}
	
	fileOut.Close();
	fileIn.Close();
}










































