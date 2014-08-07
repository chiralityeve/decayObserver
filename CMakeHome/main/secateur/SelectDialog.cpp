#include "SelectDialog.h"

const unsigned int SelectDialog::default_width  = 360;
const unsigned int SelectDialog::default_height = 480;
bool SelectDialog::prev_flag = false;
int SelectDialog::prev_x = 0;
int SelectDialog::prev_y = 0;
unsigned int SelectDialog::prev_width = 0;
unsigned int SelectDialog::prev_height = 0;
int SelectDialog::map_offset_x = 0;
int SelectDialog::map_offset_y = 0;

/// Constructs and shows a dialog windows.
///
/// @param title 		Title of the dialog window.
/// @param entries 		A vector containing the entries that can be selected.
/// @param cancelled 	Whether or not the user cancelled the selection
SelectDialog::SelectDialog(const string& title, vector<SelectDialogEntry>& entries, bool& cancelled):
entries(entries),
cancelled(cancelled),

pMainWindow(gClient->GetRoot()),
pDialogFrame(new TGTransientFrame(pMainWindow, pMainWindow, 10, 10)),

pFrameFilter(new TGHorizontalFrame(pDialogFrame, default_width, 20, kFixedWidth)),
pLayoutFrameFilter(new TGLayoutHints(kLHintsExpandX|kLHintsCenterX)),
pLayoutLabelFilter(new TGLayoutHints(kLHintsLeft|kLHintsCenterY, 5, 5, 5, 5)),
pLayoutTextEntryFilter(new TGLayoutHints(kLHintsCenterY | kLHintsExpandX, 5, 5, 5, 5)),
pLabelFilter(new TGLabel(pFrameFilter, "Filter:")),
pTextEntryFilter(new TGTextEntry(pFrameFilter)),

pListBox(new TGListBox(pDialogFrame)),
pLayoutListBox(new TGLayoutHints(kLHintsCenterX|kLHintsCenterY|kLHintsExpandX|kLHintsExpandY, 5, 5, 0, 5)),

pFrameButtons(new TGHorizontalFrame(pDialogFrame, 1, 20)),
pFrameButtons2(new TGHorizontalFrame(pDialogFrame, 1, 20)),
pLayoutFrameButtons(new TGLayoutHints(kLHintsCenterX)),
pLayoutButtons(new TGLayoutHints(kLHintsCenterY, 5, 5, 0, 5)),
pButtonSelect(new TGTextButton(pFrameButtons2, "&Select")),
pButtonInvert(new TGTextButton(pFrameButtons, "&Invert Selection")),
pButtonSelAll(new TGTextButton(pFrameButtons, "Select &All")),
pButtonCancel(new TGTextButton(pFrameButtons2, "&Cancel"))
{	
	// ROOT, please stop deleting my memory
	pDialogFrame->SetCleanup(kNoCleanup);
	pFrameFilter->SetCleanup(kNoCleanup);
	pLabelFilter->SetCleanup(kNoCleanup);
	pTextEntryFilter->SetCleanup(kNoCleanup);
	pListBox->SetCleanup(kNoCleanup);
	pFrameButtons->SetCleanup(kNoCleanup);
	pFrameButtons2->SetCleanup(kNoCleanup);
	pButtonSelect->SetCleanup(kNoCleanup);
	pButtonInvert->SetCleanup(kNoCleanup);
	pButtonSelAll->SetCleanup(kNoCleanup);
	pButtonCancel->SetCleanup(kNoCleanup);
	
	// The filter label and text box
	pTextEntryFilter->Connect("TextChanged(char*)", "SelectDialog", this, "ChangedFilter(char*)");
	pFrameFilter->AddFrame(pLabelFilter, pLayoutLabelFilter);
	pFrameFilter->AddFrame(pTextEntryFilter, pLayoutTextEntryFilter);
	pDialogFrame->AddFrame(pFrameFilter, pLayoutFrameFilter);

	
	// The list box
	int id=0;

	pListBox->SetMultipleSelections(true);
	pListBox->Resize(default_width, default_height);
	pDialogFrame->AddFrame(pListBox, pLayoutListBox);
	
	for(auto& entry : entries)
	{
		++id;
		pListBox->AddEntrySort(entry.text.c_str(), id);
		pListBox->Select(id, entry.selected);
	}
	
	// The buttons
	pButtonSelect->Connect("Pressed()", "SelectDialog", this, "PressedSelect()");
	pButtonInvert->Connect("Pressed()", "SelectDialog", this, "PressedInvert()");
	pButtonSelAll->Connect("Pressed()", "SelectDialog", this, "PressedSelAll()");
	pButtonCancel->Connect("Pressed()", "SelectDialog", this, "PressedCancel()");	
	pFrameButtons2->AddFrame(pButtonSelect, pLayoutButtons);
	pFrameButtons->AddFrame(pButtonInvert, pLayoutButtons);
	pFrameButtons->AddFrame(pButtonSelAll, pLayoutButtons);
	pFrameButtons2->AddFrame(pButtonCancel, pLayoutButtons);
	pDialogFrame->AddFrame(pFrameButtons, pLayoutFrameButtons);	
	pDialogFrame->AddFrame(pFrameButtons2, pLayoutFrameButtons);

	
	// Resize the buttons
	unsigned int btn_width = 0;
	unsigned int btn_height = 0;
	vector<TGTextButton*> pButtons = {pButtonSelect, pButtonInvert, pButtonSelAll, pButtonCancel};
	
	for(auto& pBtn : pButtons)
	{
		btn_width  = max(btn_width , pBtn->GetDefaultWidth() );
		btn_height = max(btn_height, pBtn->GetDefaultHeight());
	}	
	for(auto& pBtn : pButtons)
	{
		pBtn->ChangeOptions(kFixedWidth|pBtn->GetOptions());
		pBtn->SetTextJustify(kTextCenterX|kTextCenterY);
		pBtn->SetWrapLength(-1);
		pBtn->Resize(btn_width+10, btn_height);
	}
	
	// The dialog window		
	pDialogFrame->Connect("CloseWindow()", "SelectDialog", this, "PressedClose()");
	pDialogFrame->SetWindowName(title.c_str());
	pDialogFrame->MapSubwindows();	
	PositionWindow();

	gClient->WaitFor(pDialogFrame);
}

SelectDialog::~SelectDialog(){ Cleanup(); }
void SelectDialog::Close(){ Cleanup(); }
void SelectDialog::DelayedClose()
{ 
	prev_flag = true;
	gVirtualX->GetWindowSize(pDialogFrame->GetId(), prev_x, prev_y, prev_width, prev_height);
	TTimer::SingleShot(150, "SelectDialog", this, "Close()"); 
}

void SelectDialog::PositionWindow()
{
	int x, y;
	unsigned int w, h;
	
	if(!prev_flag)
	{
		UInt_t  width_dial = pDialogFrame-> GetDefaultWidth();
		UInt_t height_dial = pDialogFrame->GetDefaultHeight();
		UInt_t  width_disp = gClient-> GetDisplayWidth();
		UInt_t height_disp = gClient->GetDisplayHeight();
		
		pDialogFrame->Resize(width_dial, height_dial);
		pDialogFrame->Move((width_disp - width_dial)>>1, (height_disp - height_dial)>>1);
		pDialogFrame->MapWindow();
		
		gVirtualX->GetWindowSize(pDialogFrame->GetId(), x, y, w, h);
		map_offset_x = x - pDialogFrame->GetX();
		map_offset_y = y - pDialogFrame->GetY();
		
		return;
	}
	
	pDialogFrame->Resize(prev_width, prev_height);
	pDialogFrame->Move(prev_x - map_offset_x, prev_y - map_offset_y);
	pDialogFrame->MapWindow();
	
	gVirtualX->GetWindowSize(pDialogFrame->GetId(), x, y, w, h);
	map_offset_x += x - prev_x;
	map_offset_y += y - prev_y;
}

void SelectDialog::Cleanup()
{
	MaybeDelete(pLabelFilter);
	MaybeDelete(pTextEntryFilter);
	MaybeDelete(pLayoutLabelFilter);
	MaybeDelete(pLayoutTextEntryFilter);
	MaybeDelete(pLayoutFrameFilter);
	MaybeDelete(pFrameFilter);
	
	MaybeDelete(pListBox);
	MaybeDelete(pLayoutListBox);
	
	MaybeDelete(pButtonSelect);
	MaybeDelete(pButtonInvert);
	MaybeDelete(pButtonSelAll);
	MaybeDelete(pButtonCancel);
	MaybeDelete(pLayoutButtons);
	MaybeDelete(pLayoutFrameButtons);
	MaybeDelete(pFrameButtons);
	MaybeDelete(pFrameButtons2);
	
	MaybeDelete(pDialogFrame);
}

void SelectDialog::ChangedFilter(char* cstr)
{
	string str = cstr;
	int id = 0;
	
	for(auto& entry : entries)
	{
		++id;
		bool visible = pListBox->GetEntry(id) != NULL;
		bool matches = str.empty()? true : entry.text.find(str) != string::npos;
		
		// If the entry is visible, update the selection
		if(visible)
			entry.selected = pListBox->GetSelection(id);
		
		// Remove the entry if it doesn't match the filter
		if(visible && !matches) 
			pListBox->RemoveEntries(id,id);
		
		// Add the entry if it matches the filter
		if(!visible && matches)
		{
			pListBox->AddEntrySort(entry.text.c_str(), id);
			pListBox->Select(id, entry.selected);
		}
	}
	
	pListBox->Layout(); // Update the list box
}

void SelectDialog::PressedSelect()
{
	int id = 0;
	
	for(auto& entry : entries)
	{
		++id;
		
		// If the entry is not visible, continue
		if(pListBox->GetEntry(id) == NULL) continue;
		
		// Update the selection
		entry.selected = pListBox->GetSelection(id);
	}
	
	cancelled = false; // Unset the canceled flag	
	DelayedClose();	
}

void SelectDialog::PressedSelAll()
{
	int id = 0;
	pDialogFrame->MapWindow();
	for(auto& entry : entries)
	{
		++id;
		
		// If the entry is not visible, continue
		if(pListBox->GetEntry(id) == NULL) continue;
		
		// Invert the selection
		entry.selected = true;
		pListBox->Select(id, entry.selected);
	}
}

void SelectDialog::PressedInvert()
{
	int id = 0;
	pDialogFrame->MapWindow();
	for(auto& entry : entries)
	{
		++id;
		
		// If the entry is not visible, continue
		if(pListBox->GetEntry(id) == NULL) continue;
		
		// Invert the selection
		entry.selected = !pListBox->GetSelection(id);
		pListBox->Select(id, entry.selected);
	}
}

void SelectDialog::PressedCancel()
{
	cancelled = true; // Set the canceled flag
	DelayedClose();
}

void SelectDialog::PressedClose()
{
	cancelled = true; // Set the canceled flag
	pDialogFrame->DontCallClose(); // I don't want ROOT messing with my memory
	DelayedClose();
}














































