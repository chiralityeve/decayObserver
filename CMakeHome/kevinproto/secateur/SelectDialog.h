#ifndef _SELECTDIALOG_H
#define _SELECTDIALOG_H

#include <sstream>
#include <string>
#include <vector>
#include "TApplication.h"
#include "TGWindow.h"
#include "TGClient.h"
#include "TGFrame.h"
#include "TGLayout.h"
#include "TGButton.h"
#include "TVirtualX.h"
#include "TGTextEntry.h"
#include "TGLabel.h"
#include "TGListBox.h"
#include "TTimer.h"

using namespace std;

//After modifying this header, run rootcint -f SelectDialogDict.cpp -c SelectDialog.h LinkDef.h

/// This is a helper class for SelectDialog.
struct SelectDialogEntry
{
	/// Constructs an Entry for a SelectDialog.
	///
	/// @param text 	The text for the entry.
	/// @param selected On input: if the entry is preselected. On output: whether the entry was selected or not. 
	/// @param id 		An id for your own purposes.
	SelectDialogEntry(const string& text, bool selected=false, int id=0):text(text),selected(selected),id(id){}
	string text;
	bool selected;
	int id;
};

/// This is a dialog window that lets the user select entries from the SelectDialogEntry vector.
class SelectDialog
{
public:
	SelectDialog(const string& title, vector<SelectDialogEntry>& entries, bool& cancelled);
	~SelectDialog();

	void Close();
	
	void ChangedFilter(char* cstr);
	void PressedSelect();
	void PressedInvert();
	void PressedSelAll();
	void PressedCancel();
	void PressedClose();
	
private:
	vector<SelectDialogEntry>& entries;
	bool& cancelled;
	
	const TGWindow* pMainWindow;
	TGTransientFrame* pDialogFrame;
	
	TGHorizontalFrame* pFrameFilter;
	TGLayoutHints* pLayoutFrameFilter;
	TGLayoutHints* pLayoutLabelFilter;
	TGLayoutHints* pLayoutTextEntryFilter;
	TGLabel *pLabelFilter;
	TGTextEntry* pTextEntryFilter;
	
	TGListBox* pListBox;
	TGLayoutHints* pLayoutListBox;
	
	TGHorizontalFrame* pFrameButtons;
	TGHorizontalFrame* pFrameButtons2;
	TGLayoutHints* pLayoutFrameButtons;
	TGLayoutHints* pLayoutButtons;
	TGTextButton* pButtonSelect;
	TGTextButton* pButtonInvert;
	TGTextButton* pButtonSelAll;
	TGTextButton* pButtonCancel;
	
	void PositionWindow();
	void Cleanup();
	void DelayedClose();
		
	static const unsigned int default_width;
	static const unsigned int default_height;
	
	static bool prev_flag;
	static int prev_x;
	static int prev_y;
	static unsigned int prev_width;
	static unsigned int prev_height;
	static int map_offset_x;
	static int map_offset_y;
	
	template<class T>
	static inline void MaybeDelete(T& p){ if(p==NULL) return; delete p; p=NULL; }

};


#endif //_SELECTDIALOG_H











































