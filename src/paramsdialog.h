////////////////////////////////////////////////////////////////////////////////
// Name:        paramsdialog.h
// Purpose:     Dialog to set the parameters for a task
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

#ifndef _PARAMSDIALOG_H_
#define _PARAMSDIALOG_H_

//#ifdef __GNUG__
//#pragma interface "paramsdialog.cpp"
//#endif

//// Includes

#include <string>
#include <map>
#include <vector>

#include "task.h"

#include "wx/textctrl.h"
#include "wx/propgrid/propgrid.h"

//// Control identifiers

enum {
	ID_PARAMS_DIALOG = 600,
	ID_PARAMS_DIALOG_PROP_GRID = 601,
	ID_PARAMS_DIALOG_LOAD = 602
};

//// wxPropertyGrid Property Classes Identifiers

//// ParamsDialog class declaration

class ParamsDialog: public wxDialog {
	DECLARE_CLASS(ParamsDialog)
	DECLARE_EVENT_TABLE()

public:
	ParamsDialog();
	ParamsDialog(wxWindow* parent, shptr<Task> task,
		const wxString& default_dir = wxEmptyString,
		const bool readonly = false,
		wxWindowID id = ID_PARAMS_DIALOG,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxCAPTION|wxSYSTEM_MENU|wxRESIZE_BORDER);

	// Member initialization
	void Init();

	// Creation
	bool Create(wxWindow* parent, shptr<Task> Task,
		wxWindowID id = ID_PARAMS_DIALOG,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxCAPTION|wxSYSTEM_MENU|wxRESIZE_BORDER);

	// Creates the controls and sizers
	void CreateControls();
	void InitializeControls();

	const wxString get_dir() const {return default_dir_;}
	const Params get_params() const {return params_;}

	//// ParamsDialog event handler declarations

	void OnFromFile(wxCommandEvent& event);
	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);

private:

	//// ParamsDialog member variables

	bool readonly_;
	wxString default_dir_;
	Params params_;
	shptr<Task> task_;

	wxPropertyGrid* pg_;

	void build_grid();
	wxPGProperty* make_property(const Param& param);
	bool set_params();
	void init_params();
	void set_param_headers();
	void set_fixed_params();
	wxArrayString invalid_params();
	void display_invalid_params(const wxArrayString& params);
};

#endif
// _PARAMSDIALOG_H_
