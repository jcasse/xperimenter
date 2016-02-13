///////////////////////////////////////////////////////////////////////////////
// Name:        project.cpp
// Purpose:     Implements Project class
// Author:      Juan Casse
// License:     wxWindows License
///////////////////////////////////////////////////////////////////////////////

#include "project.h"
#include "utilities.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Project)

#include <iostream>
#include <fstream>

void Project::update_fixed_params() {root_->update_fixed_params();}

void Project::load(Project& project, const wxString& dir) {
	wxString pathname = dir + "/project.xml";
	std::ifstream fs(pathname.mb_str());
	boost::archive::xml_iarchive ar(fs);
	//ar >> BOOST_SERIALIZATION_NVP(project);
	ar & boost::serialization::make_nvp("Project", project);
}

void Project::save(const Project& project, const wxString& dir) {
	wxString pathname = dir + "/project.xml";
	::create_dir(dir);
	std::ofstream fs(pathname.mb_str());
	boost::archive::xml_oarchive ar(fs);
	//ar << BOOST_SERIALIZATION_NVP(project);
	ar & boost::serialization::make_nvp("Project", project);
}
