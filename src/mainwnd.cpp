
#include "mainwnd.h"

CsuMainWndMenu::CsuMainWndMenu(): CagMenuBar("Menu")
{
    iFileMenu = new CagMenuItem("File", "File");
    Append(iFileMenu);
    iFileMenu->Show();
    iFileMenu->SetMenuItemObs(this);
    iFileMenuPopup = new CsuMainWndMenuFile();
    iFileMenuPopup->Show();
}

void CsuMainWndMenu::OnActivated(CagMenuItem* aItem)
{
    if (aItem == iFileMenu) {
	iFileMenuPopup->Popup(1, gtk_get_current_event_time());
    }
}

CsuMainWndMenuFile::CsuMainWndMenuFile(): CagMenu("FileMenu")
{
    iMenuFileOpen = new CagMenuItem("FileOpen", "Open");    
    Append(iMenuFileOpen);
    iMenuFileOpen->Show();
}

CsuMainWndClient::CsuMainWndClient(): CagLayout("ClientWnd")
{
}

void CsuMainWndClient::OnExpose(GdkEventExpose* aEvent)
{
}

void CsuMainWndClient::OnSizeAllocate(GtkAllocation* aAlc)
{
    CagWidget* child = iChilds.begin()->second;
    if (child) {
	GtkRequisition child_req; child->SizeRequest(&child_req);
	*aAlc = (GtkAllocation) {0, 0, child_req.width, child_req.height};
    }
}


void CsuMainWndClient::OnSizeRequest(GtkRequisition* aRequisition)
{
    // Calculate child size
    CagWidget* child = iChilds.begin()->second;
    if (child) {
	GtkRequisition child_req; child->SizeRequest(&child_req);
	*aRequisition = child_req;
    }
}

CsuMainWnd::CsuMainWnd(const string& aName): CagWindow(aName)
{
    // Vertical layot - toolbar - view
    iVbox = new CagVBox("Vbox");
    Add(iVbox);
    iVbox->Show();
    iMenu = new CsuMainWndMenu();
    iVbox->PackStart(iMenu, false, false, 1);
    iMenu->Show();
    iClientWnd = new CsuMainWndClient();
    iVbox->PackStart(iClientWnd, false, false, 2);
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

