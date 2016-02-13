///////////////////////////////////////////////////////////////////////////////
// Name:        project.h
// Purpose:     Declares Project class, which specifies a workflow tree
// Author:      Juan Casse
// License:     wxWindows License
///////////////////////////////////////////////////////////////////////////////

#ifndef _PROJECT_H_
#define _PROJECT_H_

#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"
#include "boost/archive/xml_oarchive.hpp"
#include "boost/archive/xml_iarchive.hpp"
#include "boost/serialization/serialization.hpp"
#include "boost/serialization/tracking.hpp"
#include "boost/serialization/base_object.hpp"
#include "boost/serialization/nvp.hpp"
#include "boost/serialization/export.hpp"
#include "boost/serialization/string.hpp"
#include "boost/serialization/shared_ptr.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "boost/serialization/set.hpp"

#include "task.h"

class Task;

class Project {
public:
	Project(const wxString& dir = wxString(), shptr<Task> root = NULL):
		dir_(dir), root_(root) {}

	void set_dir(const wxString& dir) {dir_ = dir.ToStdString();}
	void set_root(shptr<Task> root) {root_ = root;}
	void update_fixed_params();

	const wxString get_dir() const {return dir_;}
	shptr<Task> get_root() const {return root_;}

	static void load(Project& project, const wxString& dir);
	static void save(const Project& project, const wxString& dir);

private:
	std::string dir_;				// directory where this project lives
	shptr<Task> root_;				// root of workflow tree

	friend class boost::serialization::access;
	template<typename Ar>
	void serialize(Ar& ar, const unsigned int ver) {
		ar & boost::serialization::make_nvp("Dir", dir_);
		ar & boost::serialization::make_nvp("Root", root_);
	}

	void depth_first_execute(shptr<Task> task);
};

BOOST_CLASS_EXPORT_KEY(Project)

#endif
// _PROJECT_H_
