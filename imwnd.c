#include <stdio.h>
#include <gtk/gtk.h>
#include "imwnd.h"
#include "gtk/gtkshortcut.h"
#include "protocol/yamp.h"
#include "globals.h"
#include <cjson/cJSON.h>
GtkWidget *main_window;
GtkWidget *BuddyList;
void StartMainIMWindow() {
	gtk_widget_set_visible(main_window, 1);
	gtk_window_present(GTK_WINDOW(main_window));
	BuddyList = gtk_list_box_new();
	gtk_window_set_child(GTK_WINDOW(main_window), BuddyList);
}

void CreateMainIMWindow(GtkApplication *app) {

	main_window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(main_window), "Yampen");
	gtk_window_set_default_size(GTK_WINDOW(main_window), 200, 300);
	gtk_window_present(GTK_WINDOW(main_window));
	GMenu *menu = g_menu_new();
	gtk_widget_set_visible(main_window, 0);
}

void onYAMPBuddyListed(cJSON *Buddies) {
	for (int i = 0; i < cJSON_GetArraySize(Buddies); i++) {
		cJSON *Buddy = cJSON_GetArrayItem(Buddies, i);
		const char *DisplayName =
		    cJSON_GetObjectItem(Buddy, "display_name")->valuestring;

		GtkWidget *LBRow = gtk_list_box_row_new();
		GtkWidget *LBRowLabel = gtk_label_new(DisplayName);
		gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(LBRow), LBRowLabel);
		gtk_widget_set_halign(LBRowLabel, GTK_ALIGN_START);
		gtk_list_box_append(GTK_LIST_BOX(BuddyList), LBRow);
	}
}