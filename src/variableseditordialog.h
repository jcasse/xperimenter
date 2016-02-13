////////////////////////////////////////////////////////////////////////////////
// Name:        variableseditordialog.h
// Purpose:     Declares dialog to select list of variables
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

#ifndef _VARIABLESEDITORDIALOG_H_
#define _VARIABLESEDITORDIALOG_H_

//#ifdef __GNUG__
//#pragma interface "variableseditordialog.cpp"
//#endif

// Includes

#include <wx/statline.h>
#include <wx/arrstr.h>
#include <wx/listbox.h>
#include <string>
#include <set>
#include <fstream>

#include "utilities.h"

// Control identifiers

enum {
	ID_VARIABLES_EDITOR_DIALOG = 20000,
	ID_VE_FROM_FILE = 20001,
	ID_VE_DATASETLISTBOX = 20005,
	ID_VE_PROJECTLISTBOX = 20010,
	ID_VE_MOVETODATASET = 20008,
	ID_VE_MOVETOPROJECT = 20007,
	ID_VE_DATASETLABEL = 20011,
	ID_VE_PROJECTLABEL = 20012,
	ID_VE_ALL_IN = 20013,
	ID_VE_ALL_OUT = 20014
};

// VariablesEditorDialog class declaration

class VariablesEditorDialog: public wxDialog {
	DECLARE_CLASS(VariablesEditorDialog)
	DECLARE_EVENT_TABLE()

public:
	VariablesEditorDialog();
	VariablesEditorDialog(wxWindow* parent,
			const wxString& default_dir = wxEmptyString,
			wxWindowID id = ID_VARIABLES_EDITOR_DIALOG,
			const wxString& caption = wxT("Variables Editor Dialog"),
			const wxArrayString& dataset_vars = wxArrayString(),
			const wxArrayString& project_vars = wxArrayString(),
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU);

	// Member initialization
	void Init();

	// Creation
	bool Create(wxWindow* parent,
			wxWindowID id = ID_VARIABLES_EDITOR_DIALOG,
			const wxString& caption = wxT("Variables Editor Dialog"),
			const wxArrayString& dataset_vars = wxArrayString(),
			const wxArrayString& project_vars = wxArrayString(),
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxCLOSE_BOX|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU);

	// Creates the controls and sizers
	void CreateControls();
	void InitializeControls(const wxArrayString& dataset_vars,
		const wxArrayString& project_vars);

	const wxString get_dir() const {return default_dir_;}
	void SetDialogValue(const wxVariant& value);
	wxVariant GetDialogValue() const;
	bool IsModified();

	//// VariablesEditorDialog event handler declarations

	void OnMoveToProject(wxCommandEvent& event);
	void OnMoveToDataset(wxCommandEvent& event);
	void OnAllIn(wxCommandEvent& event);
	void OnAllOut(wxCommandEvent& event);
	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnFromFile(wxCommandEvent& event);

private:
	bool modified_;
	wxString default_dir_;

	wxStaticText* datasetlabel;
	wxStaticText* projectlabel;
	wxListBox* datasetlistbox;
	wxListBox* projectlistbox;
	wxButton* movetodatasetb;
	wxButton* movetoprojectb;

	wxArrayString load_list(const wxString& pathname);
	bool validate_list(wxArrayString& list, const wxArrayString& dataset_vars);
	void update_listboxes();
	void update_label(const wxWindowID id, const wxListBox* lb);
	void move(wxListBox* to, wxListBox* from, wxArrayInt& sel);
	void move_all(wxListBox* to, wxListBox* from);
	static int descending_order(int* first, int* second) {
		if (*first > *second) return -1;
		else if (*first < *second) return 1;
		else return 0;
	}
};

#endif
// _VARIABLESEDITORDIALOG_H_
