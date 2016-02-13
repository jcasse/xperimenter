////////////////////////////////////////////////////////////////////////////////
// Name:        taskdialog.h
// Purpose:     Dialog to select new task to add/edit/view
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

#ifndef _TASK_DIALOG_H_
#define _TASK_DIALOG_H_

//#ifdef __GNUG__
//#pragma interface "taskdialog.cpp"
//#endif

//// Includes

#include "wx/choice.h"

//// Control identifiers

enum {
	ID_TASK_DIALOG = 50000,
};

//// TaskDialog class declaration

class TaskDialog: public wxDialog {
	DECLARE_CLASS(TaskDialog)
	DECLARE_EVENT_TABLE()

public:
	TaskDialog();
	TaskDialog(wxWindow* parent,
			wxWindowID id = ID_TASK_DIALOG,
			const wxString& caption = wxT("Task Dialog"),
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxCLOSE_BOX|wxCAPTION|wxSYSTEM_MENU);

	// Member initialization
	void Init();

	// Creation
	bool Create(wxWindow* parent,
			wxWindowID id = ID_TASK_DIALOG,
			const wxString& caption = wxT("Task Dialog"),
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxCLOSE_BOX|wxCAPTION|wxSYSTEM_MENU);

	// Creates the controls and sizers
	void CreateControls();
	void InitializeControls();

	wxString GetSelection() const;

	/// TaskDialog event handler declarations

	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);

	static wxArrayString taskChoices;

private:

	/// TaskDialog member variables

	wxChoice* taskctrl;
};

#endif
// _TASK_DIALOG_H_
