////////////////////////////////////////////////////////////////////////////////
// Name:        deletetaskdialog.h
// Purpose:     Dialog to delete task (along with its subtasks)
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// Licence:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

#ifndef _DELETE_TASK_DIALOG_H_
#define _DELETE_TASK_DIALOG_H_

//#ifdef __GNUG__
//#pragma interface "deletetaskdialog.cpp"
//#endif

//// Includes

#include "wx/dataview.h"

#include "task.h"
#include "workflowtreectrl.h"

//// Control identifiers

enum {
	ID_DELETE_TASK_DIALOG = 700,
};

//// DeleteTaskDialog class declaration

class DeleteTaskDialog: public wxDialog {
	DECLARE_CLASS(DeleteTaskDialog)
	DECLARE_EVENT_TABLE()

public:
	DeleteTaskDialog();
	DeleteTaskDialog(wxWindow* parent, shptr<Task> root,
			wxWindowID id = ID_DELETE_TASK_DIALOG,
			const wxString& caption = wxT("Delete Subtree"),
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxCAPTION|wxSYSTEM_MENU);

	// Member initialization
	void Init();

	// Creation
	bool Create(wxWindow* parent, shptr<Task> root,
			wxWindowID id = ID_DELETE_TASK_DIALOG,
			const wxString& caption = wxT("Delete Subtree"),
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxCAPTION|wxSYSTEM_MENU);

	// Creates the controls and sizers
	void CreateControls();
	void InitializeControls(shptr<Task> root);

	/// DeleteTaskDialog event handler declarations

	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnWorkflowTreeCtrlChanged(wxCommandEvent& event);

private:

	/// DeleteTaskDialog member variables

	WorkflowTreeCtrl* treectrl_;
};

#endif
// _DELETE_TASK_DIALOG_H_
