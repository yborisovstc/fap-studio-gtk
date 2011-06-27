
#ifndef __FAPSTU_GTK_APP_H
#define __FAPSTU_GTK_APP_H

#include <fapext.h>
#include "mainwnd.h"

class CsuVisAdp: public CAE_Object
{
public:
    CsuVisAdp(const string&  aInstName, CAE_Env* aEnv);
    void SetWnd(CagWidget* aWnd);
    TBool ConnectWnd(CAE_ConnPointBase* aCp);
    void DisconnectWnd();
private:
    CAE_TState<TUint32>*   iWnd;
};


class CagProvider;
class CsuApp: public MsuMwndMenuObserver
{
    public:
	CsuApp();
	virtual ~CsuApp();
    private:
	void OnCmdOpenFile();
	void OnCmdFileSaveAs();
	void OnCmdStep();
	void OnCmdRun();
	void OnCmdPause();
	void OpenFile(const string& aFileName);
	void SaveAs(const string& aFileName);
	// From MsuMwndMenuObserver
	virtual void OnCmd(TCmd aCmd);
	virtual TBool OnCmdUpdateRequest(TCmd aCmd);
    private:
	static string FormTitle(const string& aFileName);
	static string GetDefaultLogFileName();
	gboolean static HandleTimerEvent(gpointer data);
	gboolean OnTimerEvent();
    private:
	CsuMainWnd* iMainWnd;
	CAE_Env* iCaeEnv;
	CagProvider* iCagProv;
	MAE_Opv* iViewProxy;
	string iLogFileName;
	TBool iRun;
	guint iTickToId;
	CsuVisAdp* iVisAdp;
};

#endif 
