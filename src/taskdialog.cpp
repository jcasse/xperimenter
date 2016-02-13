////////////////////////////////////////////////////////////////////////////////
// Name:        taskdialog.cpp
// Purpose:     Dialog to select new task to add/edit/view
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

//#ifdef __GNUG__
//#pragma implementation "taskdialog.h"
//#endif

#include "wx/wx.h"
#include "wx/statline.h"

#include <fstream>

#include "taskdialog.h"

static const char * choices[] = {
		"Preprocess",
		"FeatureExtraction",
		"FeatureSelection",
		"ML"
};
wxArrayString TaskDialog::taskChoices(4, choices);

//// TaskDialog type definition

IMPLEMENT_CLASS(TaskDialog, wxDialog)

//// TaskDialog event table definition

BEGIN_EVENT_TABLE(TaskDialog, wxDialog)
	EVT_CLOSE(TaskDialog::OnClose)
END_EVENT_TABLE()

//// TaskDialog constructors

TaskDialog::TaskDialog() {
	Init();
}

TaskDialog::TaskDialog(wxWindow* parent,
		wxWindowID id, const wxString& caption,
		const wxPoint& pos, const wxSize& size, long style) {
	Init();
	Create(parent, id, caption, pos, size, style);
}

void TaskDialog::Init() {
}

//// TaskDialog creator

bool TaskDialog::Create(wxWindow* parent,
		wxWindowID id, const wxString& caption,
		const wxPoint& pos, const wxSize& size, long style) {

	// We have to set extra styles before creating the dialog
	SetExtraStyle(wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP);

	if (!wxDialog::Create(parent, id, caption, pos, size, style)) return false;

	CreateControls();
	InitializeControls();
	//SetDialogHelp();
	//SetDialogValidators();

	// This fits the dialog to the minimum size dictated by the sizers
	//GetSizer()->Fit(this);

	// This ensures that the dialog cannot be sized smaller
	// than the minimum size
	GetSizer()->SetSizeHints(this);

	// Center the dialog on the parent or (if none) screen
	Centre();

	return true;
}

// Control creation for TaskDialog

void TaskDialog::InitializeControls() {
}

void TaskDialog::CreateControls() {

	// top-level box sizer
	wxBoxSizer* topbox = new wxBoxSizer(wxVERTICAL);

    // spin control box sizer
    wxBoxSizer* choicebox = new wxBoxSizer(wxHORIZONTAL);

	// spin control
	taskctrl= new wxChoice(this, wxID_ANY,
			wxDefaultPosition, wxDefaultSize, taskChoices, 0);

    choicebox->Add(taskctrl, 0, wxALL|wxALIGN_LEFT, 5);

	wxButton* okButton = new wxButton(this, wxID_ANY, "Ok");
	wxButton* cancelButton = new wxButton(this, wxID_ANY, "Cancel");

	wxStaticLine* hline = new wxStaticLine(this, wxID_ANY, wxDefaultPosition);

	wxBoxSizer* btnbox = new wxBoxSizer(wxHORIZONTAL);

	btnbox->Add(okButton, 0,
			wxLEFT|wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	btnbox->Add(cancelButton, 0,
			wxLEFT|wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);

	topbox->Add(choicebox, 1, wxALL|wxEXPAND, 5);
	topbox->Add(hline, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5);
	topbox->Add(btnbox, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5);

	this->SetSizer(topbox);

	// bind the widgets to event handlers

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &TaskDialog::OnCancel,
			this, cancelButton->GetId());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &TaskDialog::OnOK,
			this, okButton->GetId());


	Center();
}

//// event handlers

void TaskDialog::OnOK(wxCommandEvent & event) {
	EndModal(wxID_OK);
	//Destroy();
	Close();
}

void TaskDialog::OnCancel(wxCommandEvent & event) {
	EndModal(wxID_CANCEL);
	//Destroy();
	Close();
}

void TaskDialog::OnClose(wxCloseEvent& event) {
	event.Skip();
}

//// public functions

wxString TaskDialog::GetSelection() const {
	return taskctrl->GetString(taskctrl->GetSelection());
}
