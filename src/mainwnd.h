
#ifndef __FAPSTU_GTK_MAINWND_H
#define __FAPSTU_GTK_MAINWND_H

#include "cagwindow.h"
#include <cagmenu.h>
#include <cagbox.h>
#include <caglayout.h>
#include <cagtoolbar.h>
#include <mainwndiface.h>

class CsuMainWndMenuFile: public CagMenu
{
    friend class CsuMainWndMenu;
    public: 
	CsuMainWndMenuFile();
    private:
	CagMenuItem* iMenuFileOpen;
	CagMenuItem* iMenuFileSaveAs;
};

class MsuMwndMenuObserver
{
    public:
	enum TCmd
	{
	    ECmd_FileOpen,
	    ECmd_FileSaveAs,
	};
    public:
	virtual void OnCmd(TCmd aCmd) = 0;
	virtual TBool OnCmdUpdateRequest(TCmd aCmd) = 0;
};

class CsuMainWndMenu: public CagMenuBar, public MagMenuShellObs
{
    public: 
	CsuMainWndMenu();
	void SetMenuObserver(MsuMwndMenuObserver* aObs); 
    private:
	// From MCagMenuItemObs
	virtual void OnActivated(CagMenuItem* aItem);
	// From MagMenuShellObs
	virtual void OnItemActivated(CagMenuShell* aMenuShell, CagMenuItem* aItem);
    private:
	CagMenuItem* iFileMenu;
	CsuMainWndMenuFile* iFileMenuPopup;
	MsuMwndMenuObserver* iMenuObs;
};

class CsuMainWndClient: public CagLayout
{
    public:
	CsuMainWndClient();
    private:
	virtual void OnExpose(GdkEventExpose* aEvent);
	virtual void OnSizeAllocate(GtkAllocation* aAllocation);
	virtual void OnSizeRequest(GtkRequisition* aRequisition);
};

class CsuMainWnd: public CagWindow, public MOpMainWnd
{
    public:
	static inline const char* Type() { return "CsuMainWnd";} ; 
	CsuMainWnd(const string& aName);
	virtual ~CsuMainWnd();
	void SetMenuObserver(MsuMwndMenuObserver* aObs);
	// From CAE_Base
	virtual void *DoGetObj(const char *aName);
    private:
	// From MOpMainWnd
	virtual CagContainer* ClientWnd();
	virtual CagToolBar* Toolbar();
    private:
	CagVBox* iVboxMain;
	CagVBox* iVboxMenuTbar;
	CsuMainWndMenu* iMenu;
	CagToolBar* iToolbar;
	CsuMainWndClient* iClientWnd;
};

#endif 
