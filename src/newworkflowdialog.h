////////////////////////////////////////////////////////////////////////////////
// Name:        newworkflowdialog.h
// Purpose:     Dialog to get dataset and output directories for experiement
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// Licence:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

#ifndef _NEW_WORKFLOW_DIALOG_H_
#define _NEW_WORKFLOW_DIALOG_H_

#ifdef __GNUG__
#pragma interface "newworkflowdialog.cpp"
#endif

// Includes

#include "wx/filepicker.h"

// Control identifiers

enum {
	ID_NEW_WORKFLOW_DIALOG = 200,
	ID_PARENT_DIR_PICKER = 201,
	ID_WORKFLOW_DIR_TXT_CTRL = 202,
};

// NewWorkflowDialog class declaration

class NewWorkflowDialog: public wxDialog {
	DECLARE_CLASS(NewWorkflowDialog)

public:
	NewWorkflowDialog();
	NewWorkflowDialog(wxWindow* parent,
			const wxString& parent_dir,
			wxWindowID id = ID_NEW_WORKFLOW_DIALOG,
			const wxString& caption = wxT("New Project"),
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxCAPTION|wxSYSTEM_MENU|wxRESIZE_BORDER|wxOK_DEFAULT);

	// Member initialization
	void Init();

	// Creation
	bool Create(wxWindow* parent,
			const wxString& parent_dir,
			wxWindowID id = ID_NEW_WORKFLOW_DIALOG,
			const wxString& caption = wxT("New Project"),
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxCAPTION|wxSYSTEM_MENU|wxRESIZE_BORDER|wxOK_DEFAULT);

	// Creates the controls and sizers
	void CreateControls(const wxString& parent_dir);

	wxString GetWorkflowDir() const;

	//// NewWorkflowDialog event handler declarations

	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);

	//// NewWorkflowDialog member variables

	// Data members
	wxTextCtrl* workflow_dir;
	wxDirPickerCtrl* parent_dir;

private:
	bool isValid();
};

#endif
// _NEW_WORKFLOW_DIALOG_H_
