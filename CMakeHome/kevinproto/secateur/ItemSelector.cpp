#include "ItemSelector.h"

ItemSelector::ItemSelector(vector<pair<int, string> >& items, const string& title):
TGMainFrame(gClient->GetRoot(), 120, 480),
items(items),
pListBox(new TGListBox(this, 89))
{
	int var_n = items.size();
	TGHorizontalFrame* pHorFrame = new TGHorizontalFrame(this, 480, 20, kFixedWidth);
	TGTextButton* pButton = new TGTextButton(pHorFrame, "&Select");
	TGTextButton* pButtonInvert = new TGTextButton(pHorFrame, "&Invert Selection");
	
	for(int i=0; i<var_n; ++i)
		pListBox->AddEntry(items[i].second.c_str(), i+1);
	
	pListBox->Resize(120,480);
	AddFrame(pListBox, new TGLayoutHints(kLHintsTop|kLHintsLeft|kLHintsExpandX|kLHintsExpandY, 0, 0, 0, 0));
	pButton->Connect("Pressed()", "ItemSelector", this, "Select()");
	pButtonInvert->Connect("Pressed()", "ItemSelector", this, "InvertSelection()");
	pHorFrame->AddFrame(pButton, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY));
	pHorFrame->AddFrame(pButtonInvert, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY));
	AddFrame(pHorFrame, new TGLayoutHints(kLHintsExpandX|kLHintsCenterX, 5, 5, 5, 5));
	Connect("CloseWindow()", "ItemSelector", this, "Close()");
	SetWindowName(title.c_str());
	MapSubwindows();
	Resize(GetDefaultSize());
	MapWindow();
	pListBox->SetMultipleSelections(true);
}

ItemSelector::~ItemSelector()
{
	Cleanup();
	gApplication->Terminate(0);
}

void ItemSelector::Select()
{
	items = GetSelection();
	CloseWindow();
}

void ItemSelector::Close()
{	
	//DontCallClose();
	items = vector<pair<int, string> >();
}

void ItemSelector::InvertSelection()
{
	int var_n = items.size();
	
	for(int i=0; i<var_n; ++i)
		pListBox->Select(i+1, !pListBox->GetSelection(i+1));
}

vector<pair<int, string> > ItemSelector::GetSelection()
{
	vector<pair<int, string> > selection;
	int var_n = items.size();
	
	for(int i=0; i<var_n; ++i)
		if(pListBox->GetSelection(i+1))
			selection.push_back(items[i]);
	
	return selection;
}

void ItemSelectDialog(vector<pair<int, string> >& items, const string& title)
{
	if(!gApplication) new TApplication("", NULL, NULL);
	ItemSelector* pItemSel = new ItemSelector(items, title);
	gApplication->Run(1);
	
	// ItemSelector destructor is called by ROOT's dark magic GUI framework
	// I do something with pItemSel to suppress the compiler's unused variable warning
	pItemSel = NULL;
	if(pItemSel) return;
}


































