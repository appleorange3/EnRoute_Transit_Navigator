#include <gtk/gtk.h>

// Callback function for the "Find Route" button
void on_find_route_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *start_entry = GTK_WIDGET(g_object_get_data(G_OBJECT(widget), "start_entry"));
    GtkWidget *end_entry = GTK_WIDGET(g_object_get_data(G_OBJECT(widget), "end_entry"));
    GtkWidget *output_text_view = GTK_WIDGET(g_object_get_data(G_OBJECT(widget), "output_text_view"));
    GtkWidget *path_combo = GTK_WIDGET(g_object_get_data(G_OBJECT(widget), "path_combo"));

    const gchar *start = gtk_entry_get_text(GTK_ENTRY(start_entry));
    const gchar *end = gtk_entry_get_text(GTK_ENTRY(end_entry));
    gchar *path_type = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(path_combo));

    // Implement route finding logic here
    gchar *result = g_strdup_printf("Finding %s route from %s to %s...", path_type, start, end);

    // Display result
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_text_view));
    gtk_text_buffer_set_text(buffer, result, -1);
    g_free(result);
    g_free(path_type);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *grid;
    GtkWidget *start_label, *end_label, *path_label;
    GtkWidget *start_entry, *end_entry;
    GtkWidget *find_route_button, *reset_button;
    GtkWidget *path_combo;
    GtkWidget *output_text_view;
    GtkTextBuffer *buffer;
    GtkCssProvider *css_provider;
    GtkStyleContext *context;

    gtk_init(&argc, &argv);

    // Create main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Delhi Transportation System");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create vertical box container
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create grid container for input fields
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 10);

    // Create labels and input fields
    start_label = gtk_label_new("Starting Location:");
    end_label = gtk_label_new("Destination:");
    path_label = gtk_label_new("Path Type:");

    start_entry = gtk_entry_new();
    end_entry = gtk_entry_new();

    // Create dropdown menu for path type
    path_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(path_combo), "Shortest Path");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(path_combo), "Cost-Effective Path");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(path_combo), "Safest Path");

    // Create output text view
    output_text_view = gtk_text_view_new();
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_text_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(output_text_view), FALSE);

    // Create buttons
    find_route_button = gtk_button_new_with_label("Find Route");
    reset_button = gtk_button_new_with_label("Reset");

    // Arrange widgets in grid
    gtk_grid_attach(GTK_GRID(grid), start_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), start_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), end_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), end_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), path_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), path_combo, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), find_route_button, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), reset_button, 1, 3, 1, 1);

    gtk_box_pack_start(GTK_BOX(vbox), output_text_view, TRUE, TRUE, 10);

    // Connect signals
    g_object_set_data(G_OBJECT(find_route_button), "start_entry", start_entry);
    g_object_set_data(G_OBJECT(find_route_button), "end_entry", end_entry);
    g_object_set_data(G_OBJECT(find_route_button), "output_text_view", output_text_view);
    g_object_set_data(G_OBJECT(find_route_button), "path_combo", path_combo);
    g_signal_connect(find_route_button, "clicked", G_CALLBACK(on_find_route_clicked), NULL);

    // Load and apply CSS for styling
    css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider,
        "button { padding: 10px; margin: 5px; border-radius: 5px; }"
        "entry { padding: 5px; margin: 5px; }"
        "textview { padding: 10px; margin: 10px; border: 1px solid #ccc; }"
        "label { margin: 5px; }",
        -1, NULL);
    context = gtk_widget_get_style_context(window);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Show all widgets
    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}
