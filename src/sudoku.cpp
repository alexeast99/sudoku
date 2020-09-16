/*
* Date: 09/15/20
* Author: Alex Eastman
* Summary: Main program file for Sudoku
*/

#include <gtk/gtk.h>
#include <iostream>

extern "C" {
    void open_game_single (GtkButton *button, gpointer user_data);
    void close_game_single (GtkButton *button, gpointer user_data);
}

GtkBuilder *builder;


/*
*  User-defined functions
*/

// Opens the game window for single-board games
// TODO: Generate game board before displaying (i.e. initialize game state)
void open_game_single (GtkButton *button, gpointer user_data) {
  GtkWidget *window = GTK_WIDGET ( gtk_builder_get_object (builder, "game_window_single"));
  gtk_widget_show (window);
}

// Closes the game window for single-board games
// TODO: Clear game board after closing (i.e. reset game state)
void close_game_single (GtkButton *button, gpointer user_data) {
  GtkWidget *window = GTK_WIDGET ( gtk_builder_get_object (builder, "game_window_single"));
  gtk_widget_hide (window);
}





int main (int argc, char **argv) {
  std::cout << "Spinning up\n";
  // Every GTK application needs a window. Since we're using Glade, we'll get the
  // window from a builder.
  GtkWidget  *window;

  // Initializes everything needed to operate toolkit. Call before other functions.
  gtk_init (&argc, &argv);

  // Get the builder from Glade-generate UI definition. Get window from builder.
  builder = gtk_builder_new_from_file ("res/GUI.glade");
  window  = GTK_WIDGET ( gtk_builder_get_object (builder, "application_window"));

  // De-reference builder after it's been used. References to builder should go
  // before this line.
  // g_object_unref ( G_OBJECT(builder));

  // Show the window. Stop application when window is closed. Start main loop.
  gtk_widget_show_all (window);
  g_signal_connect (window, "destroy", gtk_main_quit, NULL);
  gtk_builder_connect_signals (builder, NULL);
  gtk_main ();

  return 0;
}
