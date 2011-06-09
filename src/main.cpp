
#include <gtk/gtk.h>
#include <stdlib.h>
#include "app.h"

/* Time slice of FAP environment, in milliseconds */
const gint KFapeTimeSlice = 50;

gboolean idle_event_handler(gpointer data);

static GtkWidget *main_window;

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    CsuApp* app = new CsuApp();

    /* Use idle of main loop to drive FAP environment */
    g_timeout_add(KFapeTimeSlice, idle_event_handler, NULL);

    gtk_main(); 
    
    delete app;
    return 0;
}

gboolean idle_event_handler(gpointer data)
{
    return ETrue;
}
