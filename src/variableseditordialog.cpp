////////////////////////////////////////////////////////////////////////////////
// Name:        variableseditordialog.cpp
// Purpose:     Dialog to select episodes for experiment
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

//#ifdef __GNUG__
//#pragma implementation "variableseditordialog.h"
//#endif

#include "wx/wx.h"

#include "wx/valtext.h"
#include "wx/valgen.h"

#include <fstream>
#include <algorithm>
#include <sstream>

#include "variableseditordialog.h"

//// type definition

IMPLEMENT_CLASS(VariablesEditorDialog, wxDialog)

//// event table definition

BEGIN_EVENT_TABLE(VariablesEditorDialog, wxDialog)
	EVT_BUTTON(ID_VE_MOVETOPROJECT, VariablesEditorDialog::OnMoveToProject)
	EVT_BUTTON(ID_VE_MOVETODATASET, VariablesEditorDialog::OnMoveToDataset)
	EVT_BUTTON(ID_VE_ALL_IN, VariablesEditorDialog::OnAllIn)
	EVT_BUTTON(ID_VE_ALL_OUT, VariablesEditorDialog::OnAllOut)
END_EVENT_TABLE()

//// constructors

VariablesEditorDialog::VariablesEditorDialog(): default_dir_() {
	Init();
}

VariablesEditorDialog::VariablesEditorDialog(wxWindow* parent,
		const wxString& default_dir,
		wxWindowID id,
		const wxString& caption,
		const wxArrayString& dataset_vars, const wxArrayString& project_vars,
		const wxPoint& pos, const wxSize& size, long style):
		default_dir_(default_dir) {
	Init();
	Create(parent, id, caption, dataset_vars, project_vars, pos, size, style);
}

void VariablesEditorDialog::Init() {
	modified_ = false;
}

//// creator

bool VariablesEditorDialog::Create(wxWindow* parent,
		wxWindowID id, const wxString& caption,
		const wxArrayString& dataset_vars, const wxArrayString& project_vars,
		const wxPoint& pos, const wxSize& size, long style) {

	// We have to set extra styles before creating the dialog
	SetExtraStyle(wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP);

	if (!wxDialog::Create(parent, id, caption, pos, size, style)) return false;

	CreateControls();
	InitializeControls(dataset_vars, project_vars);
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

//// Control creation

void VariablesEditorDialog::CreateControls() {

	/// items

    // label for the project list box
    projectlabel = new wxStaticText(this, ID_VE_PROJECTLABEL,
            wxT("&In Project:"), wxDefaultPosition, wxDefaultSize, 0);

	// list box for project
	projectlistbox = new wxListBox(this, ID_VE_PROJECTLISTBOX,
			wxPoint(-1, -1), wxSize(-1, -1), 0, NULL,
			wxLB_SORT|wxLB_MULTIPLE);

    // label for the dataset list box
    datasetlabel = new wxStaticText(this, ID_VE_DATASETLABEL,
            wxT("&Available:"), wxDefaultPosition, wxDefaultSize, 0);

	// list box for dataset
	datasetlistbox = new wxListBox(this, ID_VE_DATASETLISTBOX,
			wxPoint(-1, -1), wxSize(-1, -1), 0, NULL,
			wxLB_SORT|wxLB_MULTIPLE);

	// control buttons
	movetoprojectb = new wxButton(this, ID_VE_MOVETOPROJECT, wxT("In  <<"));
	movetodatasetb = new wxButton(this, ID_VE_MOVETODATASET, wxT(">> Out"));
	wxButton* allinb = new wxButton(this, ID_VE_ALL_IN, wxT("All In"));
	wxButton* alloutb = new wxButton(this, ID_VE_ALL_OUT, wxT("All Out"));

	// divider line
	wxStaticLine* hline = new wxStaticLine(this, wxID_ANY, wxDefaultPosition);

	// window buttons
	wxButton* fromFileButton = new wxButton(this, ID_VE_FROM_FILE, "From File");
	wxButton* okButton = new wxButton(this, wxID_ANY, "Ok");
	wxButton* cancelButton = new wxButton(this, wxID_ANY, "Cancel");

	/// layout

	// box sizer for project list box
	wxBoxSizer* projectbox = new wxBoxSizer(wxVERTICAL);

    projectbox->Add(projectlabel, 0, wxALIGN_LEFT|wxLEFT, 5);
	projectbox->Add(projectlistbox, 1, wxEXPAND|wxALL, 5);

	// box sizer for dataset list box
	wxBoxSizer* datasetbox = new wxBoxSizer(wxVERTICAL);

    datasetbox->Add(datasetlabel, 0, wxALIGN_LEFT|wxLEFT, 5);
	datasetbox->Add(datasetlistbox, 1, wxEXPAND|wxALL, 5);

	// button box sizer
	wxBoxSizer* buttonbox = new wxBoxSizer(wxVERTICAL);

	buttonbox->Add(-1, 20); // vertical space
	buttonbox->Add(movetoprojectb, 0, wxTOP, 5);
	buttonbox->Add(movetodatasetb, 0, wxTOP, 5);
	buttonbox->Add(-1, 10); // vertical space
	buttonbox->Add(allinb, 0, wxTOP, 5);
	buttonbox->Add(alloutb, 0, wxTOP, 5);

    // center box sizer
    wxBoxSizer* centerbox = new wxBoxSizer(wxHORIZONTAL);

	centerbox->Add(projectbox, 1, wxEXPAND|wxALL, 5);
	centerbox->Add(buttonbox, 0, wxEXPAND|wxALL, 5);
	centerbox->Add(datasetbox, 1, wxEXPAND|wxALL, 5);

	// window button box sizer
	wxBoxSizer* btnbox = new wxBoxSizer(wxHORIZONTAL);

	btnbox->Add(fromFileButton, 0,
		wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	btnbox->AddStretchSpacer(1);
	btnbox->Add(okButton, 0,
		wxLEFT|wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	btnbox->Add(cancelButton, 0,
		wxLEFT|wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);

	// top-level box sizer
	wxBoxSizer* topbox = new wxBoxSizer(wxVERTICAL);

	topbox->Add(centerbox, 1, wxEXPAND|wxALL, 5);
	topbox->Add(hline, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5);
	topbox->Add(btnbox, 0,
		wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALIGN_CENTER, 10);

	this->SetSizer(topbox);

	/// bind the widgets to event handlers

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &VariablesEditorDialog::OnCancel,
		this, cancelButton->GetId());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &VariablesEditorDialog::OnOK,
		this, okButton->GetId());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &VariablesEditorDialog::OnFromFile,
		this, fromFileButton->GetId());

	Center();
}

void VariablesEditorDialog::InitializeControls(
		const wxArrayString& dataset_vars, const wxArrayString& project_vars) {
	datasetlistbox->Set(dataset_vars);
	projectlistbox->Set(project_vars);
	update_listboxes();
}

//// event handlers

void VariablesEditorDialog::OnMoveToProject(wxCommandEvent& event) {
	wxArrayInt sel;
	datasetlistbox->GetSelections(sel);
//wxArrayInt::iterator it;
//for (it = sel.begin(); it != sel.end(); it++)
//std::cerr << *it << std::endl;
	move(projectlistbox, datasetlistbox, sel);
	update_label(ID_VE_DATASETLABEL, datasetlistbox);
	update_label(ID_VE_PROJECTLABEL, projectlistbox);
	modified_ = true;
}

void VariablesEditorDialog::OnMoveToDataset(wxCommandEvent& event) {
	wxArrayInt sel;
	projectlistbox->GetSelections(sel);
	move(datasetlistbox, projectlistbox, sel);
	update_label(ID_VE_DATASETLABEL, datasetlistbox);
	update_label(ID_VE_PROJECTLABEL, projectlistbox);
	modified_ = true;
}

void VariablesEditorDialog::OnAllIn(wxCommandEvent& event) {
	move_all(projectlistbox, datasetlistbox);
	update_label(ID_VE_DATASETLABEL, datasetlistbox);
	update_label(ID_VE_PROJECTLABEL, projectlistbox);
	modified_ = true;
}

void VariablesEditorDialog::OnAllOut(wxCommandEvent& event) {
	move_all(datasetlistbox, projectlistbox);
	update_label(ID_VE_DATASETLABEL, datasetlistbox);
	update_label(ID_VE_PROJECTLABEL, projectlistbox);
	modified_ = true;
}

void VariablesEditorDialog::OnFromFile(wxCommandEvent& event) {
	wxString prompt("Select Variable File");
	wxFileDialog dialog(this, prompt, default_dir_, wxEmptyString,
		wxFileSelectorDefaultWildcardStr, wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (dialog.ShowModal() == wxID_CANCEL) return;
	const wxString pathname = dialog.GetPath();
//std::cerr << pathname << std::endl;
	default_dir_ = ::get_path(pathname);
	wxArrayString list = load_list(pathname);
	OnAllOut(event);
	const wxArrayString& dataset_vars = datasetlistbox->GetStrings();
	if (!validate_list(list, dataset_vars)) {
		const wxString msg = wxString("Some variables did not match.\n") +
            "Make sure you are loading the correct file.";
        wxMessageBox(msg, "Variable Validation",
            wxOK|wxOK_DEFAULT|wxICON_WARNING|wxSTAY_ON_TOP);

	}
	projectlistbox->Set(list);
	update_listboxes();
	modified_ = true;
}

void VariablesEditorDialog::OnOK(wxCommandEvent & event) {
	EndModal(wxID_OK);
	//Destroy();
	Close();
}

void VariablesEditorDialog::OnCancel(wxCommandEvent & event) {
	EndModal(wxID_CANCEL);
	//Destroy();
	Close();
}

void VariablesEditorDialog::move_all(wxListBox* to, wxListBox* from) {
	const wxArrayString& to_vars = to->GetStrings();
	const wxArrayString& from_vars = from->GetStrings();
	wxArrayString all;
	all.reserve(to_vars.size() + from_vars.size());
	all.insert(all.end(), to_vars.begin(), to_vars.end());
	all.insert(all.end(), from_vars.begin(), from_vars.end());
	to->Set(all);
	from->Clear();
}

void VariablesEditorDialog::move(wxListBox* to,
		wxListBox* from, wxArrayInt& sel) {
	sel.Sort(&VariablesEditorDialog::descending_order);
	for (wxArrayInt::iterator it = sel.begin(); it != sel.end(); it++)
		from->Deselect(*it);
		int last = 0;
	for (wxArrayInt::iterator it = sel.begin(); it != sel.end(); it++)
		last = to->Append(from->GetString(*it));
	for (wxArrayInt::iterator it = sel.begin(); it != sel.end(); it++)
		from->Delete(*it);
	to->SetFirstItem(last);
}

//// public functions

void VariablesEditorDialog::SetDialogValue(const wxVariant& value) {
	projectlistbox->Set(value.GetArrayString());
	update_listboxes();
}

wxVariant VariablesEditorDialog::GetDialogValue() const {
	wxVariant ret(projectlistbox->GetStrings());
	return ret;
}

bool VariablesEditorDialog::IsModified() {
	return modified_;
}

//// helper functions

wxArrayString VariablesEditorDialog::load_list(const wxString& pathname) {
	wxArrayString ret;
	std::fstream fs(pathname.c_str(), std::fstream::in);
	if (fs.fail()) return ret; // file not found
	std::string line;
	std::set<wxString> list;
	while (getline(fs, line)) {
		if (iscntrl(line.back())) line.pop_back(); // remove Microsoft return
		list.emplace(dequote(line));
	}
	fs.close();
	for (const wxString& item : list) ret.Add(item);
	return ret;
}

bool VariablesEditorDialog::validate_list(wxArrayString& list,
		const wxArrayString& dataset_vars) {
	bool ret = true;
	std::set<wxString> set(dataset_vars.begin(), dataset_vars.end());
	for (size_t i = list.GetCount(); i != 0; i--) {
		if (set.find(list[i - 1]) == set.end()) {
			std::cout << list[i - 1] << " not found" << std::endl;
			list.RemoveAt(i - 1);
			ret = false;
		}
	}
	return ret;
}

void VariablesEditorDialog::update_listboxes() {
	// remove from dataset list box the variables already in project
	const wxArrayString& dataset_vars = datasetlistbox->GetStrings();
	const wxArrayString& project_vars = projectlistbox->GetStrings();

	wxArrayString diff = dataset_vars;
	wxArrayString::iterator it = std::set_difference(
			dataset_vars.begin(), dataset_vars.end(),
			project_vars.begin(), project_vars.end(), diff.begin());
	diff.resize(it - diff.begin());

	// reload list boxes
	//datasetlistbox->Clear();
	datasetlistbox->Set(diff);
	//projectlistbox->Clear();
	//projectlistbox->Set(project_vars);

	// update labels
	update_label(ID_VE_DATASETLABEL, datasetlistbox);
	update_label(ID_VE_PROJECTLABEL, projectlistbox);
}

void VariablesEditorDialog::update_label(const wxWindowID id,
		const wxListBox* lb) {
	std::ostringstream ss;
	switch (id) {
		case ID_VE_DATASETLABEL: ss << "Available: "; break;
		case ID_VE_PROJECTLABEL: ss << "In Project: "; break;
		default: assert(false); break; // terminate program
	}
	ss << lb->GetCount();
	((wxStaticText*)FindWindow(id))->SetLabel(ss.str());
}
