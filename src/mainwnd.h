
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
	CagMenuItem* iMenuFileSave;
	CagMenuItem* iMenuFileSaveAs;
};

class MsuMwndMenuObserver
{
    public:
	enum TCmd
	{
	    ECmd_FileOpen,
	    ECmd_FileSave,
	    ECmd_FileSaveAs,
	    ECmd_Step,
	    ECmd_Run,
	    ECmd_Pause,
	    ECmd_Start,
	    ECmd_Stop,
	    ECmd_Close,
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

// Client window
class CsuMainWndClient: public CagScrolledWindow
{
    public:
	CsuMainWndClient();
};

// Log view
class CsuLogView: public CagTextView
{
    public:
	CsuLogView(const string& aName);
	virtual ~CsuLogView();
	void SetLogFileName(const string& aLogFileName);
    private:
	static gboolean handle_timer_event(gpointer data);
	static void handle_filemon_event(GFileMonitor *monitor, GFile *file, GFile *other_file, GFileMonitorEvent event_type, gpointer user_data);
	void OnFileChanged(GFileMonitorEvent event);
	void OnFileContentChanged();
	void OnFileCreated();
	void OnFileDeleted();
    private:
	string iLogFileName;
	GFile* iLogFile;
	GFileMonitor* iFileMon;
	GFileInputStream* iInpStream;
	GtkTextBuffer* iBuffer;
	char* iLogBuffer;
};

// Main window
class CsuMainWnd: public CagWindow, public MOpMainWnd, public MagToolButtonObserver
{
    public:
	static inline const char* Type() { return "CsuMainWnd";} ; 
	CsuMainWnd(const string& aName);
	virtual ~CsuMainWnd();
	void SetMenuObserver(MsuMwndMenuObserver* aObs);
	void SetEnvLog(const string& aLogFileName);
	CagWidget* GetEnviw();
	// From CagWidget
	virtual void OnDestroy();
	// From CAE_Base
	virtual void *DoGetObj(const char *aName);
	// From MagToolButtonObserver
	virtual void OnClicked(CagToolButton* aBtn);
    private:
	// From MOpMainWnd
	virtual CagScrolledWindow* ClientWnd();
	virtual CagToolBar* Toolbar();
    private:
	CagVBox* iVboxMain;
	CagVBox* iVboxMenuTbar;
	CsuMainWndMenu* iMenu;
	CagToolBar* iToolbar;
	CsuMainWndClient* iClientWnd;
	CagVPaned* iVpaned;
	CagHPaned* iPaned2;
	CsuLogView* iLogView;
	MsuMwndMenuObserver* iMenuObs;
	CagScrolledWindow* iLogWnd;
	CagAlignment* iEnviWnd;
};

#endif 
