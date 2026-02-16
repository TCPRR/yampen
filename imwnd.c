#include <stdio.h>
#include <gtk/gtk.h>
#include "imwnd.h"
#include "gtk/gtkshortcut.h"
#include "protocol/yamp.h"
#include "globals.h"


void StartMainIMWindow(GtkApplication *app) {
	GtkWidget *window;

	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Yampen");
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 300);
	gtk_window_present(GTK_WINDOW(window));
	GMenu *menu = g_menu_new();
}