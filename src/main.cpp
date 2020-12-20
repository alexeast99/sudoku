/*
* Last Modified: 12/20/20
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

bool quit (GdkEventAny*, Glib::RefPtr<Gtk::Application>);

void open_instructions ();
void close_instructions ();

void initalize_board ();
void reset_board ();

void set_pointer (Glib::ustring);
void restore_pointer (Glib::ustring);

bool check_if_number (char, Glib::RefPtr< Gtk::EntryBuffer >);
void insert_to_board (char, int, int);
void on_inserted (guint, const char*, guint, Glib::RefPtr< Gtk::EntryBuffer >, int, int);

void check_win ();

void open_sorry ();
void close_sorry ();

void open_congratulations ();
void close_congratulations ();

bool timeout_handler ();

bool check_for_user ();
void handle_user ();

// Global references
Glib::RefPtr<Gtk::Builder> builder;
Board board;


/************************
*  User-defined functions
************************/

// Opens the game window for single-board games. Gets users name
// TODO: Generate game board before displaying (i.e. initialize game state)
void
open_game (void)
{
	// Make sure there's a username before starting the game
	bool valid_user = check_for_user();
	if (!valid_user) {
		return;
	}

    Gtk::Window* game_window;
	builder -> get_widget ("game_window", game_window);
    if (game_window) {
		game_window -> show();
	}

    Glib::signal_timeout().connect_seconds(  // Updates counter in game screen
      sigc::ptr_fun(&timeout_handler), 1
    );

	return;
}

// Closes the game window for single-board games
// TODO: Reset board state after closing. Maybe different function?
void
close_game (void)
{
    Gtk::Window* game_window;
    builder -> get_widget ("game_window", game_window);
    board.set_total_time();
    if (game_window) {
		game_window -> hide();
	}

	return;
}

// Callback used when the "X" is hit on the main menu window. This ensures that
// the users data is saved, and then calls quit on the application to quit the game
bool
quit (GdkEventAny* event, Glib::RefPtr<Gtk::Application> app)
{
	board.save_data();
	app -> quit();
	return true;
}

// Open the 'How to Play' dialog
void
open_instructions (void)
{
    Gtk::Dialog* instructions_dialog;
    builder -> get_widget ("how_to_play_dialog", instructions_dialog);
    if (instructions_dialog) {
		instructions_dialog -> show();
	}

	return;
}

// Close the 'How to Play' dialog
void
close_instructions (void)
{
    Gtk::Dialog* instructions_dialog;
    builder -> get_widget ("how_to_play_dialog", instructions_dialog);
    if (instructions_dialog) {
		instructions_dialog -> hide();
	}

	return;
}

// Write the text buffer that holds the instructions for the 'How to Play' dialog
void
write_instructions (void)
{
    Gtk::TextView* instructions_text_view;
    Glib::RefPtr<Gtk::TextBuffer> instructions_text_buffer;

    builder -> get_widget ("how_to_play_text_view", instructions_text_view);
    if (instructions_text_view) {
		instructions_text_buffer = instructions_text_view -> get_buffer();
	}
    instructions_text_buffer -> set_text ("Success!");

	return;
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

    for (i=0; i<9; i++) {
        for (j=0; j<9; j++) {
            // ID of Gtk::Entry based on position in matrix
            gchar* cell_name = (gchar *) g_malloc(31);
            g_snprintf(cell_name, 31,"row_%d_%d", i, j);

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
                sigc::ptr_fun(&on_inserted), buffer, i, j));
        }
    }

	return;
}

// Set every entry back to blank when reset button is hit. Excludes buttons set by the game.
void
reset_board (void)
{
    int i, j;
    for (i=0; i<9; i++) {
        for (j=0; j<9 && !board.check_reserved(i, j); j++) {
            // ID of Gtk::Entry based on position in matrix
            gchar* cell_name = (gchar *) g_malloc(31);
            g_snprintf(cell_name, 31, "row_%d_%d", i, j);

            // Get current Gtk::Entry from the builder
            Gtk::Entry*  cell;
            builder -> get_widget (cell_name, cell);

            // Clear this Gtk::Entry's text
            cell -> set_text("");
        }
   }

   return;
}

// Set cursor to pointer when over button
void
set_pointer (Glib::ustring widget)
{
    auto clickable_cursor = Gdk::Cursor::create (Gdk::HAND2);
    Gtk::Button* button;
    builder -> get_widget (widget, button);
    button -> get_window() -> set_cursor (clickable_cursor);

	return;
}

// Set cursor to default when leaving button
void
restore_pointer (Glib::ustring widget)
{
    auto normal_cursor = Gdk::Cursor::create (Gdk::LEFT_PTR);
    Gtk::Button* button;
    builder -> get_widget (widget, button);
    button -> get_window() -> set_cursor (normal_cursor);

	return;
}

// Only allow numbers 1-9 to be entered on sudoku
bool
check_if_number (char inserted, Glib::RefPtr< Gtk::EntryBuffer > buffer)
{
  if (inserted > '9' || inserted < '1') {
    buffer -> delete_text(0, 1);
    return false;
  }
  return true;
}

// When text is entered, must store in internal board
void
insert_to_board (char inserted, int outer, int inner)
{
  board.set_number(inserted - '0', outer, inner);
  return;
}

// Call this every time a number is inserted on the board
void
on_inserted (guint position, const gchar* chars, guint n_chars,
  Glib::RefPtr< Gtk::EntryBuffer > buffer, int outer, int inner)
{
  char inserted = *chars;
  if ( check_if_number(inserted, buffer)) {
    insert_to_board (inserted, outer, inner);
  }

  return;
}

// Check if a board is a win. If it is, check if this is the fastest score and
// show a congratulatory dialog. If it is not, show a dialog saying it's not, and
// give the option to continue
void
check_win (void)
{
  board.set_total_time();  // Stop time while performing checks

  bool winner = board.is_win();

  if (winner) {
    bool new_record = board.new_record();

    if (new_record) {
      Gtk::Label *fastest_time_time_label;

      builder -> get_widget ("fastest_time_time_label", fastest_time_time_label);

      fastest_time_time_label -> set_text( board.get_fastest_time());
    }
    // TODO: show dialog saying they won. Username?

  } else {
    open_sorry ();  // Show dialog saying it was not a win
  }

  return;
}

void
open_sorry (void)
{
  Gtk::Dialog* sorry_dialog;
  builder -> get_widget ("sorry_dialog", sorry_dialog);
  if (sorry_dialog) {
  	sorry_dialog -> show();
  }

  return;
}

void
close_sorry (void)
{
  Gtk::Dialog* sorry_dialog;
  builder -> get_widget ("sorry_dialog", sorry_dialog);
  if (sorry_dialog) {
  	sorry_dialog -> hide();
  }

  return;
}

// Wrapper for boards timeout handler to match handler for Glib::signal_timeout
bool
timeout_handler (void)
{
  Gtk::Label* current_time_time_label;
  builder -> get_widget ("current_time_time_label", current_time_time_label);

  current_time_time_label -> set_text( board.timeout_handler_helper());
  return true;
}

// Checks to see if a username has been entered already. If it hasn't, it spawns
// the dialog to enter the username
bool
check_for_user (void)
{
	Gtk::Dialog* player_info_dialog;
	Glib::ustring user = board.get_username();
	builder -> get_widget ("player_info_dialog", player_info_dialog);

	if ( !user.length()) {  // If there is not a user set up yet
		int result = player_info_dialog -> run();  // Blocks for user input
		if (result == Gtk::RESPONSE_ACCEPT) {  // User entered name
			player_info_dialog -> hide();
			return true;
		}
		return false;  // Something went wrong or user didn't enter name
	}

	return true;  // User already entered name
}

// Called when the "lets go" button is clicked after entering a username
void
handle_user (void)
{
	Gtk::Entry* username_entry;
	Gtk::Dialog* player_info_dialog;
	Gtk::Label* welcome_label;
	Gtk::Label* fastest_time_time_label;

	builder -> get_widget ("username_entry", username_entry);
	builder -> get_widget ("player_info_dialog", player_info_dialog);
	builder -> get_widget ("welcome_label", welcome_label);
	builder -> get_widget ("fastest_time_time_label", fastest_time_time_label);

	if ( username_entry -> get_text_length()) {  // If user has entered a name

		// Set username in board and signal to dialog
		player_info_dialog -> response(Gtk::RESPONSE_ACCEPT);  // Signal to dialog
		Glib::ustring username = username_entry -> get_text();
		board.set_username(username);
		Glib::ustring user_fastest = board.get_fastest_time();
		fastest_time_time_label -> set_text(user_fastest);

		// Set welcome message on main menu
		gchar* welcome_message = (gchar *) g_malloc(120);
		g_snprintf(welcome_message, 120,"Welcome, %s!", username.c_str());
		welcome_label -> set_text(welcome_message);
	}

	return;
}


int
main(int argc, char **argv)
{
    // Required. Initialize gtkmm, check command line arguments, initialize variables
    auto app = Gtk::Application::create (argc, argv, "alexeast.sudoku");

    /* Pointers for widgets to be loaded from builder
     *
     */

    // Window pointers
    Gtk::Window* game_window;
    Gtk::Window* window;

    // Dialog pointers
    Gtk::Dialog* sorry_dialog;
	Gtk::Dialog* player_info_dialog;

    // Button pointers
    Gtk::Button* begin_button;
    Gtk::Button* done_button;
    Gtk::Button* how_to_play_button;
    Gtk::Button* got_it_button;
    Gtk::Button* reset_button;
    Gtk::Button* finish_later_button;
    Gtk::Button* hint_button;
    Gtk::Button* continue_button;
	Gtk::Button* lets_go_button;

    // Grid points
    Gtk::Grid* board_container_grid;

    /* Create builder from Glade file. Load necessary widgets.
     *
     */

    // Window widgets
    builder  = Gtk::Builder::create_from_file ("res/GUI.glade");
    builder -> get_widget ("application_window", window);
    builder -> get_widget ("game_window", game_window);

    // Dialog widgets
    builder -> get_widget ("sorry_dialog", sorry_dialog);
	builder -> get_widget ("player_info_dialog", player_info_dialog);

    // Button widgets
    builder -> get_widget ("begin_button", begin_button);
    builder -> get_widget ("done_button", done_button);
    builder -> get_widget ("how_to_play_button", how_to_play_button);
    builder -> get_widget ("got_it_button", got_it_button);
    builder -> get_widget ("reset_button", reset_button);
    builder -> get_widget ("finish_later_button", finish_later_button);
    builder -> get_widget ("hint_button", hint_button);
    builder -> get_widget ("continue_button", continue_button);
	builder -> get_widget ("lets_go_button", lets_go_button);

    // Grid widgets
    builder -> get_widget ("board_container_grid", board_container_grid);

    /* Connect signals. sigc::ptr_fun() creates a slot/function object/functor.
     * Helps with compatibility
     */

	// Window signals
	// Callback to ensure that user data is saved and application is properly
	// quit when user hits "X" on main menu window. App referenced is app that
	// was created at the top of this function
	window -> signal_delete_event().connect(
		sigc::bind<Glib::RefPtr<Gtk::Application>>( sigc::ptr_fun(&quit), app)
	);

    // Button signals
    begin_button  -> signal_clicked().connect(  // Begin button opens game
      sigc::ptr_fun(&open_game)
    );
    begin_button  -> signal_clicked().connect(  // Start internal clock
      sigc::mem_fun(board, &Board::start)
    );
    begin_button  -> signal_enter().connect(  // Cursor pointer
      sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "begin_button")
    );
    begin_button  -> signal_leave().connect(  // Cursor normal
      sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "begin_button")
    );

    how_to_play_button  -> signal_clicked().connect(  // Show instructions
      sigc::ptr_fun(&open_instructions)
    );
    how_to_play_button  -> signal_enter().connect(  // Cursor pointer
      sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "how_to_play_button")
    );
    how_to_play_button  -> signal_leave().connect(  // Cursor normal
      sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "how_to_play_button")
    );

    // Must pause time while checking game. Dialog to resume?
    done_button  -> signal_clicked().connect(   // Check if win on done
      sigc::ptr_fun(&check_win)
    );
    done_button  -> signal_enter().connect(  // Cursor pointer
      sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "done_button")
    );
    done_button  -> signal_leave().connect(  // Cursor normal
      sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "done_button")
    );

    got_it_button  -> signal_clicked().connect(  // Close instructions
      sigc::ptr_fun(&close_instructions)
    );
    got_it_button  -> signal_enter().connect(  // Cursor pointer
      sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "got_it_button")
    );
    got_it_button  -> signal_leave().connect(  // Cursor normal
      sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "got_it_button")
    );

    // Decision: Reset time or not?
    reset_button  -> signal_clicked().connect(  // Clear all entries on reset
      sigc::ptr_fun(&reset_board)
    );
    reset_button  -> signal_leave().connect(  // Cursor normal
      sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "reset_button")
    );
    reset_button  -> signal_enter().connect(  // Cursor clickable
      sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "reset_button")
    );

    finish_later_button  -> signal_leave().connect(  // Cursor normal
      sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "finish_later_button")
    );
    finish_later_button  -> signal_enter().connect(  // Cursor clickable
      sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "finish_later_button")
    );

    continue_button -> signal_clicked().connect(  // Close 'almost there' dialog
      sigc::ptr_fun(&close_sorry)
    );
    continue_button -> signal_clicked().connect(  // Start time when back to game
      sigc::mem_fun(board, &Board::start)
    );

	lets_go_button -> signal_clicked().connect(  // After username, main menu
		sigc::ptr_fun(&handle_user)
	);
	lets_go_button -> signal_enter().connect(  // Cursor clickable
		sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "lets_go_button")
	);
	lets_go_button -> signal_leave().connect(  // Cursor normal
		sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "lets_go_button")
	);

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
	lets_go_button -> get_style_context() ->
		add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Add stylesheet to windows
    window -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    game_window -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Add stylesheet to grids
    board_container_grid -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

	// Add stylesheet to dialogs
	player_info_dialog -> get_style_context() ->
		add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    /* Initial setup
     * TODO: Set fastest time on menu when game starts
     */

    write_instructions();
    initialize_board();

    if (window) {
		app -> run(*window);
	}


    delete window;

    return 0;
}
