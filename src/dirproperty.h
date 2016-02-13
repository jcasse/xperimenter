////////////////////////////////////////////////////////////////////////////////
// Name:        dirproperty.h
// Purpose:     Declares Porperty Grid dir property that validates value
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

#ifndef _DIRPROPERTY_H_
#define _DIRPROPERTY_H_

//// Includes

#include "wx/wx.h"
#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/property.h"

//// DirProperty class declaration

class DirProperty : public wxDirProperty {
	WX_PG_DECLARE_PROPERTY_CLASS(DirProperty)

public:
	DirProperty(
		const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL,
		const wxString& value = wxEmptyString);

	virtual bool ValidateValue(wxVariant& value,
		wxPGValidationInfo& validationinfo) const override;
};

#endif
//// _DIRPROPERTY_H_
