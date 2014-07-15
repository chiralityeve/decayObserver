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
#include "TList.h"
#include "RQ_OBJECT.h"

// rootcint -f ItemSelectorDict.cpp -c ItemSelector.h LinkDef.h

using namespace std;

class ItemSelector : public TGMainFrame
{
public:
	ItemSelector(vector<pair<int, string> >& items, const string& title);
	virtual ~ItemSelector();
	void Select();
	void Close();
	void InvertSelection();
	

private:
	vector<pair<int, string> >& items;
	TGListBox* pListBox;
	
	vector<pair<int, string> > GetSelection();
	
	static TApplication* pApp;

	ClassDef(ItemSelector, 0)
};

void ItemSelectDialog(vector<pair<int, string> >& items, const string& title);

template<class T>
void SelectDialogFilter(vector<T>& objects, const string& title)
{
	vector<pair<int, string> > items;
	vector<T> objectsSelected;
	int n = objects.size();
	
	for(int i=0; i<n; ++i)
		items.emplace_back(i, objects[i]);

	ItemSelectDialog(items, title);
	
	for(auto it=items.begin(); it!=items.end(); ++it)
		objectsSelected.emplace_back(objects[it->first]);
	
	objects = objectsSelected;
	
}


#endif // ITEMSELECTOR_H
































