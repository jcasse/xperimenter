///////////////////////////////////////////////////////////////////////////////
// Name:        utilities.cpp
// Purpose:     Implements utility functions
// Author:      Juan Casse
// License:     wxWindows License
///////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "utilities.h"

#include "boost/filesystem.hpp"
#include "boost/filesystem/operations.hpp"

//#include "wx/msgdlg.h"

#include <algorithm>
#include <sstream>

bool exists(const wxString& dir) {
	return boost::filesystem::exists(dir.ToStdString());
}

void create_dir(const wxString& dir) {
	boost::filesystem::path path(dir);
	boost::filesystem::create_directories(path);
}

void remove_dir(const wxString& dir) {
	boost::filesystem::path path(dir);
	boost::filesystem::remove_all(path);
}

void clean_dir(const wxString& dir) {
	namespace fs = boost::filesystem;
	fs::path path(dir.ToStdString());
	fs::directory_iterator end_dir_it;
	for (fs::directory_iterator it(path); it != end_dir_it; ++it) {
		if (fs::is_directory(it->path())) continue;
		fs::remove_all(it->path());
	}
}

void rename(const wxString& from, const wxString& to) {
	boost::filesystem::rename(from.ToStdString(), to.ToStdString());
}

wxString get_parent(const wxString& dir) {
	return dir.substr(0, dir.find_last_of('/'));
}

wxString get_path(const wxString& pathname) {
	return pathname.substr(0, pathname.find_last_of('/'));
}

wxString get_name(const wxString& pathname) {
	return pathname.substr(pathname.find_last_of('/') + 1);
}

wxString get_pathname(const wxString& dir) {
	return dir + '/' + ::get_name(dir) + ".xml";
}

wxString enquote(const wxString& str) {
	return "\"" + str + '\"';
}

std::string enquote(const std::string& str) {
	return "\"" + str + '\"';
}

wxString dequote(const wxString& str) {
	wxString ret = str;
	ret.erase(std::remove(ret.begin(), ret.end(), '\"'), ret.end());
	return ret;
}

std::string dequote(const std::string& str) {
	std::string ret = str;
	ret.erase(std::remove(ret.begin(), ret.end(), '\"'), ret.end());
	return ret;
}

std::vector<std::string> tokenize(const std::string & line, const char delim) {
	std::string field;
	std::vector<std::string> fields;
	std::stringstream linestream(line);
	while(getline(linestream, field, delim))
		fields.emplace_back(dequote(field));
	return fields;
}

std::vector<wxString> tokenize2(const std::string & line, const char delim) {
	std::string field;
	std::vector<wxString> ret;
	std::stringstream linestream(line);
	while(getline(linestream, field, delim))
		ret.emplace_back(dequote(field));
	return ret;
}

std::vector<std::string> get_fields(const std::string & line, const char delim){
	std::string tmp = line;
	if (iscntrl(tmp.back())) tmp.pop_back(); // remove Microsoft end-of-line
	return tokenize(tmp, delim);
}

std::vector<wxString> get_fields2(const std::string & line, const char delim){
	std::string tmp = line;
	if (iscntrl(tmp.back())) tmp.pop_back(); // remove Microsoft end-of-line
	return tokenize2(tmp, delim);
}

size_t file_size(const wxString& pathname) {
	boost::filesystem::path path(pathname);
	assert(boost::filesystem::exists(path));
	//assert(!boost::filesystem::is_regular(path));
	return boost::filesystem::file_size(path);
}
