
#ifndef __FAPSTU_GTK_MAINWND_H
#define __FAPSTU_GTK_MAINWND_H

#include "cagwindow.h"
#include <cagmenu.h>
#include <cagbox.h>
#include <caglayout.h>
#include <cagtoolbar.h>
#include <cagpaned.h>
#include <cagtextview.h>
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
	    ECmd_Step,
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

class CsuLogView: public CagTextView
{
    public:
	CsuLogView(const string& aName);
	void SetLogFileName(const string& aLogFileName);
    private:
	string iLogFileName;
};

class CsuMainWnd: public CagWindow, public MOpMainWnd, public MagToolButtonObserver
{
    public:
	static inline const char* Type() { return "CsuMainWnd";} ; 
	CsuMainWnd(const string& aName);
	virtual ~CsuMainWnd();
	void SetMenuObserver(MsuMwndMenuObserver* aObs);
	void SetEnvLog(const string& aLogFileName);
	// From CAE_Base
	virtual void *DoGetObj(const char *aName);
	// From MagToolButtonObserver
	virtual void OnClicked(CagToolButton* aBtn);
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
	CagVPaned* iVpaned;
	CsuLogView* iLogView;
	MsuMwndMenuObserver* iMenuObs;
};

#endif 
