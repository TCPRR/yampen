#include <glib.h>
#include "hashtables.h"
GHashTable *dtt;
void InitDisplayNameTable() { dtt = g_hash_table_new(g_str_hash, g_str_equal); }
char *InsertDisplayName(char *username, char *display_name) {
	g_hash_table_insert(dtt, username, display_name);
}
char *GetDisplayName(char *username) {
	return g_hash_table_lookup(dtt,username);
}