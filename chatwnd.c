#include "globals.h"
#include "gtk/gtk.h"
#include "hashtables.h"
#include "protocol/yamp.h"
typedef struct {
	char *toWho;
	GtkWidget* EntryArea;
} send_im_obj;
void onYAMPReceiveIM(char *username, char *data) {
	printf("Received an IM from %s, it says uh %s\n", username, data);
}
void gui_send_im(GtkApplication *app, gpointer user_data) {
	send_im_obj *dat = (send_im_obj *)user_data;
	char *content = gtk_entry_buffer_get_text(
	    gtk_entry_get_buffer(GTK_ENTRY(dat->EntryArea)));
	YAMPSendIM(mainsock,dat->toWho,content);
}
send_im_obj dat;
void SpawnChatWindow(char *toWho) {
	printf("spawning a chat window for %s\n", toWho);
	if (GetChatWindow(toWho)) {
		gtk_window_destroy(GTK_WINDOW(GetChatWindow(toWho)));
	}
	GtkWidget *chat_window = gtk_application_window_new(global_app);
	RegisterChatWindow(chat_window, toWho);
	char *window_title = malloc(6 + 3 + strlen(GetDisplayName(toWho)) + 1);
	sprintf(window_title, "Yampen - %s", GetDisplayName(toWho));
	gtk_window_set_title(GTK_WINDOW(chat_window), window_title);
	gtk_window_set_default_size(GTK_WINDOW(chat_window), 600, 400);

	// vertical arragning box thing
	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
	gtk_window_set_child(GTK_WINDOW(chat_window), vbox);

	// le chat area
	GtkWidget *scroll = gtk_scrolled_window_new();
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
	                               GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_vexpand(scroll, TRUE);

	GtkWidget *chat_view = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(chat_view), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(chat_view), GTK_WRAP_WORD_CHAR);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), chat_view);
	gtk_box_append(GTK_BOX(vbox), scroll);

	// input row
	GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_box_append(GTK_BOX(vbox), hbox);

	GtkWidget *entry = gtk_entry_new();
	gtk_widget_set_hexpand(entry, TRUE);
	gtk_box_append(GTK_BOX(hbox), entry);

	GtkWidget *send_btn = gtk_button_new_with_label("Send");
	gtk_box_append(GTK_BOX(hbox), send_btn);
	dat.EntryArea = entry;
	dat.toWho = toWho;
	g_signal_connect(send_btn, "clicked", G_CALLBACK(gui_send_im), &dat);

	gtk_window_present(GTK_WINDOW(chat_window));
}