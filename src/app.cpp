#include "app.h"
#include <cagprov.h>
#include <widgets.h>

const char* KLogFileName = "fap-studio.log";
const char* KTmpFileName = ".fap-studio-tmp.xml";
const char* KSpecFileName = "/usr/share/fap-studio-gtk/templ/empty.xml";
const char* KAppName = "fap-studio";

/* Time slice of FAP environment, in milliseconds */
const gint KFapeTimeSlice = 50;

CsuVisAdp::CsuVisAdp(const string&  aInstName, CAE_Env* aEnv): CAE_Object(aInstName.c_str(), NULL, aEnv)
{
    iWnd = CAE_TState<TUint32>::NewL("Viswinadp", this, TTransInfo());
}

void CsuVisAdp::SetWnd(CagWidget* aWnd)
{
    (*iWnd) = (TUint32) aWnd->iWidget;
    iWnd->Confirm();
}

TBool CsuVisAdp::ConnectWnd(CAE_ConnPointBase* aCp)
{
    aCp->Connect(iWnd->Output());
    iWnd->Output()->Connect(aCp);
}

void CsuVisAdp::DisconnectWnd()
{
    iWnd->Output()->Disconnect();
}

CsuApp::CsuApp(): iRun(EFalse)
{
    // Default logfilename
    iLogFileName = GetDefaultLogFileName();
    // Create Main window
    iMainWnd = new CsuMainWnd("MainWnd");
    iMainWnd->Show();
    iMainWnd->SetMenuObserver(this);
    // Create model
    iCaeEnv = CAE_Env::NewL(NULL, NULL, KSpecFileName, 1, NULL, iLogFileName.c_str());
    // Create view
    iCagProv = new CagProvider(iMainWnd);
    iViewProxy = iCagProv->CreateViewProxy();
    iCaeEnv->ConstructSystem();
    iCaeEnv->Root()->SetBaseViewProxy(iViewProxy, ETrue);

    iMainWnd->SetEnvLog(iLogFileName);
    iMainWnd->SetTitle(FormTitle(KSpecFileName));
    iMainWnd->Maximize();

    // Visualization window adapter
    iVisAdp = new CsuVisAdp("VisAdp", iCaeEnv);
    CAE_ConnPointBase* c_adinp = iCaeEnv->Root()->GetInpN("viswin");
    if (c_adinp != NULL) {
	iVisAdp->ConnectWnd(c_adinp);
    }
    iVisAdp->SetWnd(iMainWnd->GetEnviw());
}

string CsuApp::FormTitle(const string& aFilePath)
{
    size_t pos = aFilePath.find_last_of("/");
    string filename = pos != string::npos ? aFilePath.substr(pos + 1) : aFilePath;
    return string(KAppName) + " [" + filename + "]";
}

CsuApp::~CsuApp()
{
}

gboolean CsuApp::HandleTimerEvent(gpointer data)
{
    CsuApp* self = (CsuApp*) data;
    return self->OnTimerEvent();
}

gboolean CsuApp::OnTimerEvent()
{
    if (iRun) {
	iCaeEnv->Step();
    }
    return iRun;
}

string CsuApp::GetDefaultLogFileName()
{
    const gchar* home = g_getenv("HOME");
    return string(home) + "/" + KLogFileName;
}

string CsuApp::GetDefaultTmpFileName()
{
    const gchar* home = g_getenv("HOME");
    return string(home) + "/" + KTmpFileName;
}

void CsuApp::OnCmd(TCmd aCmd)
{
    if (aCmd == ECmd_FileOpen) {
	OnCmdOpenFile();
    }
    else if (aCmd == ECmd_FileSaveAs) {
	OnCmdFileSaveAs();
    }
    else if (aCmd == ECmd_Step) {
	OnCmdStep();
    }
    else if (aCmd == ECmd_Close) {
	gtk_main_quit ();
    }
    else if (aCmd == ECmd_Run) {
	OnCmdRun();
    }
    else if (aCmd == ECmd_Pause) {
	OnCmdPause();
    }
    else if (aCmd == ECmd_Start) {
	OnCmdStart();
    }
    else if (aCmd == ECmd_Stop) {
	OnCmdStop();
    }
}

TBool CsuApp::OnCmdUpdateRequest(TCmd aCmd)
{
}

void CsuApp::OnCmdOpenFile()
{
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW(iMainWnd->iWidget), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, 
	    GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
	char *filename;
	filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
	OpenFile(filename);
	iSaved = EFalse;
	g_free (filename);
    }
    gtk_widget_destroy (dialog);
}

void CsuApp::OnCmdStep()
{
    if (!iSaved) {
	SaveAs(GetDefaultTmpFileName());
	iSaved = ETrue;
    };
    iCaeEnv->Step();
}

void CsuApp::OnCmdRun()
{
    if (!iSaved) {
	SaveAs(GetDefaultTmpFileName());
	iSaved = ETrue;
    };
    iTickToId = g_timeout_add(KFapeTimeSlice, HandleTimerEvent, this);
    iRun = ETrue;
}

void CsuApp::OnCmdPause()
{
    iRun = EFalse;
    g_source_remove(iTickToId);
}

void CsuApp::OnCmdStart()
{
    OnCmdStop();
    OnCmdRun();
}

void CsuApp::OnCmdStop()
{
    iRun = EFalse;
    g_source_remove(iTickToId);
    if (iSaved) {
	OpenFile(GetDefaultTmpFileName());
    }
}

void CsuApp::OpenFile(const string& aFileName)
{
    if (iCaeEnv != NULL) {
	iCaeEnv->Root()->RemoveBaseViewProxy(iViewProxy);
	iViewProxy->UnsetRoot(iCaeEnv->Root());
	delete iCaeEnv;
	iCaeEnv = NULL;
	delete iVisAdp;
	iVisAdp = NULL;
    }
    iCaeEnv = CAE_Env::NewL(NULL, NULL, aFileName.c_str(), 1, NULL, iLogFileName.c_str());
    iCaeEnv->ConstructSystem();
    iCaeEnv->Root()->SetBaseViewProxy(iViewProxy, ETrue);
    iMainWnd->SetTitle(FormTitle(aFileName));
    // Visualization window adapter
    iVisAdp = new CsuVisAdp("VisAdp", iCaeEnv);
    CAE_ConnPointBase* c_adinp = iCaeEnv->Root()->GetInpN("viswin");
    if (c_adinp != NULL) {
	iVisAdp->ConnectWnd(c_adinp);
    }
    iVisAdp->SetWnd(iMainWnd->GetEnviw());
}

void CsuApp::OnCmdFileSaveAs()
{
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new ("Save As", GTK_WINDOW(iMainWnd->iWidget), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, 
	    GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
	char *filename;
	filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
	SaveAs(filename);
	g_free (filename);
    }
    gtk_widget_destroy (dialog);
}

void CsuApp::SaveAs(const string& aFileName)
{
    iCaeEnv->Root()->Chromo().Save(aFileName);
}

void CsuApp::SaveTmp()
{
}


