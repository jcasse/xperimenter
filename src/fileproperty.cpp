////////////////////////////////////////////////////////////////////////////////
// Name:        fileproperty.cpp
// Purpose:     Defines custom Property Grid File Property that validates value
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

//#ifdef __GNUG__
//#pragma implementation "fileproperty.h"
//#endif

#include <iostream>

#include "boost/filesystem.hpp"

#include "fileproperty.h"

//// FileProperty type definition

WX_PG_IMPLEMENT_PROPERTY_CLASS(FileProperty,			// Property name
								wxFileProperty,			// Inherited from
								wxString,				// Value type name
								const wxString&,		// Value type from ctr
								TextCtrlAndButton)		// Initial editor

FileProperty::FileProperty(const wxString& label,
		const wxString& name, const wxString& value):
		wxFileProperty(label, name, value) {}

bool FileProperty::ValidateValue(wxVariant& value,
		wxPGValidationInfo& validationinfo) const {
	const wxString pathname = value.GetString();
	namespace fs = boost::filesystem;
	return fs::exists(fs::path(pathname.ToStdString()));
}
