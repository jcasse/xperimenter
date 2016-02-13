////////////////////////////////////////////////////////////////////////////////
// Name:        workflowtreectrl.h
// Purpose:     Workflow Tree Control
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

#ifndef _WORKFLOW_TREE_CTRL_H_
#define _WORKFLOW_TREE_CTRL_H_

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
#include "wx/dataview.h"

//#ifdef __GNUG__
//#pragma interface "workflowtreectrl.cpp"
//#endif

#include "task.h"
#include "project.h"

//// WorkflowTreeCtrl class declaration

enum {
	ID_WORKFLOW_TREE_CTRL = 800,
	ID_MAIN_TREE_CTRL = 801,
	ID_DELETE_TASK_TREE_CTRL = 802
};

wxDECLARE_EVENT(TREE_EVENT, wxCommandEvent);
wxDECLARE_EVENT(TREE_EDITING_DONE_EVENT, wxCommandEvent);
wxDECLARE_EVENT(TREE_CONTEXT_EVENT, wxCommandEvent);

class WorkflowTreeCtrl: public wxDataViewTreeCtrl {

	DECLARE_CLASS(WorkflowTreeCtrl)
	DECLARE_EVENT_TABLE()

public:
	WorkflowTreeCtrl();
	WorkflowTreeCtrl(wxWindow* parent,
			wxWindowID id = ID_WORKFLOW_TREE_CTRL,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxDV_NO_HEADER|wxDV_ROW_LINES);

	bool Create(wxWindow* parent,
			wxWindowID id = ID_WORKFLOW_TREE_CTRL,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxDV_NO_HEADER|wxDV_ROW_LINES);

	void Init();

	void print_task_status(shptr<Task> task,
		const wxString& status = wxEmptyString);

	shptr<Task> get_root() {return get_task(root_);}
	shptr<Task> get_task() {return get_task(GetCurrentItem());}

	void clear();
	void add_task(const wxString& name);
	void rename_task(const wxString& name);
	void copy_task(const wxString& name);
	void delete_task();

	void build_tree(shptr<Task> root);
	void set_project(const Project* project) {project_ = project;}

	void OnSelectionChanged(wxDataViewEvent& event);
	void OnItemCollapsing(wxDataViewEvent& event);
	void OnItemActivated(wxDataViewEvent& event);
	void OnItemStartEditing(wxDataViewEvent& event);
	void OnItemEditingStarted(wxDataViewEvent& event);
	void OnItemEditingDone(wxDataViewEvent& event);
	void OnItemContextMenu(wxDataViewEvent& event);

private:
	/// Define TaskData contained in wxDataViewItem of wxDataViewTreeCtrl

	class TaskData: public wxClientData {
	public:
		TaskData(shptr<Task> task = NULL): task_(task) {}
		shptr<Task> get_task() {return task_;}
	private:
		shptr<Task> task_;
	};

	/// WorkflowTreeCtrl member variables

	const Project* project_;
	wxDataViewItem root_;
	wxDataViewItem current_;

	/// helper functions

	wxDataViewItem GetParent(const wxDataViewItem& item);
	wxDataViewItemArray GetChildren(const wxDataViewItem& item);

	wxDataViewItem build(const wxDataViewItem& parent);
	wxDataViewItem add_item(const wxDataViewItem& parent, shptr<Task> t);
	void delete_item(const wxDataViewItem& item);

	shptr<Task> get_task(const wxDataViewItem& item);
	TaskData* get_data(const wxDataViewItem& item);
	bool create_dir(const wxString& dir);

	bool name_exists(const wxString& name, shptr<Task> task);

	wxDataViewItem get_item(shptr<Task> task) {return find_item(root_, task);}
	wxDataViewItem find_item(const wxDataViewItem& item, shptr<Task> task);

	void emit_changed_event();
	void emit_editing_done_event();
	void emit_context_event();
};

#endif
// _WORKFLOW_TREE_CTRL_H_
