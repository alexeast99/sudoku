/*
* Date: 09/15/20
* Author: Alex Eastman
* Summary: Main program file for Sudoku
*/

#include <gtkmm.h>
#include <iostream>

/*
*  User-defined headers
*/
#include "include/board.h"

/*
*  Declare functions
*/
void open_game_single ();
void close_game_single ();
void change_game_type ();

Glib::RefPtr<Gtk::Builder> builder;


/*
*  User-defined functions
*/

// Opens the game window for single-board games
// TODO: Generate game board before displaying (i.e. initialize game state)
void
open_game_single (void)
{
  Gtk::Window* window;
  builder -> get_widget ("game_window_single", window);
  if (window)
    window -> show();
}

// Closes the game window for single-board games
// TODO: Clear game board after closing (i.e. reset game state)
void
close_game_single (void)
{
  Gtk::Window* window;
  builder -> get_widget ("game_window_single", window);
  if (window)
    window -> hide();
}

// Deselects opposite toggle button on main menu when one is clicked
void
change_game_type (void)
{
  Gtk::ToggleButton*  multi_board_toggle;
  Gtk::ToggleButton* single_board_toggle;

  builder -> get_widget ("multi_board_toggle", multi_board_toggle);
  builder -> get_widget ("single_board_toggle", single_board_toggle);



}





int
main(int argc, char **argv)
{

  // Required. Initialize gtkmm, checks command line arguments, initializes variabes.
  auto app = Gtk::Application::create (argc, argv, "alexeast.sudoku");

  // Declare pointers for widgets to be loaded from builder
  Gtk::Window* window;
  Gtk::Button* begin_button;
  Gtk::Button* single_done_button;
  Gtk::ToggleButton* multi_board_toggle;
  Gtk::ToggleButton* single_board_toggle;

  // Create builder from Glade file. Load necessary widgets.
  builder  = Gtk::Builder::create_from_file ("res/GUI.glade");
  builder -> get_widget ("application_window", window);
  builder -> get_widget ("begin_button", begin_button);
  builder -> get_widget ("single_done_button", single_done_button);
  builder -> get_widget ("multi_board_toggle", multi_board_toggle);
  builder -> get_widget ("single_board_toggle", single_board_toggle);

  // Connect signals
  begin_button        -> signal_clicked().connect(sigc::ptr_fun(&open_game_single));
  single_done_button  -> signal_clicked().connect(sigc::ptr_fun(&close_game_single));
  multi_board_toggle  -> signal_clicked().connect(sigc::ptr_fun(&change_game_type));
  single_board_toggle -> signal_clicked().connect(sigc::ptr_fun(&change_game_type));

  // Ensure window is not null. Show window and enter gtkmm main loop
  if (window)
    app -> run(*window);

  delete window;

  return 0;
}
