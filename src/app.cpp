#include "app.h"
#include <cagprov.h>
#include <widgets.h>

const char* KLogFileName = "fap-tias.log";

CsuApp::CsuApp()
{
    // Create Main window
    iMainWnd = new CsuMainWnd("MainWnd");
    iMainWnd->Show();
    iMainWnd->SetMenuObserver(this);
    // Create model
    iCaeEnv = CAE_Env::NewL(1, NULL, KLogFileName, 1);
    // Create view
    iCagProv = new CagProvider(iMainWnd);
    iViewProxy = iCagProv->CreateViewProxy();
    iCaeEnv->Root()->SetBaseViewProxy(iViewProxy, ETrue);
}

CsuApp::~CsuApp()
{
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
	g_free (filename);
    }
    gtk_widget_destroy (dialog);
}

void CsuApp::OnCmdStep()
{
    iCaeEnv->Step();
}

void CsuApp::OpenFile(const string& aFileName)
{
    if (iCaeEnv != NULL) {
	iCaeEnv->Root()->RemoveBaseViewProxy(iViewProxy);
	iViewProxy->UnsetRoot(iCaeEnv->Root());
	delete iCaeEnv;
	iCaeEnv = NULL;
    }
    iCaeEnv = CAE_Env::NewL(NULL, NULL, aFileName.c_str(), 1, NULL, KLogFileName);
    iCaeEnv->ConstructSystem();
    iCaeEnv->Root()->SetBaseViewProxy(iViewProxy, ETrue);
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

