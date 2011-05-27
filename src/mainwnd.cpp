
#include "mainwnd.h"

CsuMainWndMenu::CsuMainWndMenu(): CagMenuBar("Menu"), iMenuObs(NULL), iFileMenu(NULL),
    iFileMenuPopup(NULL)
{
    // Menu item "File"
    iFileMenu = new CagMenuItem("File", "File");
    Append(iFileMenu);
    iFileMenu->Show();
    iFileMenu->SetMenuItemObs(this);
    // Popup menu "File"
    iFileMenuPopup = new CsuMainWndMenuFile();
    iFileMenuPopup->Show();
    iFileMenuPopup->SetMenuShellObs(this);
}

void CsuMainWndMenu::SetMenuObserver(MsuMwndMenuObserver* aObs)
{
    _FAP_ASSERT(iMenuObs == NULL);
    iMenuObs = aObs;
}

void CsuMainWndMenu::OnActivated(CagMenuItem* aItem)
{
    if (aItem == iFileMenu) {
	iFileMenuPopup->Popup(1, gtk_get_current_event_time());
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

CsuMainWnd::CsuMainWnd(const string& aName): CagWindow(aName)
{
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
    // Client area
    iClientWnd = new CsuMainWndClient();
    iVboxMain->PackStart(iClientWnd, false, false, 2);
    iClientWnd->Show();
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
    iMenu->SetMenuObserver(aObs);
}

