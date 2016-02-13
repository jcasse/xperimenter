////////////////////////////////////////////////////////////////////////////////
// Name:        app.h
// Purpose:     Workflow Tree Management Application
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

#ifndef _APP_H_
#define _APP_H_

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
#include "wx/thread.h"

//#ifdef __GNUG__
//#pragma interface "app.cpp"
//#endif

class MyThread;

WX_DEFINE_ARRAY_PTR(wxThread*, wxArrayThread);

class MyApp: public wxApp {
public:
	MyApp();
	virtual ~MyApp() {}

	// Called on application startup
	virtual bool OnInit() override;

	// critical section protects access to all of the fields below
	wxCriticalSection m_critsect;

	// all the threads currently alive - as soon as the thread terminates, it's
	// removed from the array
	wxArrayThread m_threads;

	// semaphore used to wait for the threads to exit, see MyFrame::OnQuit()
	wxSemaphore m_semAllDone;

	// indicates that we're shutting down and all threads should exit
	bool m_shuttingDown;
};

DECLARE_APP(MyApp)

#endif
