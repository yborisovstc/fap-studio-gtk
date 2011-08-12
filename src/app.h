
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
private:
    CAE_TState<TUint32>*   iWnd;
};


class CsuApp;
class CsuSystObserver: public CAE_Base, public MAE_EbaseObserver
{
    public:
	CsuSystObserver(CsuApp& aOwner): iOwner(aOwner) {};
	// From MAE_EbaseObserver
       	virtual void OnActivated(CAE_EBase* aObj);
	// From CAE_EBase
	virtual void *DoGetFbObj(const char *aName);
    private:
	CsuApp& iOwner;
};

class CagProvider;
class CsuApp: public MsuMwndMenuObserver
{
    public:
	friend class CsuSystObserver;
    public:
	CsuApp();
	virtual ~CsuApp();
    private:
	void OnCmdOpenFile();
	void OnCmdFileSaveAs();
	void OnCmdFileSave();
	void OnCmdStep();
	void OnCmdRun();
	void OnCmdPause();
	void OnCmdStart();
	void OnCmdStop();
	void OpenFile(const string& aFileName, TBool aAsTmp = EFalse);
	void SaveAs(const string& aFileName);
	void SaveTmp();
	// From MsuMwndMenuObserver
	virtual void OnCmd(TCmd aCmd);
	virtual TBool OnCmdUpdateRequest(TCmd aCmd);
    private:
	static string FormTitle(const string& aFileName);
	static string GetDefaultLogFileName();
	static string GetDefaultTmpFileName();
	gboolean static HandleTimerEvent(gpointer data);
	gboolean OnTimerEvent();
	TBool IsSpecFileWrittable();
    private:
	CsuMainWnd* iMainWnd;
	CAE_Env* iCaeEnv;
	CagProvider* iCagProv;
	MAE_Opv* iViewProxy;
	string iLogFileName;
	TBool iRun;
	guint iTickToId;
	CsuVisAdp* iVisAdp;
	TBool iSaved;
	TBool iIsTempl; // System is created from template spec
	string iSpecFileName;
	TBool iStopped;
	TBool iChanged;
	CsuSystObserver iSystObs;
};

#endif 
