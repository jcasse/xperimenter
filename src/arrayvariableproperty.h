////////////////////////////////////////////////////////////////////////////////
// Name:        arrayvariableproperty.h
// Purpose:     Declares Porperty Grid property for selecting list of variables
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

#ifndef _ARRAYVARIABLEPROPERTY_H_
#define _ARRAYVARIABLEPROPERTY_H_

#include "wx/wx.h"
#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/property.h"

#include "variableseditordialog.h"

class ArrayVariableProperty: public wxArrayStringProperty {
	WX_PG_DECLARE_PROPERTY_CLASS(ArrayVariableProperty)

public:
	ArrayVariableProperty(
		const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL,
		const wxArrayString& value = wxArrayString());

	virtual ~ArrayVariableProperty();

	bool OnCustomStringEdit(wxWindow*, wxString&) override;

	bool OnButtonClick(wxPropertyGrid* propgrid,
		wxWindow* WXUNUSED(primaryCtrl), const wxChar* cbt) override;

	virtual bool ValidateValue(wxVariant& value,
		wxPGValidationInfo& validationinfo) const override;

protected:
	wxArrayString dataset_vars_; // all vars from dataset
	wxArrayString project_vars_; // vars selected for project

private:
	wxArrayString get_dataset_vars() const;
	const wxString get_var_pathname() const;
	const wxString get_value(const wxString& param) const;
	const std::set<wxString> get_set(const wxArrayString& val) const;
	wxArrayString get_valid(const wxArrayString& val);
	void write_cache(const wxArrayString& vars) const;
	wxArrayString read_cache() const;
};

#endif
