////////////////////////////////////////////////////////////////////////////////
// Name:        frame.cpp
// Purpose:     Defines the application main frame
// Author:      Juan Casse
// Created:     08/26/15
// Copyright:   (c) 2015, Juan Casse
// License:     wxWindows License
////////////////////////////////////////////////////////////////////////////////

//#ifdef __GNUG__
//#pragma implementation "frame.h"
//#endif

#include "frame.h"
#include "taskthread.h"
#include "newworkflowdialog.h"
#include "taskdialog.h"
#include "deletetaskdialog.h"
#include "paramsdialog.h"
#include "taskcontextmenu.h"
#include "utilities.h"

#include "wx/filepicker.h"
#include "wx/busyinfo.h"
#include "wx/statline.h"

#include "boost/filesystem.hpp"
#include "boost/thread.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

boost::mutex status_mutex;

const wxString filetype("XML files (*.xml)|*.xml");

// Event table for MyFrame
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
	EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
	EVT_MENU(ID_NEW, MyFrame::OnNew)
	EVT_MENU(ID_OPEN, MyFrame::OnOpen)
	EVT_MENU(ID_SAVE, MyFrame::OnSave)
	EVT_MENU(ID_SAVEAS, MyFrame::OnSaveAs)
	EVT_MENU(ID_TASK_RENAME, MyFrame::OnRenameTask)
	EVT_MENU(ID_TASK_COPY, MyFrame::OnCopyTask)
	EVT_MENU(ID_TASK_DELETE, MyFrame::OnDeleteTask)
	EVT_MENU(ID_TASK_ADD, MyFrame::OnAddTask)
	EVT_MENU(ID_TASK_EDIT, MyFrame::OnEditParams)
	EVT_MENU(ID_TASK_VIEW, MyFrame::OnViewParams)
	EVT_MENU(ID_TASK_CLEAR, MyFrame::OnClearTask)
	EVT_MENU(ID_TASK_EXECUTE, MyFrame::OnExecuteTask)
	EVT_MENU(ID_TASK_KILL, MyFrame::OnKillTask)
	EVT_MENU(ID_MAIN_EXECUTE, MyFrame::OnExecuteProject)
	EVT_MENU_OPEN(MyFrame::OnMenuOpen)
	EVT_COMMAND(ID_MAIN_TREE_CTRL, TREE_EVENT, MyFrame::OnWorkflowTreeChanged)
	EVT_COMMAND(ID_MAIN_TREE_CTRL, TREE_EDITING_DONE_EVENT, MyFrame::OnWorkflowTreeEditingDone)
	EVT_COMMAND(ID_MAIN_TREE_CTRL, TREE_CONTEXT_EVENT, MyFrame::OnWorkflowTreeContext)
	EVT_THREAD(WORKER_EVENT, MyFrame::OnWorkerEvent)
	EVT_IDLE(MyFrame::OnIdle)
	EVT_CLOSE(MyFrame::OnClose)
END_EVENT_TABLE()

#include "mondrian.xpm"

MyFrame::MyFrame(const wxString& title): wxFrame(NULL, wxID_ANY, title) {

	m_item_editing_done_hack = false;

	m_oldLogger = wxLog::GetActiveTarget();

	// Set the frame icon
	SetIcon(wxIcon(mondrian_xpm));

	// File menu
	fileMenu = new wxMenu;

	fileMenu->Append(ID_NEW, wxT("&New Project\tCtrl-N"),
			wxT("Start a new project."));
	fileMenu->Append(ID_OPEN, wxT("&Open Project\tCtrl-O"),
			wxT("Open an existing project."));
	//fileMenu->Append(ID_SAVE, wxT("&Save Project"),
	//		wxT("Save the current project."));
	fileMenu->Append(ID_SAVEAS, wxT("&Save Project As"),
			wxT("Copy the project."));
	fileMenu->AppendSeparator();
	fileMenu->Append(ID_MAIN_EXECUTE, wxT("&Execute Project\tCtrl-E"),
			wxT("Execute all pending tasks in project."));
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT, wxT("E&xit\tCtrl-X"),
			wxT("Quit " + NAME + '.'));

	// The "About" item should be in the help menu
	wxMenu* helpMenu = new wxMenu;

	helpMenu->Append(wxID_ABOUT, wxT("&About " + NAME),
			wxT("Show information about " + NAME + '.'));

	// Now append the freshly created menu to the menu bar...
	wxMenuBar* menuBar = new wxMenuBar();

	menuBar->Append(fileMenu, wxT("&File"));
	menuBar->Append(helpMenu, wxT("&Help"));

	// ... and attach this menu bar to the frame
	SetMenuBar(menuBar);

	CreateStatusBar();
	SetStatusText("Welcome to " + NAME + '.');

	m_nRunning = m_nCount = 0;

	m_dlgProgress = NULL;

	//wxLog::SetActiveTarget(this); // cannot compile

	CreateControls();
	InitializeControls();
	Init();

	//SetDialogHelp();
	//SetDialogValidators();

	// This fits the dialog to the minimum size dictated by the sizers
	GetSizer()->Fit(this);

	// This ensures that the dialog cannot be sized smaller
	// than the minimum size
	GetSizer()->SetSizeHints(this);

	// Center the dialog on the parent or (if none) screen
	Center();
}

MyFrame::~MyFrame() {
	wxLog::SetActiveTarget(m_oldLogger);

	// NB: although the OS will terminate all the threads anyhow when the main
	//     one exits, it's good practice to do it ourselves -- even if it's not
	//     completely trivial in this example

	// tell all the threads to terminate: note that they can't terminate while
	// we're deleting them because they will block in their OnExit() -- this is
	// important as otherwise we might access invalid array elements

	{
		wxCriticalSectionLocker locker(wxGetApp().m_critsect);

		// check if we have any threads running first
		const wxArrayThread& threads = wxGetApp().m_threads;
		size_t count = threads.GetCount();

		if (!count) return;

		// set the flag indicating that all threads should exit
		wxGetApp().m_shuttingDown = true;
	}

	// now wait for them to really terminate
	wxGetApp().m_semAllDone.Wait();
}

bool MyFrame::Canceled() {
	wxCriticalSectionLocker lock(m_csCanceled);
	return m_canceled;
}

void MyFrame::DoLogLine(wxTextCtrl *text,
		const wxString& timestr,
		const wxString& threadstr,
		const wxString& msg) {
	text->AppendText(wxString::Format("%9s %10s %s", timestr, threadstr, msg));
}

void MyFrame::DoLogRecord(wxLogLevel level,
		const wxString& msg,
		const wxLogRecordInfo& info) {
	// let the default GUI logger treat warnings and errors as they should be
	// more noticeable than just another line in the log window and also trace
	// messages as there may be too many of them
	if (level <= wxLOG_Warning || level == wxLOG_Trace) {
		m_oldLogger->LogRecord(level, msg, info);
		return;
	}

	DoLogLine(m_txtctrl, wxDateTime(info.timestamp).FormatISOTime(),
		info.threadId == wxThread::GetMainId() ? wxString("main")
		: wxString::Format("%lx", info.threadId), msg + "\n");
}

void MyFrame::CreateControls() {

	/// items

    wxStaticText* workflowtextlabel = new wxStaticText(this, wxID_STATIC,
			wxT("Project Location"),
			wxDefaultPosition, wxDefaultSize, 0);
    wxStaticText* workflowtreelabel = new wxStaticText(this, wxID_STATIC,
			wxT("Workflow Tree"),
			wxDefaultPosition, wxDefaultSize, 0);

	projecttextctrl_ = new wxTextCtrl(this, ID_MAIN_WORKFLOW_TXT_CTRL,
            "", wxDefaultPosition, wxSize(500, -1),
            wxTE_READONLY|wxTE_RICH|wxTE_DONTWRAP);
	projecttextctrl_->SetDefaultStyle(wxTextAttr(wxNullColour, *wxLIGHT_GREY));
	workflowtreectrl_ = new WorkflowTreeCtrl(this, ID_MAIN_TREE_CTRL,
			wxDefaultPosition, wxSize(500, 300),
			wxDV_NO_HEADER|wxDV_ROW_LINES);

	wxStaticLine* hline = new wxStaticLine(this, wxID_ANY, wxDefaultPosition);

	/// layout

	wxBoxSizer* ctrlbox = new wxBoxSizer(wxHORIZONTAL);

	ctrlbox->Add(projecttextctrl_, 1,
			wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT, 10);

	wxBoxSizer* topbox = new wxBoxSizer(wxVERTICAL);

	topbox->Add(workflowtextlabel, 0, wxALL|wxALIGN_LEFT, 10);
	topbox->Add(ctrlbox, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 0);
	topbox->Add(hline, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5);
	topbox->Add(workflowtreelabel, 0, wxALL|wxALIGN_LEFT, 10);
	topbox->Add(workflowtreectrl_, 1,
			wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 10);

	this->SetSizer(topbox);

	// bind the widgets to event handlers

	Center();
}

void MyFrame::InitializeControls() {
}

void MyFrame::set_menu_enable() {
	if (project_ && project_->get_root()) {
		fileMenu->Enable(ID_MAIN_EXECUTE, true);
		fileMenu->Enable(ID_SAVEAS, true);
	}
	else {
		fileMenu->Enable(ID_MAIN_EXECUTE, false);
		fileMenu->Enable(ID_SAVEAS, false);
	}
}

void MyFrame::Init() {
	project_ = NULL;
	load_defaults();
}

void MyFrame::OnAbout(wxCommandEvent& event) {
std::cerr << "OnAbout()" << std::endl;
	wxString msg;
	msg.Printf(NAME + ' ' + VERSION + ", built with %s", wxVERSION_STRING);
	wxMessageBox(msg, wxT("About this program"), wxOK|wxICON_INFORMATION, this);
}

void MyFrame::OnQuit(wxCommandEvent& event) {
std::cerr << "OnQuit()" << std::endl;
	Close();
}

void MyFrame::OnClose(wxCloseEvent& event) {
std::cerr << "OnClose()" << std::endl;
	if (!event.CanVeto()) event.Skip();
	// check if we have any threads running first
	wxCriticalSectionLocker locker(wxGetApp().m_critsect);
	const wxArrayThread& threads = wxGetApp().m_threads;
	size_t count = threads.GetCount();
	if (count) {
		wxMessageBox("Tasks are running. Please wait.", "Oops",
			wxOK|wxOK_DEFAULT|wxICON_INFORMATION|wxSTAY_ON_TOP);
		event.Veto();
	}
	else event.Skip();
}

void MyFrame::OnSave(wxCommandEvent&) {
std::cerr << "OnSave()" << std::endl;
	//if (project_) {if (project_->is_changed()) project_->save(); return;}
	//wxMessageBox("Nothing to save.", "Oops", wxICON_ERROR);
}

void MyFrame::OnSaveAs(wxCommandEvent& WXUNUSED(event)) {
std::cerr << "OnSaveAs()" << std::endl;
	const wxString name = "Parent Directory";
	const wxString parent_dir = get_default(name);
	NewWorkflowDialog dialog(this, parent_dir);
	if (!(dialog.ShowModal() == wxID_OK)) return;
	const wxString dir = dialog.GetWorkflowDir();
	save_defaults(name, ::get_parent(dir));
	projecttextctrl_->ChangeValue(dir);
	project_->set_dir(dir);
	project_->update_fixed_params();
	Project::save(*project_, project_->get_dir());
}

void MyFrame::OnNew(wxCommandEvent& event) {
std::cerr << "OnNew()" << std::endl;
	const wxString name = "Parent Directory";
	const wxString parent_dir = get_default(name);
	NewWorkflowDialog dialog(this, parent_dir);
	if (!(dialog.ShowModal() == wxID_OK)) return;
	const wxString dir = dialog.GetWorkflowDir();
	save_defaults(name, ::get_parent(dir));
	if (project_) delete project_;
	project_ = new Project(dir);
	projecttextctrl_->ChangeValue(dir);
	workflowtreectrl_->clear();
	workflowtreectrl_->set_project(project_);
	Project::save(*project_, project_->get_dir());
}

void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event)) {
std::cerr << "OnOpen()" << std::endl;
	const wxString name = "Parent Directory";
	const wxString parent_dir = get_default(name);
	const wxString msg = "Select Project";
	const wxString dir = get_dir(msg, parent_dir, wxDD_DIR_MUST_EXIST);
	if (dir.empty()) return;
	save_defaults(name, ::get_parent(dir));
	if (project_) delete project_;
	project_ = new Project();
	Project::load(*project_, dir);
	projecttextctrl_->ChangeValue(project_->get_dir());
	workflowtreectrl_->clear();
	workflowtreectrl_->build_tree(project_->get_root());
	workflowtreectrl_->set_project(project_);
}

void MyFrame::OnRenameTask(wxCommandEvent& event) {
std::cerr << "OnRenameTask()" << std::endl;
	if (!project_) return;
	wxTextEntryDialog dialog(this, wxT("Enter new task name:"));
	if (!(dialog.ShowModal() == wxID_OK)) return;
	const wxString name = dialog.GetValue();
	workflowtreectrl_->rename_task(name);
	Project::save(*project_, project_->get_dir());
}

void MyFrame::OnCopyTask(wxCommandEvent& event) {
std::cerr << "OnCopyTask()" << std::endl;
	if (!project_) return;
	wxTextEntryDialog dialog(this, wxT("Enter new task name:"));
	if (!(dialog.ShowModal() == wxID_OK)) return;
	const wxString name = dialog.GetValue();
	workflowtreectrl_->copy_task(name);
}

void MyFrame::OnDeleteTask(wxCommandEvent& event) {
std::cerr << "OnDeleteTask()" << std::endl;
	DeleteTaskDialog dialog(this, workflowtreectrl_->get_task());
	if (!(dialog.ShowModal() == wxID_OK)) return;
	workflowtreectrl_->delete_task();
}

void MyFrame::OnAddTask(wxCommandEvent& event) {
std::cerr << "OnAddTask()" << std::endl;
	if (!project_) return;
	wxTextEntryDialog dialog(this, wxT("Enter new task name:"));
	if (!(dialog.ShowModal() == wxID_OK)) return;
	const wxString name = dialog.GetValue();
	workflowtreectrl_->add_task(name);
}

void MyFrame::OnEditParams(wxCommandEvent& event) {
	shptr<Task> task = workflowtreectrl_->get_task();
	assert(task);
	const wxString default_dir = get_default("Params Directory");
	ParamsDialog dialog(this, task, default_dir);
	if (!(dialog.ShowModal() == wxID_OK)) return;
	const wxString dir = dialog.get_dir();
	save_defaults("Params Directory", dir);
	const Params params = dialog.get_params();
	task->set_params(params);
	task->set_fixed_params();
	//task->write_params();
	Project::save(*project_, project_->get_dir());
	update_task_status(task);
}

void MyFrame::OnViewParams(wxCommandEvent& event) {
std::cerr << "OnViewParams()" << std::endl;
	shptr<Task> task = workflowtreectrl_->get_task();
	assert(task);
	const wxString default_dir = get_default("Params Directory");
	ParamsDialog dialog(this, task, default_dir, true);
	dialog.ShowModal();
}

void MyFrame::OnWorkflowTreeChanged(wxCommandEvent& event) {
std::cerr << "OnWorkflowTreeChanged()" << std::endl;
	int id = event.GetId();
	project_->set_root(workflowtreectrl_->get_root());
	Project::save(*project_, project_->get_dir());
}

void MyFrame::OnWorkflowTreeEditingDone(wxCommandEvent& event) {
std::cerr << "OnWorkflowTreeEditingDone()" << std::endl;
	int id = event.GetId();
	m_item_editing_done_hack = true;
}

void MyFrame::OnWorkflowTreeContext(wxCommandEvent& event) {
std::cerr << "OnWorkflowTreeContext()" << std::endl;
	int id = event.GetId();
	if (!project_) return;
	shptr<Task> task = workflowtreectrl_->get_task();
	TaskContextMenu* menu = new TaskContextMenu(task);
	PopupMenu(menu);
}

void MyFrame::OnExecuteTask(wxCommandEvent& event) {
std::cerr << "OnExecuteTask()" << std::endl;
	shptr<Task> task = workflowtreectrl_->get_task();
	start_task(task);
}

void MyFrame::OnKillTask(wxCommandEvent& event) {
std::cerr << "OnKillTask()" << std::endl;
	shptr<Task> task = workflowtreectrl_->get_task();
	kill_task(task);
}

void MyFrame::OnExecuteProject(wxCommandEvent& event) {
std::cerr << "OnExecuteProject()" << std::endl;
	shptr<Task> task = workflowtreectrl_->get_root();
	start_task(task);
}

void MyFrame::OnMenuOpen(wxMenuEvent& event) {
std::cerr << "OnMenuOpen()" << std::endl;
	set_menu_enable();
}

void MyFrame::OnClearTask(wxCommandEvent& event) {
std::cerr << "OnClearTask()" << std::endl;
	shptr<Task> task = workflowtreectrl_->get_task();
	const wxString name = task->get_name();
	const wxString title = "Clear Task " + name;

	if (task->has_closed_children()) {
		const wxString msg = wxString("Cannot clear task's results.\n") +
			"Other tasks depend on it.";
		wxMessageBox(msg, title,
			wxOK|wxOK_DEFAULT|wxICON_ERROR|wxSTAY_ON_TOP);
		return;
	}

	const wxString msg = wxString("This action cannot be undone!\n") +
			"Do you want to continue?";
	if (wxMessageBox(msg, title,
			wxYES_NO|wxNO_DEFAULT|wxICON_WARNING|wxSTAY_ON_TOP) == wxYES) {
		::remove_dir(task->get_dir());
	}

	update_task_status(task);
}

void MyFrame::OnWorkerEvent(wxThreadEvent& event) {
std::cerr << "OnWorkerEvent()" << std::endl;
	shptr<Task> task = event.GetPayload<shptr<Task>>();
	if (task->is_done()) {
		for (shptr<Task> child : task->get_children()) start_task(child);
	}
	update_task_status(task);
}

void MyFrame::OnIdle(wxIdleEvent& event) {
//std::cerr << "OnIdle()" << std::endl;
	UpdateThreadStatus();
	// displays correct text after changing it through dataviewtreectrl
	if (m_item_editing_done_hack) {
		m_item_editing_done_hack = false;
		shptr<Task> task = workflowtreectrl_->get_task();
		if (task) update_task_status(task);
	}
	event.Skip();
}

void MyFrame::OnShowCPUs(wxCommandEvent&) {
std::cerr << "OnShowCPUs()" << std::endl;
	wxString msg;
	int nCPUs = wxThread::GetCPUCount();
	switch (nCPUs) {
	case -1: msg = wxT("Unknown number of CPUs"); break;
	case 0: msg = wxT("WARNING: you're running without any CPUs!"); break;
	case 1: msg = wxT("This system only has one CPU."); break;
	default: msg.Printf(wxT("This system has %d CPUs"), nCPUs);
	}
	wxLogMessage(msg);
}

//// helper functions

void MyFrame::start_task(shptr<Task> task) {
	//if (!task->is_ready()) return;
	MyWorkerThread* thread = new MyWorkerThread(this, task);
	if (thread->Create() != wxTHREAD_NO_ERROR) {
		wxLogError(wxT("Can't create thread!"));
		return;
	}
	create_task(task);
	wxCriticalSectionLocker enter(wxGetApp().m_critsect);
	wxGetApp().m_threads.Add(thread);
	// thread is not running yet, no need for crit sect
	m_canceled = false;
	thread->Run();
}

void MyFrame::kill_task(shptr<Task> task) {
	boost::process::child* child = task->get_process();
	if (child) child->terminate();
}

void MyFrame::create_task(shptr<Task> task) {
	namespace fs = boost::filesystem;
	fs::path path(task->get_dir().ToStdString());
	if (!fs::exists(path)) assert(fs::create_directory(path));
	task->write_params();
}

void MyFrame::UpdateThreadStatus() {
	wxCriticalSectionLocker enter(wxGetApp().m_critsect);

	// update the counts of running/total threads
	size_t nRunning = 0,
	nCount = wxGetApp().m_threads.Count();
	for (size_t n = 0; n < nCount; n++) {
		if (wxGetApp().m_threads[n]->IsRunning()) nRunning++;
	}

	if (nCount != m_nCount || nRunning != m_nRunning) {
		m_nRunning = nRunning;
		m_nCount = nCount;

		wxLogStatus(this, wxT("%u threads total, %u running."),
			unsigned(nCount), unsigned(nRunning));
	}
	//else: avoid flicker - don't print anything
}

void MyFrame::update_task_status(shptr<Task> task) {
	wxCriticalSectionLocker lock(m_tree);
	//boost::unique_lock<boost::mutex> scoped_lock(status_mutex);
	workflowtreectrl_->print_task_status(task);
}

bool MyFrame::set_pathname(const wxString& prompt) {
	const wxString pathname =
			get_pathname(prompt, wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	if (pathname.empty()) return false;
	return true;
}

wxString MyFrame::get_pathname(const wxString& prompt, long style) {
	wxFileDialog fileDialog(this, prompt, wxEmptyString, wxEmptyString,
			filetype, style);
	if (fileDialog.ShowModal() == wxID_CANCEL) return wxString();
	return fileDialog.GetPath();
}

wxString MyFrame::get_dir(const wxString& prompt,
		const wxString& default_parent_dir, long style) {
	wxDirDialog dialog(this, prompt, default_parent_dir, style);
	if (dialog.ShowModal() == wxID_CANCEL) return wxString();
	return dialog.GetPath();
}

const wxString MyFrame::get_default(const wxString& name) {
	std::map<wxString, wxString>::const_iterator it = defaults_.find(name);
	return it == defaults_.end() ? "" : it->second;
}

void MyFrame::load_defaults() {
	boost::filesystem::path path = boost::filesystem::current_path();
	wxString pathname = path.string() + "/.Xperimenter_rc";
	if (!boost::filesystem::exists(pathname.ToStdString())) return;
	std::fstream fs(pathname.mb_str(), std::fstream::in);
	std::string line;
	while (getline(fs, line)) {
		std::vector<std::string> fields = get_fields(line, ',');
		assert(fields.size() == 2);
		defaults_[fields[0]] = fields[1];
	}
}

void MyFrame::save_defaults(const wxString& name, const wxString& value) {
	defaults_[name] = value;
	boost::filesystem::path path = boost::filesystem::current_path();
	wxString pathname = path.string() + "/.Xperimenter_rc";
	std::fstream fs(pathname.mb_str(), std::fstream::out);
	for (const std::pair<wxString, wxString>& item : defaults_)
	fs << '\"' << item.first << "\",\"" << item.second << '\"' << std::endl;
	fs.close();
}

//void MyFrame::UpdateStatus() {
//	if (!project) return;
//	if (project->isOpen()) SetStatusText("Workflow status: Open");
//	else SetStatusText("Workflow status: Close");
//}
