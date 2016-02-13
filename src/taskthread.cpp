////////////////////////////////////////////////////////////////////////////////
// Name:        taskthread.cpp
// Purpose:     Defines thread class to execute a task
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows License
////////////////////////////////////////////////////////////////////////////////

//#ifdef __GNUG__
//#pragma implementation "taskthread.h"
//#endif

#include <sys/types.h>

#include "taskthread.h"

namespace bp = ::boost::process;

MyWorkerThread::MyWorkerThread(MyFrame *frame, shptr<Task> task):
	wxThread(), m_status(-1), m_frame(frame), m_task(task) {}

MyWorkerThread::~MyWorkerThread() {
	// remove itself from list
	wxCriticalSectionLocker locker(wxGetApp().m_critsect);
	wxArrayThread& threads = wxGetApp().m_threads;
	threads.Remove(this);

	// signal the main thread that there are no more threads left if it is
	// waiting for us
	if (threads.IsEmpty()) {
		if (wxGetApp().m_shuttingDown) {
			wxGetApp().m_shuttingDown = false;
			wxGetApp().m_semAllDone.Post();
		}
	}
}

void MyWorkerThread::OnExit() {
	m_task->set_running(false);
	m_task->set_process(NULL);
	emit_event(m_status);
}

wxThread::ExitCode MyWorkerThread::Entry() {

	// check if the application is shutting down:
	// in this case thread should stop a.s.a.p.
	{
		wxCriticalSectionLocker locker(wxGetApp().m_critsect);
		if ( wxGetApp().m_shuttingDown ) return NULL;
	}

	if (m_frame->Canceled()) return NULL;

	// check if we were asked to exit
	if (TestDestroy()) return NULL;

	if (!m_task->is_ready()) return NULL;

	m_task->set_running(true);

	emit_event(m_status);

	// task->execute() runs a shell to call xperimenter.R which processes the
	// params.csv file and, in turn, runs another shell to call the actual task
	// program with the parameters input by the user.
	bp::child c = m_task->execute();
	m_task->set_process(&c);
	bp::posix_status s = c.wait();
	//auto exitStatus = bp::wait_for_exit(c);
	//int exitCode = BOOST_PROCESS_EXITSTATUS(exitStatus);
	//int* status = &exitCode;
	//m_status = *((int*)status);
	m_status = s.exit_status();

	return &m_status;
}

void MyWorkerThread::emit_event(int status, const wxString& msg) {
	wxThreadEvent event(wxEVT_THREAD, WORKER_EVENT);
	event.SetPayload(m_task);
	event.SetString(msg);
	event.SetInt(status);
	wxQueueEvent(m_frame, event.Clone());
}
