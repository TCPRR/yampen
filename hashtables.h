#include <gtk/gtk.h>
void InitAllTables();
char *InsertDisplayName(char *username, char *display_name);
char *GetDisplayName(char *username);
GtkWidget *GetChatWindow(char *forWho);
void RegisterChatWindow(GtkWidget *Window, char *forWho);