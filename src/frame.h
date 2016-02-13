////////////////////////////////////////////////////////////////////////////////
// Name:        frame.h
// Purpose:     Declares main frame
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

#ifndef _FRAME_H_
#define _FRAME_H_

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
#include "wx/dynarray.h"
#include "wx/numdlg.h"
#include "wx/progdlg.h"

//#ifdef __GNUG__
//#pragma interface "frame.cpp"
//#endif

#include "app.h"
#include "project.h"
#include "workflowtreectrl.h"

const wxString VERSION("1.0");
const wxString NAME("Xperimenter");

class MyFrame: public wxFrame {
public:
	MyFrame(const wxString& title);
	virtual ~MyFrame();

	void CreateControls();
	void InitializeControls();
	void Init();

	// accessors for MyWorkerThread (called in its context!)
	bool Canceled();

protected:
	virtual void DoLogRecord(wxLogLevel level,
		const wxString& msg,
		const wxLogRecordInfo& info); //override;

private:
	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnNew(wxCommandEvent& event);
	void OnOpen(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnSaveAs(wxCommandEvent& event);
	void OnRenameTask(wxCommandEvent& event);
	void OnCopyTask(wxCommandEvent& event);
	void OnDeleteTask(wxCommandEvent& event);
	void OnAddTask(wxCommandEvent& event);
	void OnEditParams(wxCommandEvent& event);
	void OnViewParams(wxCommandEvent& event);
	void OnClearTask(wxCommandEvent& event);
	void OnExecuteTask(wxCommandEvent& event);
	void OnKillTask(wxCommandEvent& event);
	void OnExecuteProject(wxCommandEvent& event);
	void OnMenuOpen(wxMenuEvent& event);
	void OnWorkflowTreeChanged(wxCommandEvent& event);
	void OnWorkflowTreeEditingDone(wxCommandEvent& event);
	void OnWorkflowTreeContext(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);

	enum {ID_MAIN_FRAME = 100,
		ID_NEW,
		ID_OPEN,
		ID_MAIN_EXECUTE,
		ID_SAVE,
		ID_SAVEAS,
		ID_MAIN_WORKFLOW_TXT_CTRL
	};

	Project* project_;
	std::map<wxString, wxString> defaults_;

	wxMenu* fileMenu;
	wxMenu* editMenu;
	wxMenu* taskMenu;

	wxTextCtrl* projecttextctrl_;
	WorkflowTreeCtrl* workflowtreectrl_;
	wxCriticalSection m_tree;				// protects m_workflowtreectrl
	bool m_item_editing_done_hack;

	void set_menu_enable();
	bool set_pathname(const wxString& prompt);
	wxString get_dir(const wxString& prompt,
		const wxString& default_parent_dir, long style);
	wxString get_pathname(const wxString& prompt, long style);

	void load_defaults();
	void save_defaults(const wxString& name, const wxString& value);
	const wxString get_default(const wxString& name);
	void create_task(shptr<Task> task);

	void update_task_status(shptr<Task> task);
	void start_task(shptr<Task> task);
	void kill_task(shptr<Task> task);

	void OnShowCPUs(wxCommandEvent& event);
	void OnStartWorker(wxCommandEvent& event);
	void OnIdle(wxIdleEvent &event);
	void OnWorkerEvent(wxThreadEvent& event);
	void OnUpdateWorker(wxUpdateUIEvent& event);

	// logging helper
	void DoLogLine(wxTextCtrl *text,
	const wxString& timestr,
	const wxString& threadstr,
	const wxString& msg);

	// thread helper functions
	// -----------------------

	// helper function - creates a new thread (but doesn't run it)
	//MyThread *CreateThread();

	// update display in our status bar: called during idle handling
	void UpdateThreadStatus();

	// internal variables
	// ------------------

	// just some place to put our messages in
	wxTextCtrl *m_txtctrl;

	// old log target, we replace it with one using m_txtctrl during this
	// frame life time
	wxLog *m_oldLogger;

	// the array of pending messages to be displayed and the critical section
	// protecting it
	wxArrayString m_messages;
	wxCriticalSection m_csMessages;

	// remember the number of running threads and total number of threads
	size_t m_nRunning, m_nCount;

	// the progress dialog which we show while worker thread is running
	wxProgressDialog *m_dlgProgress;

	// was the worker thread canceled by user?
	bool m_canceled;
	wxCriticalSection m_csCanceled;        // protects m_canceled

	// This class handles events
	DECLARE_EVENT_TABLE()
};

#endif
