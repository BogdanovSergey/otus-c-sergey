#include <gtk/gtk.h>
#include <stdlib.h>
#include "button.c"

GtkWidget *button;

static void app_activate(GtkApplication *app, gpointer user_data) {

    GtkWidget *window = gtk_application_window_new (GTK_APPLICATION (app));

    gtk_window_set_default_size (GTK_WINDOW (window), 250, 200);
    gtk_window_set_title (GTK_WINDOW (window), "Otus homework!!!");
    

    button = gtk_button_new_with_label("Click Me");
    
    g_signal_connect (button, "clicked", G_CALLBACK (click_button), NULL);
    gtk_window_set_child (GTK_WINDOW (window), button);


    gtk_window_present (GTK_WINDOW (window));


}

int main (int argc, char **argv)
{

    GtkApplication *app = gtk_application_new ("com.homework", G_APPLICATION_DEFAULT_FLAGS);


    g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);


    int status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref(app);

    return status;
    
    


    // отображаем окно
    
 




    // запускаем цикл прослушивания событий и взаимодействуем с пользователем
    //выходим из цикла, когда все окна приложения будут закрыты
    // while (g_list_model_get_n_items (gtk_window_get_toplevels ()) > 0)
    //     g_main_context_iteration (NULL, TRUE);

    // // когда окно приложения закрыто, выходим из приложения
    // return EXIT_SUCCESS;

}
