/*
* Last Modified: 09/23/20
* Author: Alex Eastman
* Contact: alexeast@buffalo.edu * Summary: Main program file for Sudoku
*/

#include <gtkmm.h>
#include <iostream>
#include <time.h>

/*
*  User-defined headers
*/
#include "board.h"


// Funciton prototypes
void open_game ();
void close_game ();
void open_instructions ();
void close_instructions ();
void initalize_board ();
void reset_board();

Glib::RefPtr<Gtk::Builder> builder;
Board board;


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
// TODO: Reset board state after closing. 
void
close_game (void)
{
    Gtk::Window* window;
    builder -> get_widget ("game_window", window);
    board.set_total_time();
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

// Customize TextView for each cell on the board 
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
void
reset_board (void)
{
    int i, j;
    for (i=1; i<10; i++) {
        for (j=1; j<10 && !board.check_reserved(i, j); j++) {
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

    /* Pointers for widgets to be loaded from builder
     *
     */

    // Window pointers
    Gtk::Window* game_window;
    Gtk::Window* window;
 
    // Button pointers
    Gtk::Button* begin_button;
    Gtk::Button* done_button;
    Gtk::Button* how_to_play_button;
    Gtk::Button* got_it_button;
    Gtk::Button* reset_button;
    Gtk::Button* finish_later_button;

      

    /* Create builder from Glade file. Load necessary widgets.
     *
     */

    // Window widgets
    builder  = Gtk::Builder::create_from_file ("res/GUI.glade");
    builder -> get_widget ("application_window", window);
    builder -> get_widget ("game_window", game_window);

    // Button widgets
    builder -> get_widget ("begin_button", begin_button);
    builder -> get_widget ("done_button", done_button);
    builder -> get_widget ("how_to_play_button", how_to_play_button);
    builder -> get_widget ("got_it_button", got_it_button);
    builder -> get_widget ("reset_button", reset_button);
    builder -> get_widget ("finish_later_button", finish_later_button);



    /* Connect signals. sigc::ptr_fun() creates a slot/function object/functor. Helps with compatibility 
     *
     */

    // Window signals
    game_window -> signal_hide().connect( sigc::mem_fun(board, &Board::set_total_time));

    // Button signals
    begin_button  -> signal_clicked().connect( sigc::ptr_fun(&open_game));
    begin_button  -> signal_clicked().connect( sigc::mem_fun(board, &Board::start) );
    done_button   -> signal_clicked().connect( sigc::ptr_fun(&close_game));
    got_it_button -> signal_clicked().connect( sigc::ptr_fun(&close_instructions));
    reset_button  -> signal_clicked().connect( sigc::ptr_fun(&reset_board));
    how_to_play_button -> signal_clicked().connect( sigc::ptr_fun(&open_instructions));

    

    /* CSS for styling
     *
     */

    // Css provider from CSS file in res directory
    auto css_provider = Gtk::CssProvider::create();
    css_provider -> load_from_path("res/styles.css");

    // Add stylesheet to buttons
    begin_button -> get_style_context() -> 
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER); 
    how_to_play_button -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    done_button -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    finish_later_button -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    reset_button -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Add stylesheet to windows
    window -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    game_window -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    /* Initial setup
     *
     */

    write_instructions();
    initialize_board();

    if (window)
        app -> run(*window);

    delete window;

    return 0;
}
