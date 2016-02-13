////////////////////////////////////////////////////////////////////////////////
// Name:        dirproperty.cpp
// Purpose:     Defines custom Property Grid Dir Property that validates value
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

//#ifdef __GNUG__
//#pragma implementation "dirproperty.h"
//#endif

#include <iostream>

#include "boost/filesystem.hpp"

#include "dirproperty.h"

//// DirProperty type definition

WX_PG_IMPLEMENT_PROPERTY_CLASS(DirProperty,				// Property name
								wxDirProperty,			// Inherited from
								wxString,				// Value type name
								const wxString&,		// Value type from ctr
								TextCtrlAndButton)		// Initial editor

DirProperty::DirProperty(const wxString& label,
		const wxString& name, const wxString& value):
		wxDirProperty(label, name, value) {}

bool DirProperty::ValidateValue(wxVariant& value,
		wxPGValidationInfo& validationinfo) const {
	const wxString dir = value.GetString();
	namespace fs = boost::filesystem;
	return fs::exists(fs::path(dir.ToStdString()));
}
