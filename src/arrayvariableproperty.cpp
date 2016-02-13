////////////////////////////////////////////////////////////////////////////////
// Name:        arrayvariableproperty.cpp
// Purpose:     Defines Property Grid Variables Editor Property
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

//#ifdef __GNUG__
//#pragma implementation "arrayvariableproperty.h"
//#endif

#include <iostream>

#include "wx/busyinfo.h"

#include "boost/tokenizer.hpp"
#include "boost/filesystem.hpp"

#include "arrayvariableproperty.h"

//WX_PG_IMPLEMENT_PROPERTY_CLASS(ArrayVariableProperty, wxArrayStringProperty)
//WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(wxEnumProperty,long,Choice)
WX_PG_IMPLEMENT_PROPERTY_CLASS(ArrayVariableProperty,	// Property name
								wxArrayStringProperty,	// Inherited from
								wxArrayString,			// Value type name
								const wxArrayString&,	// Value type from ctr
								TextCtrlAndButton)		// Initial editor

//IMPLEMENT_DYNAMIC_CLASS(ArrayVariableProperty, wxArrayStringProperty)
//wxPG_IMPLEMENT_PROPERTY_CLASS(ArrayVariableProperty, wxArrayStringProperty)

ArrayVariableProperty::ArrayVariableProperty(const wxString& label,
		const wxString& name, const wxArrayString& value):
		wxArrayStringProperty(label, name, value) {}

ArrayVariableProperty::~ArrayVariableProperty() {}

bool ArrayVariableProperty::OnCustomStringEdit(wxWindow*, wxString&) {
std::cerr << "ArrayVariableProperty::OnCustomStringEdit()" << std::endl;
	return false;
}

bool ArrayVariableProperty::OnButtonClick(wxPropertyGrid* pg,
		wxWindow* WXUNUSED(primaryCtrl), const wxChar* cbt) {

	dataset_vars_ = get_dataset_vars();

	// Update the value from user entry
	wxVariant useValue = pg->GetUncommittedPropertyValue();
	project_vars_ = get_valid(useValue.GetArrayString());

	//if (!pg->EditorValidate()) return false;

	// Create editor dialog
	//VariablesEditorDialog* dlg = create_editor_dialog();
#if wxUSE_VALIDATORS
	wxValidator* validator = GetValidator();
	wxPGInDialogValidator dialogValidator;
#endif

	const wxString name = GetLabel();
	const wxString title = name + " Editor";
	const wxString default_dir = wxEmptyString;
	VariablesEditorDialog* dlg = new VariablesEditorDialog(
		pg, default_dir, wxID_ANY, title, dataset_vars_, project_vars_,
		wxDefaultPosition, wxDefaultSize);//, wxDEFAULT_DIALOG_STYLE);

    //dlg->SetDialogValue(useValue);
    //dlg->Create(pg, wxEmptyString, m_label);

	bool retVal;

	for (;;) {
		retVal = false;

		int res = dlg->ShowModal();

		if (res == wxID_OK && dlg->IsModified()) {
			wxVariant value = dlg->GetDialogValue();
			if (!value.IsNull()) {
				wxArrayString actualValue = value.GetArrayString();
				wxString tempStr;
				ConvertArrayToString(actualValue, &tempStr, m_delimiter);
			#if wxUSE_VALIDATORS
				if (dialogValidator.DoValidate(pg, validator, tempStr))
			#endif
				{
					SetValueInEvent(actualValue);
					retVal = true;
					break;
                }
			} else break;
		} else break;
	}

	delete dlg;

	return retVal;
}

bool ArrayVariableProperty::ValidateValue(wxVariant& value,
		wxPGValidationInfo& validationinfo) const {
	wxArrayString dataset_vars = get_dataset_vars();
	std::set<wxString> set = get_set(dataset_vars);
	wxArrayString tmp = value.GetArrayString();
	for (wxArrayString::const_iterator it = tmp.begin(); it != tmp.end(); it++)
		if (set.find(*it) == set.end()) return false;
	return true;
}

//// helper functions

wxArrayString ArrayVariableProperty::get_dataset_vars() const {
	wxArrayString ret = read_cache();
	if (!ret.empty()) return ret;
	wxBusyCursor wait_cursor;
	wxString msg;
	msg = "Please wait, building variable list from dataset...";
	wxBusyInfo wait_info(msg);
	const wxString pathname = get_var_pathname();
	std::fstream fs(pathname.c_str(), std::fstream::in);
	if (fs.fail()) return ret; // file not found
	std::string line;
	getline(fs, line); // skip header
	size_t col = GetLabel() == "Episodes" ? 0 : 3; // episode-id or variable
	std::set<wxString> vars;
	while (getline(fs, line)) {
		if (iscntrl(line.back())) line.pop_back(); // remove Microsoft return
		boost::char_separator<char> sep(","); // only commas
		boost::tokenizer<boost::char_separator<char>> tokens(line, sep);
		boost::tokenizer<boost::char_separator<char>>::const_iterator it;
		for (it = tokens.begin(); it != tokens.end(); it++)
			if (std::distance(tokens.begin(), it) == col) vars.emplace(*it);
	}
	fs.close();
	for (const wxString& var : vars) ret.Add(var);
	write_cache(ret);
	return ret;
}

wxArrayString ArrayVariableProperty::get_valid(const wxArrayString& val) {
	std::set<wxString> set = get_set(dataset_vars_);
	wxArrayString ret;
	for (wxArrayString::const_iterator it = val.begin(); it != val.end(); it++)
		if (set.find(*it) != set.end()) ret.Add(*it);
	return ret;
}

const std::set<wxString> ArrayVariableProperty::get_set(
		const wxArrayString& val) const {
	std::set<wxString> ret;
	for (wxArrayString::const_iterator it = val.begin(); it != val.end(); it++)
		ret.emplace(*it);
	return ret;
}

const wxString ArrayVariableProperty::get_value(const wxString& param) const {
	wxString ret;
	wxPropertyGrid* pg = GetGrid();
	pg->CommitChangesFromEditor(); // get user input
	for (wxPropertyGridIterator it = pg->GetIterator(); !it.AtEnd(); it++) {
		wxPGProperty* property = *it;
		wxString name = property->GetName();
		if (name != param) continue;
		wxAny value = property->GetValue();
		if (!value.IsNull()) ret = property->GetValueAsString();
	}
	return ret;
}

const wxString ArrayVariableProperty::get_var_pathname() const {
	wxString ret;
	const wxString name = GetLabel();
	const wxString dir = get_value("Dataset");
	if (name == "Episodes") ret = dir + "/all_encounterinfo.csv";
	else if (name == "Vitals") ret = dir + "/all_vitalsigns_numeric.csv";
	else if (name == "Labs") ret = dir + "/all_labs.csv";
	else if (name == "Drugs") ret = dir + "/all_drugs.csv";
	else if (name == "Interventions") ret = dir + "/all_interventions.csv";
	else ret = "";
	return ret;
}

void ArrayVariableProperty::write_cache(const wxArrayString& val) const {
	const wxString name = GetLabel();
	const wxString dir = boost::filesystem::current_path().string();
	const wxString pathname = dir + '/' + name + ".csv";;
	std::fstream fs(pathname.c_str(), std::fstream::out);
	for (wxArrayString::const_iterator it = val.begin(); it != val.end(); it++)
		fs << enquote(*it) << std::endl;
	fs.close();
}

wxArrayString ArrayVariableProperty::read_cache() const {
	wxArrayString ret;
	const wxString name = GetLabel();
	const wxString dir = boost::filesystem::current_path().string();
	const wxString pathname = dir + '/' + name + ".csv";;
	std::fstream fs(pathname.c_str(), std::fstream::in);
	if (fs.fail()) return ret; // file not found
	std::string line;
	while (getline(fs, line)) {
		if (iscntrl(line.back())) line.pop_back(); // remove Microsoft return
		ret.Add(dequote(line));
	}
	fs.close();
	return ret;
}
