#include <gtk/gtk.h>
#include "files.h"

char ** get_files_arr(const char *path, char ** arr, size_t *count) {
    g_print ("path: %s\n", path);

    GError *error = NULL;
    GDir *dir = g_dir_open(path, 0, &error);
    if (!dir) {
        g_printerr("Ошибка: %s\n", error->message);
        g_error_free(error);
        return NULL;
    }

    const char *filename;
    int fcount = 0;

    // Вставляем ".."
    arr = g_realloc(arr, (fcount+1) * sizeof(char *));
    if(!arr) {
        g_printerr("g_realloc error");
        g_dir_close(dir);
        return NULL;
    }
    arr[fcount] = g_strdup("..");
    fcount++;

    while ((filename = g_dir_read_name(dir)) != NULL) {
        char *full_path = g_build_filename(path, filename, NULL);
        if(g_file_test(full_path, G_FILE_TEST_IS_DIR)) {
            // Добавляем / для директорий
            char * ready_path = g_strconcat(full_path, "/", NULL);
            g_free(full_path);
            full_path = ready_path;
        } else {
            // Добавляем отступ для файлов
            char * ready_path = g_strconcat("    ", filename, NULL);
            g_free(full_path);
            full_path = ready_path;
        }

        arr = g_realloc(arr, (fcount + 1) * sizeof(char *));
        if(!arr) {
            g_printerr("g_realloc error");
            return NULL;
        }
        arr[fcount] = full_path;

        fcount++;
    }

    // Вставляем терминатор NULL
    arr = g_realloc(arr, (fcount + 1) * sizeof(char *));
    if(!arr) {
        g_printerr("g_realloc error");
        return NULL;
    }
    arr[fcount] = NULL;

    *count = fcount;

    g_dir_close(dir);

    return arr;
}

