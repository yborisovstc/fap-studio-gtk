
#include <gtk/gtk.h>
#include <stdlib.h>
#include "app.h"

//const char* KLogFileName = "fap-tias.log";
/* Time slice of FAP environment, in milliseconds */
const gint KFapeTimeSlice = 50;

gboolean idle_event_handler(gpointer data);
static gboolean delete_event_handler(GtkWidget *widget, GdkEvent *event, gpointer data);
static void destroy_event_handler(GtkWidget *widget, gpointer data);

/* Finite automata programming environment */
CAE_Env* fape = NULL;

static GtkWidget *main_window;

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    CsuApp* app = new CsuApp();
#if 0
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    // Header for "delete_event" signal (this is given by the window manager, usually by the "close" option, or on the titlebar)
    g_signal_connect (G_OBJECT (main_window), "delete_event", G_CALLBACK (delete_event_handler), NULL);
    // Handler for "destroy" event. It occurs on call gtk_widget_destroy(), or on return FALSE in the "delete_event" callback
    g_signal_connect (G_OBJECT(main_window), "destroy", G_CALLBACK (destroy_event_handler), NULL);

    gtk_widget_show(main_window);
#endif

    /* Use idle of main loop to drive FAP environment */
    g_timeout_add(KFapeTimeSlice, idle_event_handler, NULL);

    gtk_main(); 
    
    delete app;
    return 0;
}

gboolean idle_event_handler(gpointer data)
{
    /*
    if (fape != NULL) {
	fape->Step();
    }
    */
    return ETrue;
}

static gboolean delete_event_handler(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    // If you return FALSE, GTK will emit the "destroy" signal. Returning TRUE means you don't want the window to be destroyed. 
    return FALSE;
}

static void destroy_event_handler(GtkWidget *widget, gpointer data)
{
    gtk_main_quit ();
}




