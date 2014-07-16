#ifndef ITEMSELECTOR_H
#define ITEMSELECTOR_H

#include <iostream>
#include <vector>
#include <map>
#include <iomanip>
#include <utility>
#include "TROOT.h"
#include "TApplication.h"
#include "TGClient.h"
#include "TGButton.h"
#include "TGListBox.h"
#include <TGTextEntry.h>
#include <TGLabel.h>
#include <TSystem.h>
#include "TList.h"
#include "RQ_OBJECT.h"

// rootcint -f ItemSelectorDict.cpp -c ItemSelector.h LinkDef.h

using namespace std;

class ItemSelector : public TGMainFrame
{
public:
	ItemSelector(vector<pair<string, bool> >& items, bool& canceled, const string& title);
	virtual ~ItemSelector();
	void Select();
	void Close();
	void InvertSelection();
	void Filter(char* str);
	void Cancel();
	

private:
	vector<pair<string, bool> >& items;
	TGListBox* pListBox;
	TGTextEntry* pTextEntry;
	bool& canceled;
	
	//vector<tuple<int, string, bool> > GetSelection();
	
	static TApplication* pApp;

	ClassDef(ItemSelector, 0)
	
	static int wid;
};


template<class T>
int SelectDialog(vector<pair<T,bool> >& objSelTuples, const string& title)
{
	bool canceled = false;
	vector<pair<string, bool> > items;
	int i = 0;
	
	if(!gApplication) new TApplication("", NULL, NULL);
	
	for(auto& objSel : objSelTuples)
		items.push_back(make_pair(objSel.first, objSel.second));
		
	new ItemSelector(items, canceled, title);
	gApplication->Run(1);
	
	for(auto& objSel : objSelTuples)
		objSel.second  = items[i++].second;

	
	return canceled?1:0;
}


#endif // ITEMSELECTOR_H
































