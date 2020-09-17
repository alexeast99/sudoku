/*
* Date: 09/15/20
* Author: Alex Eastman
* Summary: Main program file for Sudoku
*/

#include <gtkmm.h>
#include <iostream>

// extern "C" {
//     void open_game_single (GtkButton *button, gpointer user_data);
//     void close_game_single (GtkButton *button, gpointer user_data);
// }

Glib::RefPtr<Gtk::Builder> builder;


/*
*  User-defined functions
*/

// Opens the game window for single-board games
// TODO: Generate game board before displaying (i.e. initialize game state)
// void open_game_single (GtkButton *button, gpointer user_data) {
//   GtkWidget *window = GTK_WIDGET ( gtk_builder_get_object (builder, "game_window_single"));
//   gtk_widget_show (window);
// }

// Closes the game window for single-board games
// TODO: Clear game board after closing (i.e. reset game state)
// void close_game_single (GtkButton *button, gpointer user_data) {
//   GtkWidget *window = GTK_WIDGET ( gtk_builder_get_object (builder, "game_window_single"));
//   gtk_widget_hide (window);
// }





int main (int argc, char **argv) {

  Gtk::Window* window;

  auto app = Gtk::Application::create (argc, argv, "alexeast.sudoku");

  // Get the builder from Glade-generate UI definition. Get window from builder.
  builder = Gtk::Builder::create_from_file ("res/GUI.glade", "application_window");
  builder -> get_widget ("application_window", window);

  if (window) {
    app -> run(*window);
  }

  delete window;

  return 0;
}
