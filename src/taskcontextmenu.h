////////////////////////////////////////////////////////////////////////////////
// Name:        taskcontextmenu.h
// Purpose:     Declares TaskContextMenu class
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

#ifndef _TASK_CONTEXT_MENU_H_
#define _TASK_CONTEXT_MENU_H_

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
#include "wx/menu.h"

#include "task.h"

enum {ID_TASK_CONTEXT_MENU = 1500,
	ID_TASK_RENAME,
	ID_TASK_COPY,
	ID_TASK_DELETE,
	ID_TASK_ADD,
	ID_TASK_EDIT,
	ID_TASK_VIEW,
	ID_TASK_CLEAR,
	ID_TASK_EXECUTE,
	ID_TASK_KILL
};

class TaskContextMenu: public wxMenu {
public:
	TaskContextMenu(shptr<Task> task);

private:
	shptr<Task> task_;

	void disable();
	void set_enable();
};

#endif
// _TASK_CONTEXT_MENU_H_
