#include <gtk/gtk.h>
#include "modules/files.h"

const int window_height = 400;
const int window_width = 500;

static void setup_listitem (GtkListItemFactory *factory, GtkListItem *listitem) {
    (void)factory;
    GtkWidget *label = gtk_label_new(NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_list_item_set_child (listitem, label);
}

static void bind_listitem (GtkListItemFactory *factory, GtkListItem *listitem)
{
    (void)factory;
    GtkWidget* label = gtk_list_item_get_child(listitem);

    // получаем данные из модели
    GtkStringObject* strobj = gtk_list_item_get_item (listitem);

    // устанавливаем по этим данным текст метки
    gtk_label_set_text (GTK_LABEL (label), gtk_string_object_get_string(strobj));
}
 


static void app_activate(GtkApplication *app, gpointer user_data) {
    (void)user_data;

    GtkWidget *window = gtk_application_window_new (GTK_APPLICATION (app));
    gtk_window_set_default_size (GTK_WINDOW (window), window_width, window_height);
    gtk_window_set_title (GTK_WINDOW (window), "Cur dir homework!");
    
    // создаем контейнер GtkBox, расположение горизонтальное, padding = 10
    GtkWidget* box =  gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_valign(box, GTK_ALIGN_FILL);

    size_t count = 0;
    char **file_list = NULL;
    char *cwd = g_get_current_dir();

    file_list = get_files_arr(cwd, file_list, &count);
    if(!file_list) {
        g_printerr("get_files_arr() error");
        for(size_t i = 0; i < count; i++) {
            g_free( file_list[i] );
        }
        g_free(file_list);
        g_free(cwd);
        return;
    }
    for(size_t i=0;i<count;i++) {
        g_print(" %s\n", file_list[i]);
    }

    // GtkTreeView виджет является deprecated в GTK4 !
    // Вместо GtkTreeView используется теперь GtkListView

    GtkStringList* list = gtk_string_list_new((const char * const *)file_list);
    GtkSingleSelection* model = gtk_single_selection_new(G_LIST_MODEL(list)); 
    
    // Создание фабрики для элементов списка
    GtkListItemFactory* factory = gtk_signal_list_item_factory_new();
    g_signal_connect (factory, "setup", G_CALLBACK (setup_listitem), NULL);
    g_signal_connect (factory, "bind", G_CALLBACK (bind_listitem), NULL);
 
    // Создание GtkListView
    GtkWidget* list_view = gtk_list_view_new(GTK_SELECTION_MODEL(model), factory);

    GtkWidget *scroll = gtk_scrolled_window_new();

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list_view);
    
    gtk_box_append(GTK_BOX (box), scroll);

    gtk_widget_set_hexpand(scroll, TRUE);
    gtk_widget_set_vexpand(scroll, TRUE);

    gtk_window_set_child (GTK_WINDOW (window), box);
    gtk_window_present (GTK_WINDOW (window));

    for(size_t i = 0; i < count; i++) {
        g_free( file_list[i] );
    }
    g_free(file_list);
    g_free(cwd);

}

int main (int argc, char **argv) {
    GtkApplication *app = gtk_application_new ("com.homework", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);

    int status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref(app);

    return status;
}
