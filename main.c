#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

// Defines 
#define MAX_VERSION_LENGTH 100000 // Maximum length of version number

// Callback function for when the window is closed
static void on_window_closed(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

// Function to read version number from version.conf file
static char *read_version_number() {
    FILE *fp = fopen("version.conf", "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: version.conf not found or cannot be opened.\n");
        exit(EXIT_FAILURE);
    }

    char *version = (char *)malloc(MAX_VERSION_LENGTH * sizeof(char));
    if (version == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    if (fgets(version, MAX_VERSION_LENGTH, fp) == NULL) {
        fprintf(stderr, "Error: Failed to read version number.\n");
        exit(EXIT_FAILURE);
    }

    fclose(fp);
    return version;
}

// Callback function for opening a file
static void on_open_file(GtkWidget *widget, gpointer data) {
    GtkFileChooserDialog *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = GTK_FILE_CHOOSER_DIALOG(gtk_file_chooser_dialog_new("Open File",
                                         GTK_WINDOW(data),
                                         action,
                                         "_Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         "_Open",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL));

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        
        // Open the file and read its contents
        FILE *file = fopen(filename, "r");
        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            rewind(file);
            char *file_contents = (char *)malloc(file_size + 1);
            if (file_contents != NULL) {
                fread(file_contents, 1, file_size, file);
                file_contents[file_size] = '\0';
                
                // Set the file contents into the text buffer
                GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(data));
                gtk_text_buffer_set_text(buffer, file_contents, -1);
                
                free(file_contents);
            }
            fclose(file);
        }
        // Update file label with selected filename
        gtk_label_set_text(GTK_LABEL(data), filename);
        g_free(filename);
    }

    gtk_widget_destroy(GTK_WIDGET(dialog));
}

int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    char *version = read_version_number();
    char window_title[50];
    snprintf(window_title, 50, "Kilt - Unix %s", version); // Append version number to window title
    gtk_window_set_title(GTK_WINDOW(window), window_title);
    free(version);

    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_closed), NULL);

    // Create a vertical box container for the sidebar and text editor
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create a label for displaying the file name
    GtkWidget *file_label = gtk_label_new("File: None");
    gtk_box_pack_start(GTK_BOX(vbox), file_label, FALSE, FALSE, 0);

    // Create a button to open a file
    GtkWidget *open_button = gtk_button_new_with_label("Open File");
    g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_file), file_label);
    gtk_box_pack_start(GTK_BOX(vbox), open_button, FALSE, FALSE, 0);

    // Create a text view for editing code
    GtkWidget *text_view = gtk_text_view_new();
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    // Create a scrolled window to contain the text view
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);

    // Add the scrolled window to the vertical box container
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    // Show all widgets
    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();

    return 0;
}
