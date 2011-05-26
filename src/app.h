
#ifndef __FAPSTU_GTK_APP_H
#define __FAPSTU_GTK_APP_H

#include <fapext.h>
#include "mainwnd.h"

class CsuApp
{
    public:
	CsuApp();
	virtual ~CsuApp();
    private:
	CsuMainWnd* iMainWnd;
	CAE_Env* iCaeEnv;
};

#endif 
