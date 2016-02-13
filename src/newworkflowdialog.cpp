////////////////////////////////////////////////////////////////////////////////
// Name:        newworkflowdialog.cpp
// Purpose:     Dialog to get dataset and output directories for experiment
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// Licence:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "newworkflowdialog.h"
#endif

#include "wx/wx.h"
#include "wx/statline.h"

#include <iostream>

#include "utilities.h"
#include "newworkflowdialog.h"

// NewWorkflowDialog type definition

IMPLEMENT_CLASS(NewWorkflowDialog, wxDialog)

// NewWorkflowDialog constructors

NewWorkflowDialog::NewWorkflowDialog() {
	Init();
}

NewWorkflowDialog::NewWorkflowDialog(wxWindow* parent,
		const wxString& default_parent_dir,
		wxWindowID id, const wxString& caption,
		const wxPoint& pos, const wxSize& size, long style) {
	Init();
	Create(parent, default_parent_dir, id, caption, pos, size, style);
}

// Initialization
void NewWorkflowDialog::Init() {
	parent_dir = NULL;
	workflow_dir = NULL;
}

// NewWorkflowDialog creator

bool NewWorkflowDialog::Create(wxWindow* parent,
		const wxString& default_parent_dir,
		 wxWindowID id, const wxString& caption,
		const wxPoint& pos, const wxSize& size, long style) {

	// We have to set extra styles before creating the dialog
	SetExtraStyle(wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP|wxOK_DEFAULT);

	if (!wxDialog::Create(parent, id, caption, pos, size, style)) return false;

	CreateControls(default_parent_dir);
	//SetDialogHelp();
	//SetDialogValidators();

	// This fits the dialog to the minimum size dictated by the sizers
	//GetSizer()->Fit(this);

	// This ensures that the dialog cannot be sized smaller
	// than the minimum size
	GetSizer()->SetSizeHints(this);

	// Centre the dialog on the parent or (if none) screen
	Centre();

	return true;
}

wxString NewWorkflowDialog::GetWorkflowDir() const {
	return parent_dir->GetPath() + "/" + workflow_dir->GetLineText(0);
}

// Control creation for NewWorkflowDialog

void NewWorkflowDialog::CreateControls(const wxString& default_parent_dir) {

	// create items

    wxStaticText* workflowdirlabel = new wxStaticText(this, wxID_STATIC,
			wxT("Project Name:"),
			wxDefaultPosition, wxDefaultSize, 0);
    wxStaticText* parentdirlabel = new wxStaticText(this, wxID_STATIC,
			wxT("Create project as subdirectory of:"),
			wxDefaultPosition, wxDefaultSize, 0);

	workflow_dir = new wxTextCtrl(this, ID_WORKFLOW_DIR_TXT_CTRL,
			//wxEmptyString, wxDefaultPosition, wxSize(150, -1), wxTE_DONTWRAP);
			"", wxDefaultPosition, wxSize(200, -1),
			wxTE_DONTWRAP);
	parent_dir = new wxDirPickerCtrl(this, ID_PARENT_DIR_PICKER,
			//wxEmptyString, wxDirSelectorPromptStr,
			default_parent_dir, wxDirSelectorPromptStr,
			wxDefaultPosition, wxSize(400, -1), wxDIRP_DEFAULT_STYLE);

	wxStaticLine* hline = new wxStaticLine(this, wxID_ANY, wxDefaultPosition);

	wxButton* okButton = new wxButton(this, wxID_ANY, "Ok");
	wxButton* cancelButton = new wxButton(this, wxID_ANY, "Cancel");

	// create layout

	wxBoxSizer* ctrlbox = new wxBoxSizer(wxVERTICAL);

	ctrlbox->Add(workflowdirlabel, 0, wxALL|wxALIGN_LEFT, 10);
	ctrlbox->Add(workflow_dir, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT, 15);
	ctrlbox->Add(parentdirlabel, 0, wxLEFT|wxRIGHT|wxALIGN_LEFT, 10);
	ctrlbox->Add(parent_dir, 0, wxALL|wxEXPAND|wxALIGN_LEFT, 10);

	wxBoxSizer* btnbox = new wxBoxSizer(wxHORIZONTAL);

	btnbox->Add(okButton, 0,
			wxALL|wxALIGN_RIGHT|wxALIGN_BOTTOM, 5);
	btnbox->Add(cancelButton, 0,
			wxALL|wxALIGN_RIGHT|wxALIGN_BOTTOM, 5);

	wxBoxSizer* topbox = new wxBoxSizer(wxVERTICAL);

	topbox->Add(ctrlbox, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL);
	topbox->Add(hline, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5);
	topbox->Add(btnbox, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 10);

	this->SetSizer(topbox);

	workflow_dir->SetFocus();

	// bind the widgets to event handlers

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &NewWorkflowDialog::OnCancel,
			this, cancelButton->GetId());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &NewWorkflowDialog::OnOK,
			this, okButton->GetId());

	Center();
}

// event handlers

void NewWorkflowDialog::OnOK(wxCommandEvent & event) {
	if (!isValid()) return;
	EndModal(wxID_OK);
	//Destroy();
	Close();
}

bool NewWorkflowDialog::isValid() {
	bool ret = true;
	const wxString path = GetWorkflowDir();
	const wxString name = workflow_dir->GetLineText(0);
	const wxString dir = parent_dir->GetPath();
	wxString msg;
	if (name.IsEmpty())
		msg = wxString("Project name is empty.");
	else if (dir.IsEmpty())
		msg = wxString("Parent directory is empty.");
	else if (!::exists(dir))
		msg = wxString("Parent directory does not exist.");
	else if (::exists(path))
		msg = wxString("Directory\n") + path + "\nalready exixts.";
	else msg = wxEmptyString;
	if (!msg.IsEmpty()) {
		wxMessageBox(msg, wxT("Error"),
			wxOK|wxOK_DEFAULT|wxICON_ERROR|wxSTAY_ON_TOP);
		ret = false;
	}
	return ret;
}

void NewWorkflowDialog::OnCancel(wxCommandEvent & event) {
	EndModal(wxID_CANCEL);
	//Destroy();
	Close();
}

void NewWorkflowDialog::OnClose(wxCloseEvent& event) {

	//if (event.CanVeto()) {
	//	wxString msg = "The file ";
	//	msg += vargrpchoice->GetString(vargrpchoice->GetSelection());
	//	msg += " has not been saved... continue closig?";
	//	if (wxMessageBox(msg,
	//			"Please confirm",
	//			wxICON_QUESTION|wxYES_NO) != wxYES) {
	//		event.Veto();
	//		return;
	//	}
	//}

	event.Skip();
}
