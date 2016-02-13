////////////////////////////////////////////////////////////////////////////////
// Name:        taskthread.h
// Purpose:     Declares thread class to execute a task
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

#ifndef _TASKTHREAD_H_
#define _TASKTHREAD_H_

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
#include "wx/thread.h"

#include "frame.h"
#include "task.h"

//#ifdef __GNUG__
//#pragma interface "taskthread.cpp"
//#endif

enum {
	WORKER_EVENT = 333
};

class MyWorkerThread: public wxThread {
public:
	MyWorkerThread(MyFrame *frame, shptr<Task> task);
	~MyWorkerThread();

	// thread execution starts here
	virtual wxThread::ExitCode Entry() override;

	// called when the thread exits - whether it terminates normally or is
	// stopped with Delete() (but not when it is Kill()ed!)
	virtual void OnExit() override;

	void emit_event(int status, const wxString& msg = wxEmptyString);

private:
	int m_status;
	MyFrame *m_frame;
	shptr<Task> m_task;
};

#endif
// _TASKTHREAD_H_
