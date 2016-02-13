///////////////////////////////////////////////////////////////////////////////
// Name:        task.h
// Purpose:     Declares Task class
// Author:      Juan Casse
// License:     wxWindows License
///////////////////////////////////////////////////////////////////////////////

#ifndef _TASK_H_
#define _TASK_H_

#include "boost/filesystem.hpp"
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

#include "boost/process.hpp"

#include <string>
#include <set>

#include "wx/string.h"
#include "wx/dataview.h"

#include "params.h"
#include "project.h"
#include "utilities.h"

class Project;

template<typename T> using shptr = boost::shared_ptr<T>;

class Task: public boost::enable_shared_from_this<Task> {
public:
	enum Status {NOTREADY, READY, RUNNING, FAILED, DONE};

	Task(): c_(NULL), running_(false), name_(), parent_(NULL), project_(NULL) {}
	Task(const wxString& name, shptr<Task> parent, const Project* project);
	//Task(const Task& source) {shallow_copy(source);}

	void set_name(const wxString& name) {name_ = name.ToStdString();}
	void set_params(const Params& params) {params_ = params;}
	void load_params(const wxString& pathname) {params_.load(pathname);}
	shptr<Task> add_child(shptr<Task> task);
	void delete_child(shptr<Task> child);
	void set_running(const bool val) {running_ = val;}
	void set_process(boost::process::child* c) {c_ = c;}
	void set_param(const wxString& name, const wxString& value)
	{params_.set_value(name, value);}
	void set_fixed_params();
	void update_fixed_params();
	void rename(const wxString& name);

	inline bool operator <(const Task& rhs)
	{return get_dir() < rhs.get_dir();}
	inline bool operator >(const Task& rhs)
	{return get_dir() > rhs.get_dir();}

	const Params get_params() const {return params_;}
	void write_params() const;

	bool is_root() const {return parent_ ? false : true;}
	Status status();
	bool is_done() {return status() == DONE;}
	bool is_ready() {return status() == READY;}
	bool is_notready() {return status() == NOTREADY;}
	bool is_running() {return status() == RUNNING;}
	bool has_closed_children() const;
	boost::process::child* get_process() {return c_;}

	const wxString get_status();
	//const wxString get_dir() const {return dir_;}
	const wxString get_dir() const;
	const wxString get_program() const throw(Params::Exception);
	const wxString get_name() const {return name_;}
	const wxString get_param(const wxString& name) const
	{return params_.get_param(name).get_value();}

	shptr<Task> get_root();
	shptr<Task> get_parent() {return parent_;}
	std::set<shptr<Task>> get_children() {return children_;}

	boost::process::child execute();

	static shptr<Task> deep_copy(const wxString& name,
		shptr<Task> source, shptr<Task> parent);

	//wxDataViewItem item;				// corresponding GUI item

private:
	boost::process::child* c_;			// process thread
	bool running_;

	std::string name_;
	shptr<Task> parent_;				// pointer to parent task
	const Project* project_;
	std::set<shptr<Task>> children_;	// pointers to child tasks
	Params params_;						// list of parameters for task

	friend class boost::serialization::access;
	template<typename Ar>
	void serialize(Ar& ar, const unsigned int ver) {
		ar & boost::serialization::make_nvp("Name", name_);
		ar & boost::serialization::make_nvp("Parent", parent_);
		ar & boost::serialization::make_nvp("Project", project_);
		ar & boost::serialization::make_nvp("Children", children_);
		ar & boost::serialization::make_nvp("Params", params_);
	}

	Task(const wxString& name, shptr<Task> source, shptr<Task> parent);

	const wxString params_pathname() const {return get_dir() + "/params.csv";}
};

BOOST_CLASS_EXPORT_KEY(Task)

#endif
// _TASK_H_
