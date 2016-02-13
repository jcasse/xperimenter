////////////////////////////////////////////////////////////////////////////////
// Name:        paramsdialog.cpp
// Purpose:     Dialog to set the parameters for a task
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

//#ifdef __GNUG__
//#pragma implementation "paramsdialog.h"
//#endif

#include "wx/wx.h"
#include "wx/statline.h"
//#include "wx/propgrid/propgridiface.h"
#include "wx/propgrid/property.h"
//#include "wx/propgrid/propgriddefs.h"
#include "wx/propgrid/advprops.h"

#include <iostream>
#include <fstream>

#include "utilities.h"
#include "paramsdialog.h"
#include "dirproperty.h"
#include "fileproperty.h"
#include "arrayvariableproperty.h"
#include "variableseditordialog.h"

//// ParamsDialog type definition

IMPLEMENT_CLASS(ParamsDialog, wxDialog)

//// ParamsDialog event table definition

BEGIN_EVENT_TABLE(ParamsDialog, wxDialog)
	EVT_CLOSE(ParamsDialog::OnClose)
END_EVENT_TABLE()

//// ParamsDialog constructors

ParamsDialog::ParamsDialog(): default_dir_(), params_(), task_(NULL) {
	Init();
}

ParamsDialog::ParamsDialog(wxWindow* parent, shptr<Task> task,
		const wxString& default_dir,
		const bool readonly,
		wxWindowID id,
		const wxPoint& pos, const wxSize& size, long style):
		readonly_(readonly), default_dir_(default_dir),
		params_(task->get_params()), task_(task) {
	Init();
	Create(parent, task, id, pos, size, style);
}

void ParamsDialog::Init() {
	pg_ = NULL;
	init_params();
}

//// ParamsDialog creator

bool ParamsDialog::Create(wxWindow* parent, shptr<Task> task,
		wxWindowID id,
		const wxPoint& pos, const wxSize& size, long style) {

	// We have to set extra styles before creating the dialog
	SetExtraStyle(wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP);
	//SetExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS);/////////////////

	wxString caption(task->get_name() + " Task Parameters");
	if (!wxDialog::Create(parent, id, caption, pos, size, style)) return false;

	CreateControls();
	InitializeControls();
	//SetDialogHelp();
	//SetDialogValidators();

	// This fits the dialog to the minimum size dictated by the sizers
	GetSizer()->Fit(this);

	// This ensures that the dialog cannot be sized smaller
	// than the minimum size
	GetSizer()->SetSizeHints(this);

	// Center the dialog on the parent or (if none) screen
	Centre();

	return true;
}

//// Control creation for ParamsDialog

void ParamsDialog::CreateControls() {

	/// items

	pg_ = new wxPropertyGrid(this, ID_PARAMS_DIALOG_PROP_GRID,
			wxDefaultPosition, wxSize(800, 300), wxPG_BOLD_MODIFIED);

	wxButton* loadButton = new wxButton(this, ID_PARAMS_DIALOG_LOAD,
			"From File");
	wxButton* okButton = new wxButton(this, wxID_ANY, "Ok");
	wxButton* cancelButton = new wxButton(this, wxID_ANY, "Cancel");

	wxStaticLine* hline = new wxStaticLine(this, wxID_ANY, wxDefaultPosition);

	/// Options

	if (readonly_) {
		loadButton->Enable(false);
		okButton->Enable(false);
		cancelButton->SetLabel("Close");
	}

    /// layout

    wxBoxSizer* ctrlbox = new wxBoxSizer(wxVERTICAL);

	ctrlbox->Add(pg_, 1, wxALL|wxEXPAND|wxALIGN_LEFT, 10);

	wxBoxSizer* btnbox = new wxBoxSizer(wxHORIZONTAL);

	btnbox->Add(loadButton, 0,
			wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	btnbox->AddStretchSpacer(1);
	btnbox->Add(okButton, 0,
			wxLEFT|wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	btnbox->Add(cancelButton, 0,
			wxLEFT|wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);

	wxBoxSizer* topbox = new wxBoxSizer(wxVERTICAL);

	topbox->Add(ctrlbox, 1, wxALL|wxEXPAND, 5);
	topbox->Add(hline, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5);
	topbox->Add(btnbox, 0,
		wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALIGN_CENTER, 10);

	this->SetSizer(topbox);

	/// bind the widgets to event handlers

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ParamsDialog::OnCancel,
			this, cancelButton->GetId());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ParamsDialog::OnOK,
			this, okButton->GetId());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ParamsDialog::OnFromFile,
			this, loadButton->GetId());

	Center();
}

void ParamsDialog::InitializeControls() {
	if (params_.empty()) return;
	build_grid();
}

//// event handlers

void ParamsDialog::OnFromFile(wxCommandEvent&) {
	wxString prompt("Select Parameter File");
	wxFileDialog dialog(this, prompt, default_dir_, "xperimenter.csv",
		"CSV files (*.csv)|*.csv", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (dialog.ShowModal() == wxID_CANCEL) return;
	const wxString pathname = dialog.GetPath();
	default_dir_ = ::get_path(pathname);
	params_.clear();
	init_params();
	params_.load(pathname);
	build_grid();
}

void ParamsDialog::OnOK(wxCommandEvent & event) {
	const bool changed = pg_->CommitChangesFromEditor(); // ?
	wxArrayString params = invalid_params();
	if (!params.empty()) {display_invalid_params(params); return;}
	set_params();
	EndModal(wxID_OK);
	Close();
}

void ParamsDialog::OnCancel(wxCommandEvent & event) {
	EndModal(wxID_CANCEL);
	Close();
}

void ParamsDialog::OnClose(wxCloseEvent& event) {
	event.Skip(); // let the default Close handler destroy the window
}

//// helper functions

void ParamsDialog::display_invalid_params(const wxArrayString& params) {
	if (params.empty()) return;
	wxString msg = "The following parameters are invalid.\n";
	wxArrayString::const_iterator it;
	for (it = params.begin(); it != params.end(); it++) msg += '\n' + *it;
	wxMessageBox(msg, "Error", wxICON_ERROR);
}

void ParamsDialog::build_grid() {
	std::map<const wxString, wxPGProperty*> properties;
	pg_->Clear();
	for (Param& param : params_) {
		wxPGProperty* parent = param.get_parent().empty() ?
				pg_->GetRoot() : properties[param.get_parent()];
		wxPGProperty* property = make_property(param);
		const wxString name = param.get_name();
		properties[name] = pg_->AppendIn(parent, property);
	}
}

void ParamsDialog::init_params() {
	// do not change order
	// the parameters must be inserted in reverse order from the
	// desired order. Headers must end up first: General & Task-specific
	// Then Program, Input Directory, and Output Directory
	set_fixed_params();
	set_param_headers();
}

void ParamsDialog::set_param_headers() {
    params_.insert("Task-specific", "", "CAT", "");
    params_.insert("General", "", "CAT", "");
}

void ParamsDialog::set_fixed_params() {
	params_.insert("Output Directory", "General", "DIR", task_->get_dir());
	if (!task_->is_root())
		params_.insert("Input Directory", "General", "DIR",
			task_->get_parent()->get_dir());
	wxString program = wxEmptyString;
	try {program = task_->get_program();}
	catch (Params::Exception e) {}
	params_.insert("Program", "General", "FILE", program);
}

wxPGProperty* ParamsDialog::make_property(const Param& param) {
	const wxString name = param.get_name();
	const wxString type = param.get_type();
	const wxString value = param.get_value();
	const wxArrayString values = param.get_values();

	wxPGProperty* ret = NULL;

	// category
	if (type == "CAT") ret = new wxPropertyCategory(name);

	// directory
	else if (type == "DIR") {
		ret = new DirProperty(name, wxPG_LABEL, value);
		if (name == "Input Directory" && !task_->is_root()) {
			ret->ChangeFlag(wxPG_PROP_READONLY, true);
			//wxColour color("LIGHT GREY");
			//ret->SetBackgroundColour(color);
		}
		if (name == "Output Directory")
			ret->ChangeFlag(wxPG_PROP_READONLY, true);
	}

	// file
	else if (type == "FILE") {
		ret =  new FileProperty(name, wxPG_LABEL, value);
		if (name == "Program") {
			ret->SetAttribute(wxPG_FILE_WILDCARD, wxT("R files (*.R)|*.R"));
		}
	}

	// drop-down-list
	else if (type == "ENUM") {
		wxPGChoices choices(values);
		ret = new wxEditEnumProperty(name, wxPG_LABEL, choices, value);
	}

	// custom property with custom variables editor
	else if (type == "LIST")
		ret = new ArrayVariableProperty(name, wxPG_LABEL, values);

	// simple text-entry
	else ret = new wxStringProperty(name, wxPG_LABEL, value);

	if (readonly_) ret->ChangeFlag(wxPG_PROP_READONLY, true);

	return ret;
}

bool ParamsDialog::set_params() {
	for (wxPropertyGridIterator it = pg_->GetIterator(); !it.AtEnd(); it++) {
		wxPGProperty* property = *it;
		assert(!property->GetValue().IsNull());
		const wxString name = property->GetName();
		const wxString type = params_.get_type(name);
		wxVariant value = property->GetValue();
		if (type == "LIST") params_.set_values(name, value.GetArrayString());
		else params_.set_value(name, property->ValueToString(value));
	}
	return true;
}

wxArrayString ParamsDialog::invalid_params() {
	wxArrayString ret;
	for (wxPropertyGridIterator it = pg_->GetIterator(); !it.AtEnd(); it++) {
		wxPGProperty* property = *it;
		wxVariant value = property->GetValue();
		assert(!value.IsNull());
		wxString name = property->GetName();
		if (name == "Input Directory") continue; // dir might not exist yet
		if (name == "Output Directory") continue; // dir might not exist yet
		wxPGValidationInfo info;
		if (!property->ValidateValue(value, info)) ret.Add(name);
	}
	return ret;
}
