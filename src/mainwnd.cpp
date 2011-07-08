
#include "mainwnd.h"

const string KTbarIcon_Step = "/usr/share/fap-studio-gtk/icons/tbar_btn_step.png";
const string KTbarIcon_Run = "/usr/share/fap-studio-gtk/icons/tbar_btn_run.png";
const string KTbarIcon_Pause = "/usr/share/fap-studio-gtk/icons/tbar_btn_pause.png";
const string KTbarIcon_Stop = "/usr/share/fap-studio-gtk/icons/tbar_btn_stop.png";
const string KTbarIcon_Start = "/usr/share/fap-studio-gtk/icons/tbar_btn_start.png";

const TInt KLogViewDefHeight = 50;
const TInt KLogViewDefWidth = 250;


CsuMainWndMenu::CsuMainWndMenu(): CagMenuBar("Menu"), iMenuObs(NULL), iFileMenu(NULL),
    iFileMenuPopup(NULL)
{
    // Menu item "File"
    iFileMenu = new CagMenuItem("File", "File");
    Append(iFileMenu);
    iFileMenu->Show();
//    iFileMenu->SetMenuItemObs(this);
    // Popup menu "File"
    iFileMenuPopup = new CsuMainWndMenuFile();
    iFileMenuPopup->Show();
    iFileMenuPopup->SetMenuShellObs(this);
    iFileMenu->SetSubmenu(iFileMenuPopup);
}

void CsuMainWndMenu::SetMenuObserver(MsuMwndMenuObserver* aObs)
{
    _FAP_ASSERT(iMenuObs == NULL);
    iMenuObs = aObs;
}

void CsuMainWndMenu::OnActivated(CagMenuItem* aItem)
{
    iFileMenuPopup->iMenuFileSave->SetSensitive(iMenuObs->OnCmdUpdateRequest(MsuMwndMenuObserver::ECmd_FileSave));
}

void CsuMainWndMenu::OnItemActivated(CagMenuShell* aMenuShell, CagMenuItem* aItem)
{
    MsuMwndMenuObserver::TCmd cmd = MsuMwndMenuObserver::ECmd_FileOpen;
    if (aMenuShell == iFileMenuPopup) {
	if (aItem == iFileMenuPopup->iMenuFileOpen) {
	    cmd = MsuMwndMenuObserver::ECmd_FileOpen;
	}
	else if (aItem == iFileMenuPopup->iMenuFileSaveAs) {
	    cmd = MsuMwndMenuObserver::ECmd_FileSaveAs;
	}
	else if (aItem == iFileMenuPopup->iMenuFileSave) {
	    cmd = MsuMwndMenuObserver::ECmd_FileSave;
	}
    }
    if (iMenuObs != NULL) {
	iMenuObs->OnCmd(cmd);
    }
}


// Popup menu "File"

CsuMainWndMenuFile::CsuMainWndMenuFile(): CagMenu("FileMenu"), iMenuFileOpen(NULL)
{
    // Item "Open File"
    iMenuFileOpen = new CagMenuItem("FileOpen", "Open");    
    Append(iMenuFileOpen);
    iMenuFileOpen->Show();
    // Item "Save"
    iMenuFileSave = new CagMenuItem("FileSave", "Save");    
    Append(iMenuFileSave);
    iMenuFileSave->Show();
    // Item "Save as"
    iMenuFileSaveAs = new CagMenuItem("FileSaveAs", "Save as");    
    Append(iMenuFileSaveAs);
    iMenuFileSaveAs->Show();
}


// Client window

CsuMainWndClient::CsuMainWndClient(): CagScrolledWindow("ClientWnd")
{
}

// Log view


const TInt KLogViewBufLen = 4000;

CsuLogView::CsuLogView(const string& aName): 
    CagTextView(aName), iLogFile(NULL), iFileMon(NULL), iInpStream(NULL), iLogBuffer(NULL)
{
    SetEditable(EFalse);
    // Buffer
    iBuffer = gtk_text_buffer_new(NULL);
    SetBuffer(iBuffer);
    iLogBuffer = (char*) g_malloc(KLogViewBufLen);
}

CsuLogView::~CsuLogView()
{
    //fclose(iLogFile);
    g_free(iLogBuffer);
}

void CsuLogView::SetLogFileName(const string& aLogFileName)
{
    iLogFileName = aLogFileName;
    // Open log
    iLogFile = g_file_new_for_path(iLogFileName.c_str());
    // Create monitor
    GFileMonitor* iFileMon = g_file_monitor_file(iLogFile, G_FILE_MONITOR_NONE, NULL, NULL);
    g_signal_connect(G_OBJECT(iFileMon), "changed", G_CALLBACK(handle_filemon_event), this);
    // Open file for reading
    GError* err;
    iInpStream = g_file_read(iLogFile, NULL, &err);
}

void CsuLogView::OnFileContentChanged()
{
    // Get the new content from file 
    iLogBuffer[0] = 0x0;
    GError* err;
    gssize size = g_input_stream_read(G_INPUT_STREAM(iInpStream), iLogBuffer, KLogViewBufLen, NULL, &err);
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(iBuffer, &iter);
    gtk_text_buffer_insert(iBuffer, &iter, (const gchar*) iLogBuffer, size);
    gtk_text_buffer_get_end_iter(iBuffer, &iter);
    // TODO YB hack
    while(gtk_events_pending()) gtk_main_iteration();
    gboolean res = ScrollToIter(&iter, 0.0, true, 0.0, 1.0);
}

void CsuLogView::OnFileCreated()
{
    // Open file for reading
    GError* err;
    iInpStream = g_file_read(iLogFile, NULL, &err);
}

void CsuLogView::OnFileDeleted()
{
    g_object_unref(G_OBJECT(iInpStream));
    iInpStream = NULL;
    // Clean buffer
    GtkTextIter begit, endit;
    gtk_text_buffer_get_start_iter(iBuffer, &begit);
    gtk_text_buffer_get_end_iter(iBuffer, &endit);
    gtk_text_buffer_delete(iBuffer, &begit, &endit);
}

void CsuLogView::OnFileChanged(GFileMonitorEvent event)
{
    if (event == G_FILE_MONITOR_EVENT_CHANGED) {
	OnFileContentChanged();
    }
    else if (event == G_FILE_MONITOR_EVENT_DELETED) {
	OnFileDeleted();
    }
    else if (event == G_FILE_MONITOR_EVENT_CREATED) {
	OnFileCreated();
    }
}

gboolean CsuLogView::handle_timer_event(gpointer data)
{
}

void CsuLogView::handle_filemon_event(GFileMonitor *monitor, GFile *file, GFile *other_file, GFileMonitorEvent event_type, gpointer user_data)
{
    CsuLogView* self = (CsuLogView*) user_data;
    self->OnFileChanged(event_type);
}


// Main window
//

CsuMainWnd::CsuMainWnd(const string& aName): CagWindow(aName), iMenuObs(NULL)
{
    // Base settings
    GdkGeometry hints;
//    gtk_window_set_geometry_hints(GTK_WINDOW(iWidget), GTK_WIDGET(iWidget), &hints, (GdkWindowHints)(GDK_HINT_USER_POS | GDK_HINT_USER_SIZE));
    // Vertical layot - main
    iVboxMain = new CagVBox("VboxMain");
    Add(iVboxMain);
    iVboxMain->Show();
    // Vertical layot - toolbar - view
    iVboxMenuTbar = new CagVBox("VboxMenuTbar");
    iVboxMain->PackStart(iVboxMenuTbar, false, false, 1);
    iVboxMenuTbar->Show();
    // Menu
    iMenu = new CsuMainWndMenu();
    iVboxMenuTbar->PackStart(iMenu, false, false, 1);
    iMenu->Show();
    
    // Toolbar
    iToolbar = new CagToolBar("Toolbar");
    iVboxMenuTbar->PackStart(iToolbar, false, false, 1);
    iToolbar->Show();
     // Button "Run"
    CagToolButton* sBtnRun = new CagToolButton("BtnRun");
    sBtnRun->SetImage(KTbarIcon_Run);
    sBtnRun->SetObserver(this);
    sBtnRun->Show();
    iToolbar->Insert(sBtnRun, 0);
     // Button "Pause"
    CagToolButton* sBtnPause = new CagToolButton("BtnPause");
    sBtnPause->SetImage(KTbarIcon_Pause);
    sBtnPause->SetObserver(this);
    sBtnPause->Show();
    iToolbar->Insert(sBtnPause, 0);
    // Button "Step"
    CagToolButton* sBtnStep = new CagToolButton("BtnStep");
    sBtnStep->SetImage(KTbarIcon_Step);
    sBtnStep->SetObserver(this);
    sBtnStep->Show();
    iToolbar->Insert(sBtnStep, 0);
    // Button "Start"
    CagToolButton* sBtnStart = new CagToolButton("BtnStart");
    sBtnStart->SetImage(KTbarIcon_Start);
    sBtnStart->SetObserver(this);
    sBtnStart->Show();
    iToolbar->Insert(sBtnStart, 0);
    // Button "Stop"
    CagToolButton* sBtnStop = new CagToolButton("BtnStop");
    sBtnStop->SetImage(KTbarIcon_Stop);
    sBtnStop->SetObserver(this);
    sBtnStop->Show();
    iToolbar->Insert(sBtnStop, 0);
    // Separator
    iToolbar->InsertSeparator(0, ETrue);
    
    // V-paned
    iVpaned = new CagVPaned("VPaned");
    iVboxMain->PackStart(iVpaned, true, true, 2);
    iVpaned->Show();
    // Paned 2
    iPaned2 = new CagHPaned("Paned2");
    iVpaned->Add2(iPaned2);
    iPaned2->Show();
    // Client area
    iClientWnd = new CsuMainWndClient();
    iVpaned->Add1(iClientWnd);
    iClientWnd->Show();

    // Log View
    iLogWnd = new CagScrolledWindow("LogWnd");
    iLogView = new CsuLogView("Logview");
    // TODO YB Hack
    iLogView->SetSizeRequest(KLogViewDefWidth, KLogViewDefHeight);
    iLogWnd->Add(iLogView);
    iLogWnd->Show();
//    iVpaned->Pack2(iLogWnd, false, false);
    iPaned2->Pack1(iLogWnd, false, false);
    iLogView->Show();
    // Environment visualization window
    iEnviWnd = new CagAlignment("Envi", 1.0, 1.0, 1.0, 1.0);
    iPaned2->Pack2(iEnviWnd, true, true);
    iEnviWnd->Show();
}

CsuMainWnd::~CsuMainWnd()
{
}


void* CsuMainWnd::DoGetObj(const char *aName)
{
    if (strcmp(aName, Type()) == 0) 
	return this;
    else if (strcmp(aName, MOpMainWnd::Type()) == 0)
	return (MOpMainWnd*) this;
    else if (strcmp(aName, MagToolButtonObserver::Type()) == 0)
	return (MagToolButtonObserver*) this;
    else return CagWindow::DoGetObj(aName);
}

CagScrolledWindow* CsuMainWnd::ClientWnd()
{
    return iClientWnd;
}

CagToolBar* CsuMainWnd::Toolbar()
{
    return iToolbar;
}

void CsuMainWnd::SetMenuObserver(MsuMwndMenuObserver* aObs)
{
    _FAP_ASSERT(iMenuObs == NULL);
    iMenuObs = aObs;
    iMenu->SetMenuObserver(aObs);
}

void CsuMainWnd::SetEnvLog(const string& aLogFileName)
{
    iLogView->SetLogFileName(aLogFileName);
}

void CsuMainWnd::OnClicked(CagToolButton* aBtn)
{
    if (aBtn == iToolbar->Child("BtnStep")) {
	if (iMenuObs != NULL) {
	    iMenuObs->OnCmd(MsuMwndMenuObserver::ECmd_Step);
	}
    }
    else if (aBtn == iToolbar->Child("BtnRun")) {
	if (iMenuObs != NULL) {
	    iMenuObs->OnCmd(MsuMwndMenuObserver::ECmd_Run);
	}
    }
    else if (aBtn == iToolbar->Child("BtnPause")) {
	if (iMenuObs != NULL) {
	    iMenuObs->OnCmd(MsuMwndMenuObserver::ECmd_Pause);
	}
    }
    else if (aBtn == iToolbar->Child("BtnStart")) {
	if (iMenuObs != NULL) {
	    iMenuObs->OnCmd(MsuMwndMenuObserver::ECmd_Start);
	}
    }
    else if (aBtn == iToolbar->Child("BtnStop")) {
	if (iMenuObs != NULL) {
	    iMenuObs->OnCmd(MsuMwndMenuObserver::ECmd_Stop);
	}
    }
}

void CsuMainWnd::OnDestroy()
{
    if (iMenuObs != NULL) {
	iMenuObs->OnCmd(MsuMwndMenuObserver::ECmd_Close);
    }
}


CagWidget* CsuMainWnd::GetEnviw()
{
    return iEnviWnd;
}
