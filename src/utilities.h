///////////////////////////////////////////////////////////////////////////////
// Name:        utilities.h
// Purpose:     Declares utility functions
// Author:      Juan Casse
// License:     wxWindows License
///////////////////////////////////////////////////////////////////////////////

#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <string>
#include <vector>

#include "wx/string.h"

bool exists(const wxString& dir);
void create_dir(const wxString& dir);
void remove_dir(const wxString& dir);
void clean_dir(const wxString& dir);
void rename(const wxString& from, const wxString& to);
wxString get_parent(const wxString& dir);
wxString get_path(const wxString& pathname);
wxString get_name(const wxString& pathname);
wxString get_pathname(const wxString& dir);
wxString dequote(const wxString& str);
std::string dequote(const std::string& str);
wxString enquote(const wxString& str);
std::string enquote(const std::string& str);
std::vector<std::string> tokenize(const std::string & line, const char delim);
std::vector<wxString> tokenize2(const std::string & line, const char delim);
std::vector<std::string> get_fields(const std::string & line, const char delim);
std::vector<wxString> get_fields2(const std::string & line, const char delim);
size_t file_size(const wxString& pathname);

#endif
// _UTILITIES_H_
