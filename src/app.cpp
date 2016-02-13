////////////////////////////////////////////////////////////////////////////////
// Name:        app.cpp
// Purpose:     Defines Workflow Tree Management application class
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows License
////////////////////////////////////////////////////////////////////////////////

//#ifdef __GNUG__
//#pragma implementation "app.h"
//#endif

#include "app.h"
#include "frame.h"

// Give wxWidgets the means to create a MyApp object
IMPLEMENT_APP(MyApp)

MyApp::MyApp() {
	m_shuttingDown = false;
}

// "Main program" equivalent, creating windows and returning main app frame
bool MyApp::OnInit() {

	if (!wxApp::OnInit()) return false;

	// uncomment this to get some debugging messages from the trace code
	// on the console (or just set WXTRACE env variable to include "thread")
	wxLog::AddTraceMask("thread");

	// Create the main application window
	MyFrame *frame = new MyFrame(NAME);

	// Show it
	frame->Show(true);

	// Start the event loop
	return true;
}
