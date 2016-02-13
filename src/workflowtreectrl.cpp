////////////////////////////////////////////////////////////////////////////////
// Name:        workflowtreectrl.cpp
// Purpose:     Workflow Tree Control
// Author:      Juan Casse
// License:     wxWindows License
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>

#include "wx/busyinfo.h"

#include "workflowtreectrl.h"
#include "utilities.h"

wxDEFINE_EVENT(TREE_EVENT, wxCommandEvent);
wxDEFINE_EVENT(TREE_EDITING_DONE_EVENT, wxCommandEvent);
wxDEFINE_EVENT(TREE_CONTEXT_EVENT, wxCommandEvent);

//// WorkflowTreeCtrl type definition

IMPLEMENT_CLASS(WorkflowTreeCtrl, wxDataViewTreeCtrl)

//// WorkflowTreeCtrl event table definition

BEGIN_EVENT_TABLE(WorkflowTreeCtrl, wxDataViewTreeCtrl)
	EVT_DATAVIEW_SELECTION_CHANGED(
		wxID_ANY, WorkflowTreeCtrl::OnSelectionChanged)
	EVT_DATAVIEW_ITEM_COLLAPSING(
		wxID_ANY, WorkflowTreeCtrl::OnItemCollapsing)
	EVT_DATAVIEW_ITEM_ACTIVATED(
		wxID_ANY, WorkflowTreeCtrl::OnItemActivated)
	EVT_DATAVIEW_ITEM_START_EDITING(
		wxID_ANY, WorkflowTreeCtrl::OnItemStartEditing)
	EVT_DATAVIEW_ITEM_EDITING_STARTED(
		wxID_ANY, WorkflowTreeCtrl::OnItemEditingStarted)
	EVT_DATAVIEW_ITEM_EDITING_DONE(
		wxID_ANY, WorkflowTreeCtrl::OnItemEditingDone)
	EVT_DATAVIEW_ITEM_CONTEXT_MENU(
		wxID_ANY, WorkflowTreeCtrl::OnItemContextMenu)
END_EVENT_TABLE()

//// WorkflowTreeCtrl creation 

WorkflowTreeCtrl::WorkflowTreeCtrl() {
	Init();
}

WorkflowTreeCtrl::WorkflowTreeCtrl(wxWindow* parent,
		wxWindowID id, const wxPoint& pos, const wxSize& size, long style):
		project_(NULL) {
	Init();
	Create(parent, id, pos, size, style);
}

void WorkflowTreeCtrl::Init() {
	root_ = wxDataViewItem(NULL);
	current_ = root_;
}

bool WorkflowTreeCtrl::Create(wxWindow* parent,
		wxWindowID id, const wxPoint& pos, const wxSize& size, long style) {
	if (!wxDataViewTreeCtrl::Create(parent, id, pos, size, style)) return false;
	return true;
}

//// event handlers

void WorkflowTreeCtrl::OnSelectionChanged(wxDataViewEvent& event) {
std::cerr << "OnSelectionChanged()" << std::endl;
    wxDataViewItem item = event.GetItem();
	if (!item.IsOk()) {
		if (current_.IsOk()) SetCurrentItem(current_);
	}
	else current_ = item;
	// displays correct text after clicking elsewhere while editing text
	if (current_.IsOk()) print_task_status(get_task(current_));
}

void WorkflowTreeCtrl::OnItemCollapsing(wxDataViewEvent& event) {
std::cerr << "OnItemCollapsing()" << std::endl;
	SetCurrentItem(event.GetItem());
	event.Veto();
}

void WorkflowTreeCtrl::OnItemStartEditing(wxDataViewEvent& event) {
std::cerr << "OnItemStartEditing()" << std::endl;
	if (GetId() == ID_DELETE_TASK_TREE_CTRL) event.Veto(); // make read-only
	event.Veto();
	return;

    wxDataViewItem item = event.GetItem();
	shptr<Task> task = ((TaskData*)GetItemData(item))->get_task();
	if (task->is_done()) {print_task_status(task); return;}

	wxString text = GetItemText(item);
	text = text.substr(0, text.find_first_of('(') - 1);
	const wxString name = task->get_name();
	if (text == name) return;

	wxString old_dir = task->get_dir();
	wxString new_dir = ::get_path(old_dir) + '/' + text;
	::rename(old_dir, new_dir);
	task->set_name(text);

	print_task_status(task);

	emit_changed_event();
}

void WorkflowTreeCtrl::OnItemEditingStarted(wxDataViewEvent& event) {
std::cerr << "OnItemEditingStarted()" << std::endl;
}

void WorkflowTreeCtrl::OnItemEditingDone(wxDataViewEvent& event) {
std::cerr << "OnItemEditingDone()" << std::endl;
	emit_editing_done_event();
}

void WorkflowTreeCtrl::OnItemActivated(wxDataViewEvent& event) {
std::cerr << "OnItemActivated()" << std::endl;
}

void WorkflowTreeCtrl::OnItemContextMenu(wxDataViewEvent& event) {
std::cerr << "OnItemContextMenu()" << std::endl;
	if (GetId() == ID_DELETE_TASK_TREE_CTRL) return;
    wxDataViewItem item = event.GetItem();
	if (root_ && !item.IsOk()) return;	// right-clicked on empty space
	emit_context_event();
}

//// API

/*
void WorkflowTreeCtrl::update_status(wxDataViewItem& item) {
	shptr<Task> task = get_task(item);
	const wxString text = task->get_name() +
		" (" + (status.empty() ? task->get_status() : status) + ')';
	SetItemText(item, text);
	wxDataViewItemArray children = GetChildren(item);
}
*/

void WorkflowTreeCtrl::print_task_status(shptr<Task> task,
		const wxString& status) {
	const wxString text = task->get_name() +
		" (" + (status.empty() ? task->get_status() : status) + ')';
	wxDataViewItem item = get_item(task);
	SetItemText(item, text);
}

void WorkflowTreeCtrl::add_task(const wxString& name) {
	wxDataViewItem item = GetCurrentItem();
	shptr<Task> parent = get_task(item);

	//const wxString dir =
	//	(parent ? parent->get_dir() : project_->get_dir()) + '/' + name;
	shptr<Task> task = shptr<Task>(new Task(name, parent, project_));
	//if (!create_dir(task->get_dir()))  return;
	if (parent && name_exists(name, parent)) {
		const wxString msg = "Task \"" + name + "\" already exists.";
		wxMessageBox(msg, "Task Creation Aborted", wxICON_ERROR);
		return;
	}

	if (parent) {							// add child task
		//shptr<Task> task = parent->add_child(name, parent, project_);
		parent->add_child(task);
		add_item(item, task);
		emit_changed_event();
	}
	else {									// add root task
		//shptr<Task> root(new Task(dir));
		//build_tree(root);
		build_tree(task);
	}
}

bool WorkflowTreeCtrl::name_exists(const wxString& name, shptr<Task> task) {
	bool ret = false;
	std::set<shptr<Task>> children = task->get_children();
	for (shptr<Task> child : children) {
		if (name == child->get_name()) ret = true;
	}
	return ret;
}

void WorkflowTreeCtrl::rename_task(const wxString& name) {
	wxDataViewItem item = GetCurrentItem();
	shptr<Task> task = get_task(item);
	const wxString old_dir = task->get_dir();
	task->rename(name);
	::rename(old_dir, task->get_dir());
	print_task_status(task);
}

void WorkflowTreeCtrl::copy_task(const wxString& name) {
	wxDataViewItem item = GetParent(GetCurrentItem());
	if (!item) return;	// should not copy root (tree has only one root)

	shptr<Task> parent = get_task(item);
	if (name_exists(name, parent)) {
		const wxString msg = "Task \"" + name + "\" already exists.";
		wxMessageBox(msg, "Task Creation Aborted", wxICON_ERROR);
		return;
	}

	// copy task sub-tree
	shptr<Task> task =
		parent->add_child(
			Task::deep_copy(name, get_task(GetCurrentItem()), parent));

/*
	shptr<Task> task = shptr<Task>(new Task(name, parent, project_));
	//if (!create_dir(task->get_dir()))  return;

	//shptr<Task> task = get_task(item)->add_child(dir);
	parent->add_child(task);
	task->set_params(get_task(GetCurrentItem())->get_params());
	task->set_param("Output Directory", task->get_dir());
*/

	// copy workflow sub-tree
	build(add_item(item, task));

	emit_changed_event();
}

void WorkflowTreeCtrl::delete_task() {
	wxDataViewItem item = GetCurrentItem();
	shptr<Task> task = get_task(item);

std::cerr << "delete_item()" << std::endl;
	delete_item(item);

std::cerr << "remove_dir()" << std::endl;
	::remove_dir(task->get_dir());

std::cerr << "get_parent()" << std::endl;
	shptr<Task> parent = task->get_parent();
	if (parent) {
std::cerr << "delete_child()" << std::endl;
		parent->delete_child(task); // removes subtree via shared_ptr
	}
	else {
std::cerr << "root_ = NULL" << std::endl;
		root_ = wxDataViewItem(NULL);
	}

	emit_changed_event();
}

void WorkflowTreeCtrl::clear() {
	Init();
	UnselectAll();
	DeleteAllItems();
}

void WorkflowTreeCtrl::build_tree(shptr<Task> root) {
	clear();
	if (!root) return;
	root_ = wxDataViewItem(build(add_item(wxDataViewItem(NULL), root)));
	current_ = root_;
	SetCurrentItem(root_);
	emit_changed_event();
}

//// Helper functions

wxDataViewItem WorkflowTreeCtrl::build(const wxDataViewItem& parent) {
	TaskData* data = (TaskData*)GetItemData(parent);
	std::set<shptr<Task>> children = data->get_task()->get_children();
	for (shptr<Task> task : children) {
		wxDataViewItem item = build(add_item(parent, task));
	}
	return parent;
}

wxDataViewItem WorkflowTreeCtrl::add_item(
		const wxDataViewItem& parent, shptr<Task> task) {
    const wxDataViewItem item = AppendContainer(
		parent, task->get_name(), -1, -1, new TaskData(task));
	const shptr<Task> t = ((TaskData*)GetItemData(item))->get_task();
	print_task_status(t);
	Expand(item);
	SetCurrentItem(item);
	return item;
}

void WorkflowTreeCtrl::delete_item(const wxDataViewItem& item) {
std::cerr << "GetParent()" << std::endl;
	wxDataViewItem parent = GetParent(item);
	current_ = parent;
	if(parent.IsOk()) {
std::cerr << "SetCurrentItem()" << std::endl;
		SetCurrentItem(parent);
	}
std::cerr << "UnselectAll()" << std::endl;
	UnselectAll();
std::cerr << "DeleteChildren()" << std::endl;
	DeleteChildren(item);
std::cerr << "DeleteItem()" << std::endl;
	DeleteItem(item);
}

shptr<Task> WorkflowTreeCtrl::get_task(const wxDataViewItem& item) {
	if (!item.IsOk()) return NULL;
	return get_data(item)->get_task();
}

WorkflowTreeCtrl::TaskData* WorkflowTreeCtrl::get_data(
		const wxDataViewItem& item) {
	if (!item.IsOk()) return NULL;
	return (TaskData*)GetItemData(item);
}

wxDataViewItem WorkflowTreeCtrl::GetParent(const wxDataViewItem& item) {
	return GetStore()->GetParent(item);
}

wxDataViewItemArray WorkflowTreeCtrl::GetChildren(const wxDataViewItem& item) {
	wxDataViewItemArray ret;
	unsigned int count = GetStore()->GetChildren(item, ret);
	return ret;
}

bool WorkflowTreeCtrl::create_dir(const wxString& dir) {
	if (::exists(dir)) {
		const wxString msg = "Directory \"" + dir + "\" already exists.";
		wxMessageBox(msg, "Task Creation Aborted", wxICON_ERROR);
		return false;
	}
	::create_dir(dir);
	return true;
}

wxDataViewItem WorkflowTreeCtrl::find_item(
		const wxDataViewItem& item, shptr<Task> task) {
	if (get_task(item) == task) return item;
	wxDataViewItemArray children = GetChildren(item);
	wxDataViewItemArray::iterator it;
	for (it = children.begin(); it != children.end(); it++) {
		wxDataViewItem ret = find_item(*it, task);
		if (ret.IsOk()) return ret;
	}
	return wxDataViewItem(NULL);
}

void WorkflowTreeCtrl::emit_changed_event() {
	wxCommandEvent event(TREE_EVENT, GetId());
	event.SetEventObject(this);
	assert(ProcessEvent(event));
}

void WorkflowTreeCtrl::emit_editing_done_event() {
	wxCommandEvent event(TREE_EDITING_DONE_EVENT, GetId());
	event.SetEventObject(this);
	assert(ProcessEvent(event));
}

void WorkflowTreeCtrl::emit_context_event() {
	wxCommandEvent event(TREE_CONTEXT_EVENT, GetId());
	event.SetEventObject(this);
	assert(ProcessEvent(event));
}
