////////////////////////////////////////////////////////////////////////////////
// Name:        fileproperty.h
// Purpose:     Declares Porperty Grid file property that validates value
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

#ifndef _FILEPROPERTY_H_
#define _FILEPROPERTY_H_

//// Includes

#include "wx/wx.h"
#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/property.h"

//// FileProperty class declaration

class FileProperty : public wxFileProperty {
	WX_PG_DECLARE_PROPERTY_CLASS(FileProperty)

public:
	FileProperty(
		const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL,
		const wxString& value = wxEmptyString);

	virtual bool ValidateValue(wxVariant& value,
		wxPGValidationInfo& validationinfo) const override;
};

#endif
//// _FILEPROPERTY_H_
