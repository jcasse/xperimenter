////////////////////////////////////////////////////////////////////////////////
// Name:        deletetaskdialog.cpp
// Purpose:     Dialog to delete task (along with its subtasks)
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

//#ifdef __GNUG__
//#pragma implementation "deletetaskdialog.h"
//#endif

#include "wx/wx.h"
#include "wx/statline.h"

#include "deletetaskdialog.h"

//// DeleteTaskDialog type definition

IMPLEMENT_CLASS(DeleteTaskDialog, wxDialog)

//// DeleteTaskDialog event table definition

BEGIN_EVENT_TABLE(DeleteTaskDialog, wxDialog)
	EVT_CLOSE(DeleteTaskDialog::OnClose)
	EVT_COMMAND(ID_DELETE_TASK_TREE_CTRL, TREE_EVENT, DeleteTaskDialog::OnWorkflowTreeCtrlChanged)
END_EVENT_TABLE()

//// DeleteTaskDialog constructors

DeleteTaskDialog::DeleteTaskDialog() {
	Init();
}

DeleteTaskDialog::DeleteTaskDialog(wxWindow* parent, shptr<Task> task,
		wxWindowID id, const wxString& caption,
		const wxPoint& pos, const wxSize& size, long style) {
	Init();
	Create(parent, task, id, caption, pos, size, style);
}

//// Initialization
void DeleteTaskDialog::Init() {
}

//// DeleteTaskDialog creator

bool DeleteTaskDialog::Create(wxWindow* parent, shptr<Task> task,
		wxWindowID id, const wxString& caption,
		const wxPoint& pos, const wxSize& size, long style) {

	// We have to set extra styles before creating the dialog
	SetExtraStyle(wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP);

	if (!wxDialog::Create(parent, id, caption, pos, size, style)) return false;

	CreateControls();
	InitializeControls(task);
	//SetDialogHelp();
	//SetDialogValidators();

	// This fits the dialog to the minimum size dictated by the sizers
	GetSizer()->Fit(this);

	// This ensures that the dialog cannot be sized smaller
	// than the minimum size
	GetSizer()->SetSizeHints(this);

	// Centre the dialog on the parent or (if none) screen
	Centre();

	return true;
}

//// Control creation for DeleteTaskDialog

void DeleteTaskDialog::InitializeControls(shptr<Task> root) {
	treectrl_->build_tree(root);
}

void DeleteTaskDialog::CreateControls() {

	// top-level box sizer
	wxBoxSizer* topbox = new wxBoxSizer(wxVERTICAL);

	/// create items 

	treectrl_ = new WorkflowTreeCtrl(this, ID_DELETE_TASK_TREE_CTRL,
			wxDefaultPosition, wxSize(200, 100));

	wxButton* okButton = new wxButton(this, wxID_ANY, "Ok");
	wxButton* cancelButton = new wxButton(this, wxID_ANY, "Cancel");

	wxStaticLine* hline = new wxStaticLine(this, wxID_ANY, wxDefaultPosition);

	/// create layout

	wxBoxSizer* btnbox = new wxBoxSizer(wxHORIZONTAL);

	btnbox->Add(okButton, 0,
			wxLEFT|wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	btnbox->Add(cancelButton, 0,
			wxLEFT|wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);

    topbox->Add(treectrl_, 0, wxALL|wxALIGN_CENTER, 5);
	topbox->Add(hline, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5);
	topbox->Add(btnbox, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5);

	this->SetSizer(topbox);

	// bind the widgets to event handlers

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DeleteTaskDialog::OnCancel,
			this, cancelButton->GetId());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DeleteTaskDialog::OnOK,
			this, okButton->GetId());


	Center();
}

//// event handlers

void DeleteTaskDialog::OnOK(wxCommandEvent & event) {
	EndModal(wxID_OK);
	//Destroy();
	Close();
}

void DeleteTaskDialog::OnCancel(wxCommandEvent & event) {
	EndModal(wxID_CANCEL);
	//Destroy();
	Close();
}

void DeleteTaskDialog::OnClose(wxCloseEvent& event) {
	event.Skip();
}

void DeleteTaskDialog::OnWorkflowTreeCtrlChanged(wxCommandEvent&) {
}
