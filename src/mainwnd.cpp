
#include "mainwnd.h"

const string KTbarIcon_Step = "/usr/share/fap-studio-gtk/icons/tbar_btn_step.png";


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
    if (aItem == iFileMenu) {
//	iFileMenuPopup->Popup(1, gtk_get_current_event_time());
    }
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
    // Item "Save as"
    iMenuFileSaveAs = new CagMenuItem("FileSaveAs", "Save as");    
    Append(iMenuFileSaveAs);
    iMenuFileSaveAs->Show();
}



CsuMainWndClient::CsuMainWndClient(): CagLayout("ClientWnd")
{
}

void CsuMainWndClient::OnExpose(GdkEventExpose* aEvent)
{
}

void CsuMainWndClient::OnSizeAllocate(GtkAllocation* aAlc)
{
    if (!iChilds.empty()) {
	CagWidget* child = iChilds.begin()->second;
	GtkRequisition child_req; child->SizeRequest(&child_req);
	GtkAllocation alc = (GtkAllocation) {0, 0, child_req.width, child_req.height};
	*aAlc = (GtkAllocation) {0, 0, child_req.width, child_req.height};
    }
}


void CsuMainWndClient::OnSizeRequest(GtkRequisition* aRequisition)
{
    // Calculate child size
    if (!iChilds.empty()) {
	CagWidget* child = iChilds.begin()->second;
	GtkRequisition child_req; child->SizeRequest(&child_req);
	*aRequisition = child_req;
    }
}


// Log view


const TInt KLogViewBufLen = 1000;

CsuLogView::CsuLogView(const string& aName): 
    CagTextView(aName), iLogFile(NULL), iFileMon(NULL), iInpStream(NULL)
{
    SetEditable(EFalse);
    // Buffer
    iBuffer = gtk_text_buffer_new(NULL);
    SetBuffer(iBuffer);
    // Timer of monitoring of log file
//    g_timeout_add(KFapeTimeSlice, idle_event_handler, NULL);
}

CsuLogView::~CsuLogView()
{
    //fclose(iLogFile);
}

void CsuLogView::SetLogFileName(const string& aLogFileName)
{
    iLogFileName = aLogFileName;
    // Open log
//    iLogFile = fopen(iLogFileName.c_str(), "r");
    iLogFile = g_file_new_for_path(iLogFileName.c_str());
    // Create monitor
    GFileMonitor* iFileMon = g_file_monitor_file(iLogFile, G_FILE_MONITOR_NONE, NULL, NULL);
    g_signal_connect(G_OBJECT(iFileMon), "changed", G_CALLBACK(handle_filemon_event), this);
    // Open file for reading
    GError* err;
    iInpStream = g_file_read(iLogFile, NULL, &err);
    // char* buf = malloc(500);
    //gssize size = g_input_stream_read(iInpStream, buf, 500, NULL, err);
//    gboolean res = g_input_stream_read_all(iInpStream, void *buffer, gsize count, gsize *bytes_read, GCancellable *cancellable, GError **error);
}

void CsuLogView::OnFileContentChanged()
{
    // Get the new content from file 
    char* buf = (char*) g_malloc(KLogViewBufLen);
    GError* err;
    gssize size = g_input_stream_read(G_INPUT_STREAM(iInpStream), buf, KLogViewBufLen, NULL, &err);
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(iBuffer, &iter);
    gtk_text_buffer_insert(iBuffer, &iter, (const gchar*) buf, size);
    gtk_text_buffer_get_end_iter(iBuffer, &iter);
    while(gtk_events_pending()) gtk_main_iteration();
    gboolean res = ScrollToIter(&iter, 0.0, true, 0.0, 1.0);
    g_free(buf);
}

void CsuLogView::OnFileChanged(GFileMonitorEvent event)
{
    if (event == G_FILE_MONITOR_EVENT_CHANGED) {
	OnFileContentChanged();
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
    // Button "Step"
    CagToolButton* sBtnStep = new CagToolButton("BtnStep");
    sBtnStep->SetImage(KTbarIcon_Step);
    sBtnStep->SetObserver(this);
    sBtnStep->Show();
    iToolbar->Insert(sBtnStep, 0);
    
    // V-paned
    iVpaned = new CagVPaned("VPaned");
    //gtk_container_set_resize_mode(GTK_CONTAINER(iVpaned->iWidget), GTK_RESIZE_QUEUE);
    //gtk_container_set_resize_mode(GTK_CONTAINER(iVpaned->iWidget), GTK_RESIZE_PARENT);
    iVboxMain->PackStart(iVpaned, false, false, 2);
    iVpaned->Show();
    // Client area
    iClientWnd = new CsuMainWndClient();
    iVpaned->Add1(iClientWnd);
    iClientWnd->Show();
    // Log View
    iLogView = new CsuLogView("Logview");
    // TODO YB Hack
    iLogView->SetSizeRequest(200, 50);
    iVpaned->Add2(iLogView);
    iLogView->Show();
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

CagContainer* CsuMainWnd::ClientWnd()
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
}

void CsuMainWnd::OnDestroy()
{
    if (iMenuObs != NULL) {
	iMenuObs->OnCmd(MsuMwndMenuObserver::ECmd_Close);
    }
}


