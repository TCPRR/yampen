#include <stdio.h>
#include <gtk/gtk.h>
#include "login.h"
#include "globals.h"
#include <cjson/cJSON.h>
GtkApplication *global_app;
static void activate(GtkApplication *app, gpointer user_data) {
	DisplayLoginDialog(app);
	/*
	GtkWidget *window;

	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Yampen");
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 300);
	gtk_window_present(GTK_WINDOW(window));
	GMenu *menu = g_menu_new();
*/
}

void onYAMPBuddyListed(cJSON *Buddies) {
	printf(cJSON_Print(Buddies));
}
int main(int argc, char **argv) {
	int status;

	global_app = gtk_application_new("xyz.defautluser0.tcp.yampen", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(global_app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(global_app), argc, argv);
	g_object_unref(global_app);
	return status;
}
