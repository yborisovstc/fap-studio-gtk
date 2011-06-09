
#ifndef __FAPSTU_GTK_APP_H
#define __FAPSTU_GTK_APP_H

#include <fapext.h>
#include "mainwnd.h"

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
	void OpenFile(const string& aFileName);
	void SaveAs(const string& aFileName);
	// From MsuMwndMenuObserver
	virtual void OnCmd(TCmd aCmd);
	virtual TBool OnCmdUpdateRequest(TCmd aCmd);
    private:
	static string FormTitle(const string& aFileName);
	static string GetDefaultLogFileName();
    private:
	CsuMainWnd* iMainWnd;
	CAE_Env* iCaeEnv;
	CagProvider* iCagProv;
	MAE_Opv* iViewProxy;
	string iLogFileName;
};

#endif 
