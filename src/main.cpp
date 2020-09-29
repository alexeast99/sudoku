/*
* Last Modified: 09/28/20
* Author: Alex Eastman
* Contact: alexeast@buffalo.edu
* Summary: Main program file for Sudoku
*/

#include <gtkmm.h>
#include <iostream>
#include <time.h>
#include <string.h>

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
void reset_board ();

void set_pointer (Glib::ustring);
void restore_pointer (Glib::ustring);

void check_if_number (guint, const char*, guint, Glib::RefPtr< Gtk::EntryBuffer >);

// Global references
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
// TODO: Reset board state after closing. Maybe different function?
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

    // Create CssProvider
    auto css_provider = Gtk::CssProvider::create();
    css_provider -> load_from_path ("res/styles.css");

    for (i=1; i<10; i++) {
        for (j=1; j<10; j++) {
            // ID of Gtk::Entry based on position in matrix
            gchar* cell_name = (gchar *) g_malloc(31);
            g_snprintf(cell_name, 31,"board_%d_%d", i, j);

            // Get current Gtk::Entry from the builder. Get the TextBuffer from the widget
            Gtk::Entry*  cell;
            Glib::RefPtr< Gtk::EntryBuffer > buffer;
            builder -> get_widget (cell_name, cell);
            buffer = cell -> get_buffer();

            // Center characters. Add CSS. Assign signal handler to only allow digits
            cell -> set_alignment (0.5);
            cell -> get_style_context() ->
                add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

            cell -> get_buffer() -> signal_inserted_text().connect(
              sigc::bind<Glib::RefPtr <Gtk::EntryBuffer> >(
                sigc::ptr_fun(&check_if_number), buffer));
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
            // ID of Gtk::Entry based on position in matrix
            gchar* cell_name = (gchar *) g_malloc(31);
            g_snprintf(cell_name, 31, "board_%d_%d", i, j);

            // Get current Gtk::Entry from the builder
            Gtk::Entry*  cell;
            builder -> get_widget (cell_name, cell);

            // Clear this Gtk::Entry's text
            cell -> set_text("");
        }
   }
}

// Set cursor to pointer when over button
void
set_pointer (Glib::ustring widget)
{
    auto clickable_cursor = Gdk::Cursor::create (Gdk::HAND2);
    Gtk::Button* button;
    builder -> get_widget (widget, button);
    button -> get_window() -> set_cursor (clickable_cursor);
}

// Set cursor to default when leaving button
void
restore_pointer (Glib::ustring widget)
{
    auto normal_cursor = Gdk::Cursor::create (Gdk::LEFT_PTR);
    Gtk::Button* button;
    builder -> get_widget (widget, button);
    button -> get_window() -> set_cursor (normal_cursor);
}

// Only allow numbers to be entered on sudoku
void
check_if_number (guint position, const gchar* chars, guint n_chars, Glib::RefPtr< Gtk::EntryBuffer > buffer)
{
  gchar inserted = *chars;
  if (inserted > 57 || inserted < 48) buffer -> delete_text(0, 1);
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

    // Grid points
    Gtk::Grid* board_container_grid;

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

    // Grid widgets
    builder -> get_widget ("board_container_grid", board_container_grid);

    /* Connect signals. sigc::ptr_fun() creates a slot/function object/functor. Helps with compatibility
     *
     */

    // Window signals
    game_window -> signal_hide().connect( sigc::mem_fun(board, &Board::set_total_time));

    // Button signals
    begin_button  -> signal_clicked().connect( sigc::ptr_fun(&open_game));
    begin_button  -> signal_clicked().connect( sigc::mem_fun(board, &Board::start) );
    begin_button  -> signal_enter().connect( sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "begin_button") );
    begin_button  -> signal_leave().connect( sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "begin_button") );

    how_to_play_button -> signal_clicked().connect( sigc::ptr_fun(&open_instructions));
    how_to_play_button  -> signal_enter().connect( sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "how_to_play_button") );
    how_to_play_button  -> signal_leave().connect( sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "how_to_play_button") );

    done_button   -> signal_clicked().connect( sigc::ptr_fun(&close_game));
    done_button  -> signal_enter().connect( sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "done_button") );
    done_button  -> signal_leave().connect( sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "done_button") );

    got_it_button -> signal_clicked().connect( sigc::ptr_fun(&close_instructions));
    got_it_button  -> signal_enter().connect( sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "got_it_button") );
    got_it_button  -> signal_leave().connect( sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "got_it_button") );

    reset_button  -> signal_clicked().connect( sigc::ptr_fun(&reset_board));
    reset_button  -> signal_leave().connect( sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "reset_button") );
    reset_button  -> signal_enter().connect( sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "reset_button") );

    finish_later_button  -> signal_leave().connect( sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "finish_later_button") );
    finish_later_button  -> signal_enter().connect( sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "finish_later_button") );

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

    // Add stylesheet to grids
    board_container_grid -> get_style_context() ->
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
