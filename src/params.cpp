///////////////////////////////////////////////////////////////////////////////
// Name:        params.cpp
// Purpose:     Implements Params class
// Author:      Juan Casse
// License:     wxWindows License
///////////////////////////////////////////////////////////////////////////////

#include "params.h"
#include "utilities.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Datum)
BOOST_CLASS_EXPORT_IMPLEMENT(Value)
BOOST_CLASS_EXPORT_IMPLEMENT(Params)

#include <iostream>
#include <fstream>

const wxString CLOSEDFILENAME("CLOSED");

void Params::add(const std::vector<wxString>& line) {
	add(Param(line));
}

void Params::add(Param& param) {
	iterator it = find(param.get_name());
	if (it == end()) emplace_back(param);
	else set_value(param.get_name(), param.get_value());
}

void Params::add(Param&& param) {
	iterator it = find(param.get_name());
	if (it == end()) emplace_back(param);
	else set_value(param.get_name(), param.get_value());
}

void Params::set_value(const Name& name, const Value& value) {
	iterator it = find(name);
	if (it == end()) emplace_back(Param(name, value));
	else it->set(value);
}

void Params::set_value(const wxString& name, const wxString& value) {
	iterator it = find(name);
	assert(it != end());
	it->set(value);
}

const Param Params::get_param(const wxString& name) const {
	const_iterator it = find(name);
	if (it == end()) throw Exception(name + " not found.");
	return *it;
}

void Params::set_values(const wxString& name, const wxArrayString& values) {
	iterator it = find(name.ToStdString());
	assert(it != end());
	it->set(values);
}

void Params::set_param(const wxString& name, const wxString& parent,
		const wxString& type, const wxString& value) {
	iterator it = find(name.ToStdString());
	if (it == end()) emplace_back(Param(name, parent, type, value));
	else it->set(type, value);
}

void Params::insert(const wxString& name, const wxString& parent,
		const wxString& type, const wxString& value) {
	iterator it = find(name.ToStdString());
	if (it != end()) erase(it);
	std::vector<Param>::insert(begin(), Param(name, parent, type, value));
}

const wxString Params::get_value(const wxString& name) const {
	const_iterator it = find(name.ToStdString());
	if (it == end()) return wxEmptyString;
	return it->second;
}

const wxString Params::get_type(const wxString& name) const {
	assert(!name.empty());
	const_iterator it = find(name);
	assert(it != end());
	return it->get_type();
}

void Params::load(const wxString& pathname) {
	//clear();
	std::fstream fs(pathname.mb_str(), std::fstream::in);
	std::string line;
	while (getline(fs, line)) add(get_fields2(line, ','));
	fs.close();
}

void Value::set(const wxString& value) {
	const iterator it = find(value);
	if (it != end()) {
		begin()->set_priority(it->get_priority());
		it->set_priority(1);
	}
	else {
		if (!empty()) begin()->set_priority(size() + 1);
		emplace_back(value, 1);
	}
	std::make_heap(begin(), end());
}

void Value::set(const wxArrayString& values) {
	clear();
	wxArrayString::const_iterator it;
	for (it = values.begin(); it != values.end(); it++)
		emplace_back(it->ToStdString(), size() + 1);
}

Param::Param(const std::vector<wxString>& line) {
	assert(!line.empty()); // at least name of [name,parent,type,value,...]
	if (line.size() == 1) {
		first = Name(line[0], wxEmptyString, wxEmptyString);
		second = Value();
	}
	else if (line.size() == 2) {
		first = Name(line[0], line[1], wxEmptyString);
		second = Value();
	}
	else if (line.size() == 3) {
		first = Name(line[0], line[1], line[2]);
		second = Value();
	}
	else {
		first = Name(line[0], line[1], line[2]);
		second = Value(line.begin() + 3, line.end());
	}
}

void Param::write_value(std::fstream& fs) const {
	fs << get_value() << std::endl;
}

void Param::write_values(std::fstream& fs) const {
	const wxArrayString values = get_values();
	for (size_t i = 0; i != values.size(); i++)
		fs << (i ? "," : "") << values[i];
	fs << std::endl;
}
