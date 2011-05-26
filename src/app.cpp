#include "app.h"
#include <cagprov.h>

const char* KLogFileName = "fap-tias.log";

CsuApp::CsuApp()
{
    iMainWnd = new CsuMainWnd("MainWnd");
    iMainWnd->Show();
    iCaeEnv = CAE_Env::NewL(1, NULL, KLogFileName, 1);
    CagProvider* cagprov = new CagProvider(iMainWnd);
    MAE_Opv* viewproxy = cagprov->CreateViewProxy();
    iCaeEnv->Root()->SetBaseViewProxy(viewproxy, ETrue);
}

CsuApp::~CsuApp()
{
}

