////////////////////////////////////////////////////////////////////////////////
// Name:        taskcontextmenu.cpp
// Purpose:     Defines TaskContextMenu class
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows License
////////////////////////////////////////////////////////////////////////////////

#include "taskcontextmenu.h"

TaskContextMenu::TaskContextMenu(shptr<Task> task): wxMenu(), task_(task) {

	Append(ID_TASK_ADD, wxT("&New"),
			wxT("Add new task under the current task."));
	AppendSeparator();
	Append(ID_TASK_RENAME, wxT("&Rename"),
			wxT("Rename task."));
	Append(ID_TASK_COPY, wxT("&Copy"),
			wxT("Make copy of current task."));
	Append(ID_TASK_DELETE, wxT("&Delete"),
			wxT("Delete the current task."));
	AppendSeparator();
	Append(ID_TASK_VIEW, wxT("&View"),
			wxT("Shows the current task's parameters."));
	Append(ID_TASK_EDIT, wxT("&Edit"),
			wxT("Edit the current task's parameters."));
	Append(ID_TASK_CLEAR, wxT("&Clear"),
			wxT("Clear the current task's results."));
	AppendSeparator();
	Append(ID_TASK_EXECUTE, wxT("&Execute"),
			wxT("Execute the current task."));
	//Append(ID_TASK_KILL, wxT("&Terminate"),
	//		wxT("Terminate the current task."));

	disable();
	set_enable();
}

void TaskContextMenu::disable() {
		//Enable(ID_TASK_ADD, false);
		Enable(ID_TASK_RENAME, false);
		Enable(ID_TASK_COPY, false);
		Enable(ID_TASK_DELETE, false);
		Enable(ID_TASK_VIEW, false);
		Enable(ID_TASK_EDIT, false);
		Enable(ID_TASK_CLEAR, false);
		Enable(ID_TASK_EXECUTE, false);
		//Enable(ID_TASK_KILL, false);
}

void TaskContextMenu::set_enable() {
	if (!task_) return;

	Enable(ID_TASK_VIEW, true);

	Task::Status status = task_->status();
	switch (status) {
	case Task::READY:
	case Task::FAILED:
		Enable(ID_TASK_RENAME, true);
		Enable(ID_TASK_EDIT, true);
		if (!task_->has_closed_children()) Enable(ID_TASK_CLEAR, true);
		Enable(ID_TASK_EXECUTE, true);
		//Enable(ID_TASK_KILL, false);
		break;
	case Task::NOTREADY:
		Enable(ID_TASK_RENAME, true);
		Enable(ID_TASK_EDIT, true);
		Enable(ID_TASK_CLEAR, false);
		Enable(ID_TASK_EXECUTE, false);
		//Enable(ID_TASK_KILL, false);
		break;
	case Task::RUNNING:
		Enable(ID_TASK_RENAME, false);
		Enable(ID_TASK_EDIT, false);
		Enable(ID_TASK_CLEAR, false);
		Enable(ID_TASK_EXECUTE, false);
		//Enable(ID_TASK_KILL, true);
		break;
	case Task::DONE:
		Enable(ID_TASK_RENAME, true);
		Enable(ID_TASK_EDIT, false);
		if (!task_->has_closed_children()) Enable(ID_TASK_CLEAR, true);
		Enable(ID_TASK_EXECUTE, false);
		//Enable(ID_TASK_KILL, false);
		break;
	default: assert(false);
	}

	if (IsEnabled(ID_TASK_ADD))
		if (!task_->is_root()) Enable(ID_TASK_COPY, true);
	else Enable(ID_TASK_COPY, false);
	if (IsEnabled(ID_TASK_EDIT)) Enable(ID_TASK_DELETE, true);
	else Enable(ID_TASK_DELETE, false);
}
