#include <stdio.h>
#include <gtk/gtk.h>

void SplashScreen(GtkApplication *app) {
	GtkWidget *window;

	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Yampen - Login");
	gtk_window_set_default_size(GTK_WINDOW(window), 500, 300);
	gtk_window_present(GTK_WINDOW(window));
	GtkWidget *title = gtk_label_new("Yampen");
	PangoAttrList *attrs = pango_attr_list_new();
	PangoAttribute *attrsize = pango_attr_size_new(24 * PANGO_SCALE); 
	pango_attr_list_insert(attrs, attrsize);
	PangoAttribute *attrweight = pango_attr_weight_new(800); 
	pango_attr_list_insert(attrs, attrweight);

	gtk_label_set_attributes(GTK_LABEL(title), attrs);

	gtk_window_set_child(GTK_WINDOW(window), title);
}
