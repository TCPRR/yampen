#include <stdio.h>
#include <gtk/gtk.h>
#include "imwnd.h"
#include "glib-object.h"
#include "login.h"
#include "chatwnd.h"
#include "glibconfig.h"
#include "gtk/gtkshortcut.h"
#include "protocol/yamp.h"
#include "globals.h"
#include "hashtables.h"
#include <cjson/cJSON.h>
GtkWidget *main_window;
GtkWidget *BuddyList;
GtkWidget *GuildList;
gboolean listmode = TRUE;
char* curSpace = NULL;
void onYAMPDisconnected() {
	GtkAlertDialog *alert = gtk_alert_dialog_new("Server disconnected!");
	DisplayLoginDialog(global_app);
}
void on_buddy_row_activated(GtkListBox *box, GtkListBoxRow *row,
                            gpointer user_data) {
	if (listmode == TRUE) {
	
	GtkWidget *child = gtk_widget_get_next_sibling(
	    gtk_widget_get_first_child(gtk_list_box_row_get_child(row)));
	char *name = gtk_label_get_text(GTK_LABEL(child));
	char *username = g_object_get_data(G_OBJECT(child), "username");

	SpawnChatWindow(MakeDMChannel(username, curUsername));
	} else {
		char *channelname =
		    gtk_label_get_text(GTK_LABEL(gtk_list_box_row_get_child(row)));
		char *loc = malloc(1 + strlen(curSpace) + strlen(channelname) + 1);
		*loc = '^';
		strcpy(loc + 1, curSpace);
		strcpy(loc + strlen(curSpace) + 1, channelname);
		SpawnChatWindow(loc);
	}
}
GCallback on_space_row_activated(GtkListBox *box, GtkListBoxRow *row,
                                 gpointer user_data) {
	if ((strcmp((char *)g_object_get_data(G_OBJECT(row), "name"), "dm") == 0)) {
		if(listmode == FALSE){
			YAMPListBuddies(mainsock);
			listmode = TRUE;
		}
	} else {
		YAMPListSpaceChannels(mainsock,
		                      (char *)g_object_get_data(G_OBJECT(row), "name"));
		listmode = FALSE;
	}
}
void onYAMPSpacesFetched(cJSON *Spaces) {
	for (int i = 0; i < cJSON_GetArraySize(Spaces); i++) {
		cJSON *Space = cJSON_GetArrayItem(Spaces, i);
		printf("%s\n", cJSON_GetObjectItem(Space, "display_name")->valuestring);

		GtkWidget *gldbtn = gtk_list_box_row_new();
		GtkWidget *gldbtnimg = gtk_image_new_from_file("./yamp.png");

		gtk_widget_set_size_request(gldbtnimg, 36, 36);
		gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(gldbtn), gldbtnimg);

		gtk_list_box_append(GTK_LIST_BOX(GuildList), gldbtn);
		g_object_set_data(G_OBJECT(gldbtn), "name",
		                  cJSON_GetObjectItem(Space, "name")->valuestring);
		curSpace = strdup(cJSON_GetObjectItem(Space, "name")->valuestring);
	}
}
void onYAMPChannelsFetched(cJSON *Channels) {
	gtk_list_box_remove_all(GTK_LIST_BOX(BuddyList));
	for (int i = 0; i < cJSON_GetArraySize(Channels); i++) {
		cJSON *Channel = cJSON_GetArrayItem(Channels, i);
		char *channelname = cJSON_GetObjectItem(Channel, "name")->valuestring;
		char *dispchannelname = malloc(strlen(channelname) + 2);
		*dispchannelname = '#';
		memcpy(dispchannelname + 1, channelname, strlen(channelname) + 1);
		GtkWidget *lbr = gtk_list_box_row_new();
		GtkWidget *label = gtk_label_new(dispchannelname);
		gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(lbr), label);
		gtk_list_box_append(GTK_LIST_BOX(BuddyList), lbr);
	}
}
void StartMainIMWindow() {
	gtk_widget_set_visible(main_window, 1);
	gtk_window_present(GTK_WINDOW(main_window));
	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	GuildList = gtk_list_box_new();
	GtkWidget *dmsbtn = gtk_list_box_row_new();
	g_object_set_data(G_OBJECT(dmsbtn), "name", "dm");
	GtkWidget *dmsbtnimg = gtk_image_new_from_file("./yamp.png");

	gtk_widget_set_size_request(dmsbtnimg, 36, 36);
	gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(dmsbtn), dmsbtnimg);
	gtk_list_box_append(GTK_LIST_BOX(GuildList), dmsbtn);

	gtk_box_append(GTK_BOX(hbox), GuildList);

	gtk_widget_set_hexpand(hbox, TRUE);
	gtk_window_set_child(GTK_WINDOW(main_window), vbox);
	gtk_box_append(GTK_BOX(vbox), hbox);

	g_signal_connect(GuildList, "row-activated",
	                 G_CALLBACK(on_space_row_activated), NULL);

	BuddyList = gtk_list_box_new();
	gtk_widget_set_hexpand(BuddyList, TRUE);
	gtk_widget_set_vexpand(BuddyList, TRUE);
	gtk_box_append(GTK_BOX(hbox), BuddyList);
	gtk_list_box_set_selection_mode(GTK_LIST_BOX(BuddyList),
	                                GTK_SELECTION_MULTIPLE); // i need hlep
	g_signal_connect(BuddyList, "row-activated",
	                 G_CALLBACK(on_buddy_row_activated), NULL);
	GtkWidget *UserDetailsBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	char *displayName = GetDisplayName(curUsername);
	if (!displayName) {
		displayName = curUsername;
	}
	GtkWidget *UsernameLabel = gtk_label_new(displayName);
	GtkWidget *Pfp = gtk_image_new_from_file("./pfp.png");

	gtk_widget_set_size_request(Pfp, 36, 36);
	GtkCssProvider *provider = gtk_css_provider_new();
	gtk_css_provider_load_from_data(
	    provider, "image { border-radius: 100%; border: 2px solid #108020; }",
	    -1);

	gtk_style_context_add_provider(gtk_widget_get_style_context(Pfp),
	                               GTK_STYLE_PROVIDER(provider),
	                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	gtk_box_append(GTK_BOX(UserDetailsBox), Pfp);
	gtk_box_append(GTK_BOX(UserDetailsBox), UsernameLabel);
	gtk_box_append(GTK_BOX(vbox), UserDetailsBox);
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
	gtk_list_box_remove_all(GTK_LIST_BOX(BuddyList));
	for (int i = 0; i < cJSON_GetArraySize(Buddies); i++) {
		cJSON *Buddy = cJSON_GetArrayItem(Buddies, i);
		const char *Name = cJSON_GetObjectItem(Buddy, "name")->valuestring;
		const char *DisplayName =
		    cJSON_GetObjectItem(Buddy, "display_name")->valuestring;
		InsertDisplayName(Name, DisplayName);
		GtkWidget *ItemBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
		GtkWidget *LBRow = gtk_list_box_row_new();
		GtkWidget *LBRowLabel = gtk_label_new(DisplayName);
		GtkWidget *Pfp;
		if (!cJSON_GetObjectItem(Buddy, "pfp")) {
			Pfp = gtk_image_new_from_file("./pfp.png");
		}
		gtk_widget_set_size_request(Pfp, 36, 36);
		GtkCssProvider *provider = gtk_css_provider_new();
		gtk_css_provider_load_from_data(
		    provider,
		    "image { border-radius: 100%; border: 2px solid #108020; }", -1);

		gtk_style_context_add_provider(gtk_widget_get_style_context(Pfp),
		                               GTK_STYLE_PROVIDER(provider),
		                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
		g_object_set_data(G_OBJECT(LBRowLabel), "username", (gpointer)Name);
		gtk_box_append(GTK_BOX(ItemBox), Pfp);
		gtk_box_append(GTK_BOX(ItemBox), LBRowLabel);
		gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(LBRow), ItemBox);
		gtk_widget_set_halign(LBRowLabel, GTK_ALIGN_START);
		gtk_list_box_append(GTK_LIST_BOX(BuddyList), LBRow);
	}
}

void onYAMPUserDetailsFetched(cJSON *Details) {
	char *username = cJSON_GetObjectItem(Details, "name")->valuestring;
	char *display_name =
	    cJSON_GetObjectItem(Details, "display_name")->valuestring;
	InsertDisplayName(username, display_name);
}