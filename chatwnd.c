#include "globals.h"
#include "hashtables.h"
void onYAMPReceiveIM(char *username, char *data) {
	
}
void SpawnChatWindow(char* toWho) {
	printf("spawning a chat window for %s\n", toWho);

	GtkWidget *chat_window = gtk_application_window_new(global_app);
	char *window_title = malloc(6 + 3 + strlen(GetDisplayName(toWho)) + 1);
	sprintf(window_title,"Yampen - %s", GetDisplayName(toWho));
	gtk_window_set_title(GTK_WINDOW(chat_window), window_title);
	gtk_window_set_default_size(GTK_WINDOW(chat_window), 600, 400);
	gtk_window_present(GTK_WINDOW(chat_window));
	
}