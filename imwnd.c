#include <stdio.h>
#include <gtk/gtk.h>
#include "imwnd.h"
#include "chatwnd.h"
#include "gtk/gtkshortcut.h"
#include "protocol/yamp.h"
#include "globals.h"
#include "hashtables.h"
#include <cjson/cJSON.h>
GtkWidget *main_window;
GtkWidget *BuddyList;
void on_buddy_row_activated(GtkListBox *box, GtkListBoxRow *row,
                            gpointer user_data) {
	GtkWidget *child = gtk_list_box_row_get_child(row);
	char *name = gtk_label_get_text(GTK_LABEL(child));
	char *username = g_object_get_data(G_OBJECT(child),"username");
	SpawnChatWindow(username);
}
void StartMainIMWindow() {
	gtk_widget_set_visible(main_window, 1);
	gtk_window_present(GTK_WINDOW(main_window));
	BuddyList = gtk_list_box_new();
	gtk_window_set_child(GTK_WINDOW(main_window), BuddyList);
	gtk_list_box_set_selection_mode(GTK_LIST_BOX(BuddyList),
									GTK_SELECTION_MULTIPLE); //i need hlep
	g_signal_connect(BuddyList, "row-activated",
	                 G_CALLBACK(on_buddy_row_activated), NULL);
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
		const char *Name =
		    cJSON_GetObjectItem(Buddy, "name")->valuestring;
		const char *DisplayName =
		    cJSON_GetObjectItem(Buddy, "display_name")->valuestring;
		InsertDisplayName(Name, DisplayName);
		GtkWidget *LBRow = gtk_list_box_row_new();
		GtkWidget *LBRowLabel = gtk_label_new(DisplayName);
		g_object_set_data(G_OBJECT(LBRowLabel),"username",(gpointer)Name);
		gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(LBRow), LBRowLabel);
		gtk_widget_set_halign(LBRowLabel, GTK_ALIGN_START);
		gtk_list_box_append(GTK_LIST_BOX(BuddyList), LBRow);
	}
}