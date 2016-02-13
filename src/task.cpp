///////////////////////////////////////////////////////////////////////////////
// Name:        task.cpp
// Purpose:     Implements Task class
// Author:      Juan Casse
// License:     wxWindows License
///////////////////////////////////////////////////////////////////////////////

#include "task.h"
#include "utilities.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Task)

#include <iostream>
#include <fstream>

#include "boost/assign/list_of.hpp"

namespace bp = ::boost::process;

Task::Task(const wxString& name, shptr<Task> parent, const Project* project):
	c_(NULL), running_(false), name_(name), parent_(parent), project_(project) {
	//set_fixed_params();
}

void Task::write_params() const {
	const wxString pathname = params_pathname();
	std::fstream fs(pathname.mb_str(), std::fstream::out);
	for (const Param& param : params_) {
		if (param.get_type() == "CAT") continue;
		fs << param.get_name() << ',';
		if (param.get_type() == "LIST") param.write_values(fs);
		else param.write_value(fs);
	}
	fs.close();
}

void Task::set_fixed_params() {
	params_.insert("Output Directory", "General", "DIR", get_dir());
	if (!is_root())
		params_.insert("Input Directory", "General", "DIR",
			get_parent()->get_dir());
	wxString program = wxEmptyString;
	try {program = get_program();}
	catch (Params::Exception e) {}
	params_.insert("Program", "General", "FILE", program);
	params_.insert("Task-specific", "", "CAT", "");
	params_.insert("General", "", "CAT", "");
}

void Task::rename(const wxString& name) {
	name_ = name;
	update_fixed_params();
}

void Task::update_fixed_params() {
	set_fixed_params();
	for (shptr<Task> child : children_) child->update_fixed_params();
}

const wxString Task::get_status() {
	wxString ret;
	Status s = status();
	if (s == NOTREADY) ret = "not ready";
	if (s == READY) ret = "ready";
	if (s == RUNNING) ret = "running";
	if (s == FAILED) ret = "failed";
	if (s == DONE) ret = "done";
	return ret;
}

Task::Status Task::status() {
std::cerr << "params " << params_.size() << std::endl;
	if (params_.empty()) return NOTREADY;
	if (running_) return RUNNING;
	const wxString pathname = get_dir() + "/err";
std::cerr << pathname << std::endl;
	if (::exists(pathname)) {
		if (::file_size(pathname)) return FAILED; // execution failed
		else return DONE;
	}
	else {
		return READY;
	}
}

const wxString Task::get_dir() const {
	if (parent_) return parent_->get_dir() + '/' + name_;
	return project_->get_dir() + '/' + name_;
}

shptr<Task> Task::add_child(shptr<Task> task) {
	//shptr<Task> task = shptr<Task>(new Task(dir, shared_from_this()));
	children_.emplace(task);
	return task;
}

void Task::delete_child(shptr<Task> child) {
	assert(children_.find(child) != children_.end());
	children_.erase(child);
}

bool Task::has_closed_children() const {
	for (shptr<Task> task : children_)
		if (task->status() == DONE) return true;
	return false;
}

shptr<Task> Task::get_root() {
	if (!parent_) return shared_from_this();
	return parent_->get_root();
}

const wxString Task::get_program() const throw(Params::Exception) {
	wxString ret;
	try {ret = get_param("Program");}
	catch (Params::Exception e) {throw e;}
	return ret;
}

bp::child Task::execute() {
	assert(!has_closed_children());
	const wxString cmd = get_param("Program").ToStdString();
	const wxString arg = ::enquote(params_pathname());
	//std::vector<std::string> args = boost::assign::list_of(arg);
	const wxString cmd_args = cmd + ' ' + arg;
	bp::context ctx;
	ctx.work_directory = ::get_path(cmd.ToStdString());
	ctx.environment = bp::self::get_environment();
	ctx.stdin_behavior = bp::inherit_stream();
	ctx.stdout_behavior = bp::inherit_stream();
	ctx.stderr_behavior = bp::inherit_stream();
	//return bp::launch(cmd, args, ctx);
	running_ = true;
	return bp::launch_shell(cmd_args.ToStdString(), ctx);
}

//// helper functions

Task::Task(const wxString& name, shptr<Task> source, shptr<Task> parent):
	c_(NULL), running_(false), name_(name) {
	parent_ = parent;
	project_ = source->project_;
	params_ = source->params_;
	set_param("Input Directory", ::get_parent(get_dir()));
	set_param("Output Directory", get_dir());
}

shptr<Task> Task::deep_copy(const wxString& name,
		shptr<Task> source, shptr<Task> parent) {
	shptr<Task> ret = shptr<Task>(new Task(name, source, parent));
	std::set<shptr<Task>> children = source->get_children();
	for (shptr<Task> child : children) {
		ret->add_child(deep_copy(child->get_name(), child, ret));
	}
	return ret;
}
