
#include <gtk/gtk.h>
#include <stdlib.h>
#include "app.h"


int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    CsuApp* app = new CsuApp();

    gtk_main(); 
    
    delete app;
    return 0;
}
