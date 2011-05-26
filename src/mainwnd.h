
#ifndef __FAPSTU_GTK_MAINWND_H
#define __FAPSTU_GTK_MAINWND_H

#include "cagwindow.h"
#include <cagmenu.h>
#include <cagbox.h>
#include <caglayout.h>
#include <mainwndiface.h>

class CsuMainWndMenuFile: public CagMenu
{
    public: 
	CsuMainWndMenuFile();
    private:
	CagMenuItem* iMenuFileOpen;
};

class CsuMainWndMenu: public CagMenuBar
{
    public: 
	CsuMainWndMenu();
    private:
	// From MCagMenuItemObs
	virtual void OnActivated(CagMenuItem* aItem);
    private:
	CagMenuItem* iFileMenu;
	CsuMainWndMenuFile* iFileMenuPopup;
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
	// From CAE_Base
	virtual void *DoGetObj(const char *aName);
    private:
	// From MOpMainWnd
	virtual CagContainer* ClientWnd();
    private:
	CagVBox* iVbox;
	CsuMainWndMenu* iMenu;
	CsuMainWndClient* iClientWnd;
};

#endif 
