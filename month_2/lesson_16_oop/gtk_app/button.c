#include <gtk/gtk.h>

//static void click_button (GtkButton *btn);


static void click_button(GtkButton *btn, gpointer user_data) {
    (void)user_data;
    g_print ("Clicked !!!!!\n");

    GtkWidget* label = gtk_button_get_child (btn);
    if (GTK_IS_LABEL(label)) {
        gtk_button_set_label(GTK_LABEL(label), "Clicked!!!");
    }

}
