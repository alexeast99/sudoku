/*
* Last Modified: 09/19/20
* Author: Alex Eastman
* Contact: alexeast@buffalo.edu
* Summary: Main program file for Sudoku
*/

#include <gtkmm.h>
#include <iostream>

/*
*  User-defined headers
*/
#include "board.h"


//Declare functions
void open_game ();
void close_game ();
void open_instructions ();
void close_instructions ();
void initalize_board ();
void reset_board();

Glib::RefPtr<Gtk::Builder> builder;


/************************
*  User-defined functions
************************/

// Opens the game window for single-board games
// TODO: Generate game board before displaying (i.e. initialize game state)
void
open_game (void)
{
  Gtk::Window* window;
  builder -> get_widget ("game_window", window);
  if (window)
    window -> show();
}

// Closes the game window for single-board games
// TODO: Clear game board after closing (i.e. reset game state)
void
close_game (void)
{
  Gtk::Window* window;
  builder -> get_widget ("game_window", window);
  if (window)
    window -> hide();
}

// Open the 'How to Play' dialog
void
open_instructions (void)
{
    Gtk::Dialog* instructions_dialog;
    builder -> get_widget ("how_to_play_dialog", instructions_dialog);
    if (instructions_dialog)
        instructions_dialog -> show();
}

// Close the 'How to Play' dialog
void
close_instructions (void)
{
    Gtk::Dialog* instructions_dialog;
    builder -> get_widget ("how_to_play_dialog", instructions_dialog);
    if (instructions_dialog)
        instructions_dialog -> hide();
}

// Write the text buffer that holds the instructions for the 'How to Play' dialog
void
write_instructions (void)
{
    Gtk::TextView* instructions_text_view;
    Glib::RefPtr<Gtk::TextBuffer> instructions_text_buffer;

    builder -> get_widget ("how_to_play_text_view", instructions_text_view);
    if (instructions_text_view)
        instructions_text_buffer = instructions_text_view -> get_buffer();
    instructions_text_buffer -> set_text ("Success!");
}

// Set the alignments of all cells to 0.5 so that text is centered.
// TODO: Font size? Font? Set reserved. If reserved, set un-editable.
void
initialize_board (void)
{
    int i, j;
    for (i=1; i<10; i++) {
        for (j=1; j<10; j++) {
            char cell_name[30];
            sprintf(cell_name, "board_%d_%d", i, j);

            Gtk::Entry*  cell;
            builder -> get_widget (cell_name, cell);

            cell -> set_alignment (0.5);
        }
    }
}

// Set every entry back to blank when reset button is hit. Excludes buttons set by the game.
// TODO: Exclude buttons set by the game.
void
reset_board (void)
{
    int i, j;
    for (i=1; i<10; i++) {
        for (j=1; j<10; j++) {
            char cell_name[30];
            sprintf(cell_name, "board_%d_%d", i, j);

            Gtk::Entry*  cell;
            builder -> get_widget (cell_name, cell);

           cell -> set_text(""); 
        }
   }
}




int
main(int argc, char **argv)
{

  // Required. Initialize gtkmm, checks command line arguments, initializes variabes.
  auto app = Gtk::Application::create (argc, argv, "alexeast.sudoku");

  // Declare pointers for widgets to be loaded from builder
  Gtk::Window* window;
  
  Gtk::Button* begin_button;
  Gtk::Button* done_button;
  Gtk::Button* how_to_play_button;
  Gtk::Button* got_it_button;
  Gtk::Button* reset_button;


  // Create builder from Glade file. Load necessary widgets.
  builder  = Gtk::Builder::create_from_file ("res/GUI.glade");
  builder -> get_widget ("application_window", window);

  builder -> get_widget ("begin_button", begin_button);
  builder -> get_widget ("done_button", done_button);
  builder -> get_widget ("how_to_play_button", how_to_play_button);
  builder -> get_widget ("got_it_button", got_it_button);
  builder -> get_widget ("reset_button", reset_button);

  // Connect signals
  // sigc::ptr_fun() creates a slot/function object/functor. Helps with compatibility
  begin_button -> signal_clicked().connect(sigc::ptr_fun(&open_game));
  done_button  -> signal_clicked().connect(sigc::ptr_fun(&close_game));
  how_to_play_button -> signal_clicked().connect(sigc::ptr_fun(&open_instructions));
  got_it_button -> signal_clicked().connect(sigc::ptr_fun(&close_instructions));
  reset_button -> signal_clicked().connect(sigc::ptr_fun(&reset_board));

  // Set everything up before opening the game
  write_instructions();
  initialize_board();

  // Ensure window is not null. Show window and enter gtkmm main loop
  if (window)
    app -> run(*window);

  delete window;

  return 0;
}
