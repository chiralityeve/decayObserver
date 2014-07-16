#include "ItemSelector.h"

int ItemSelector::wid = 89;
ItemSelector::ItemSelector(vector<pair<string, bool> >& items, bool& canceled, const string& title):
TGMainFrame(gClient->GetRoot()),
items(items),
pListBox(new TGListBox(this, wid++)),
pTextEntry(NULL),
canceled(canceled)
{
	SetCleanup(kNoCleanup);
	
	TGHorizontalFrame* pHorFrame = new TGHorizontalFrame(this, 480, 20, kFixedWidth);
	TGTextButton* pButton = new TGTextButton(pHorFrame, "&Select");
	TGTextButton* pButtonInvert = new TGTextButton(pHorFrame, "&Invert Selection");
	TGTextButton* pButtonCancel = new TGTextButton(pHorFrame, "&Cancel");
		
	TGHorizontalFrame* pHorFrame2 = new TGHorizontalFrame(this, 480, 20, kFixedWidth);
	TGLabel *pLabel = new TGLabel(pHorFrame2, "Filter: ");
	pTextEntry = new TGTextEntry(pHorFrame2);
	pHorFrame2->AddFrame(pLabel, new TGLayoutHints(kLHintsLeft  | kLHintsCenterY));
	pHorFrame2->AddFrame(pTextEntry, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY));
	AddFrame(pHorFrame2, new TGLayoutHints(kLHintsExpandX | kLHintsCenterX,5,5,5,5));
	pTextEntry->Connect("TextChanged(char*)", "ItemSelector", this, "Filter(char*)");
	
	pListBox->SetMultipleSelections(true);
	
	
	int var_n = items.size();
	
	for(int i=0; i<var_n; ++i)
	{
		pListBox->AddEntrySort(items[i].first.c_str(), i+1);
		pListBox->Select(i+1, items[i].second?1:0);
		
	}
	
	pListBox->Resize(480,480);
	AddFrame(pListBox, new TGLayoutHints(kLHintsTop|kLHintsLeft|kLHintsExpandX|kLHintsExpandY, 0, 0, 0, 0));
	pButton->Connect("Pressed()", "ItemSelector", this, "Select()");
	pButtonInvert->Connect("Pressed()", "ItemSelector", this, "InvertSelection()");
	pButtonCancel->Connect("Pressed()", "ItemSelector", this, "Cancel()");
	pHorFrame->AddFrame(pButton, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY));
	pHorFrame->AddFrame(pButtonInvert, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY));
	pHorFrame->AddFrame(pButtonCancel, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY));
	AddFrame(pHorFrame, new TGLayoutHints(kLHintsExpandX|kLHintsCenterX, 5, 5, 5, 5));
	Connect("CloseWindow()", "ItemSelector", this, "Close()");
	SetWindowName(title.c_str());
	MapSubwindows();
	Resize(GetDefaultSize());
	MapWindow();	
}

ItemSelector::~ItemSelector()
{
	//Cleanup(); 
	pTextEntry->Disconnect("TextChanged(char*)", this, "Filter(char*)");
	delete pTextEntry;
	gApplication->Terminate(0);
}

void ItemSelector::Select()
{
	int id=0;
	
	for(auto& item : items)
	{
		++id;
		if(pListBox->GetEntry(id) != NULL)
			item.second = pListBox->GetSelection(id);
	}	
		
	CloseWindow();
}

void ItemSelector::Cancel()
{
	canceled = true;
	CloseWindow();
}

void ItemSelector::Filter(char* cstr)
{
	string str = cstr;
	int id=0;
	for(auto& item : items)
	{
		++id;
		bool visible = pListBox->GetEntry(id) != NULL;
		bool matches = str.size()>0? item.first.find(str) != string::npos : true;
		
		if(visible)
			item.second = pListBox->GetSelection(id);
		
		if(visible && !matches) 
			pListBox->RemoveEntries(id,id);
		
		if(!visible && matches)
		{
			pListBox->AddEntrySort(item.first.c_str(), id);
			pListBox->Select(id, item.second);
		}
	}
	pListBox->Layout();
}

void ItemSelector::Close()
{	
	//DontCallClose();
	canceled = true;
}

void ItemSelector::InvertSelection()
{
	int id=0;
	
	for(auto& item : items)
	{
		++id;
		if(pListBox->GetEntry(id) != NULL)
		{
			item.second = !pListBox->GetSelection(id);
			pListBox->Select(id, item.second);
		}
	}	
}

/*
vector<pair<int, string> > ItemSelector::GetSelection()
{
	vector<pair<int, string> > selection;
	//int var_n = items.size();
	
	for(auto& item : items)
	{
		int id = item.first+1;
		bool visible = pListBox->GetEntry(id) != NULL;
		if(visible && pListBox->GetSelection(id))
			selection.emplace_back(item);
		if(!visible && selected[id-1])
			selection.emplace_back(item);
	}
	
	//for(int i=0; i<var_n; ++i)
	//	if(pListBox->GetSelection(i+1))
	//		selection.push_back(items[i]);
	
	return selection;
}*/
/*
void ItemSelectDialog(vector<pair<int, string> >& items, const string& title)
{
	if(!gApplication) new TApplication("", NULL, NULL);
	ItemSelector* pItemSel = new ItemSelector(items, title);
	gApplication->Run(1);
	
	// ItemSelector destructor is called by ROOT's dark magic GUI framework
	// I do something with pItemSel to suppress the compiler's unused variable warning

	pItemSel = NULL;
	if(pItemSel) return;
}*/


































