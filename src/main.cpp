/*
* Last Modified: 01/19/21
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


// Function prototypes
void open_game ();
void close_game (bool);
void new_game ();
void new_from_main_menu ();

bool quit (GdkEventAny*, Glib::RefPtr<Gtk::Application>);
void quit_wrapper (Gtk::Window*);

void open_instructions ();
void close_instructions ();

void initialize_board ();
void populate_board ();

void reset_all ();
void reset_board ();
void reset_reserved ();

void set_pointer (Glib::ustring);
void restore_pointer (Glib::ustring);

bool check_if_number (char, Glib::RefPtr< Gtk::EntryBuffer >);
void insert_to_board (char, int, int);

void on_inserted (guint, const char*, guint, Glib::RefPtr< Gtk::EntryBuffer >, int, int);
void on_removed (guint, guint, int, int);

void check_win ();

void open_sorry ();
void close_sorry ();

void open_congratulations ();
void close_congratulations ();

bool timeout_handler ();

void handle_user ();

void update_main_menu ();
void switch_stack_page (Glib::ustring);
void hide_dialog (Glib::ustring);

// Global references
Glib::RefPtr<Gtk::Builder> builder;
Board board;


/************************
*  User-defined functions
************************/

// Opens the game window for single-board games. Gets users name
void
open_game (void)
{
	Gtk::Grid* board_container_grid;
	Gtk::Label* current_time_time_label;

	builder -> get_widget ("board_container_grid", board_container_grid);
	builder -> get_widget ("current_time_time_label", current_time_time_label);

	// Show game board
	board_container_grid -> show();
	switch_stack_page("Game Board");

	// Set correct time
	Glib::ustring game_time = board.formatted_time( board.get_total_time());
	current_time_time_label -> set_text( game_time);

	board.generate_reserved(); // Generate reserved if not already set
	populate_board();		   // Update GUI to match internal board state

	// Update game time every second
    Glib::signal_timeout().connect_seconds(  // Updates counter in game screen
      sigc::ptr_fun(&timeout_handler), 1
    );

	board.start(); // Start internal clock
	return;
}

// Closes the game window and saves the time if finish later was clicked
void
close_game (bool game_in_progress)
{
	if (game_in_progress) {
		board.set_total_time();
		board.save_data();  // Save in case switching user
	} else hide_dialog("congratulations_dialog");

	Gtk::Grid* board_container_grid;
	builder -> get_widget ("board_container_grid", board_container_grid);

	update_main_menu();

	board_container_grid -> hide();
    switch_stack_page("Main Menu");

	return;
}

// Resets the board time and the main menu for the current user. Called when
// New Game is clicked in the main menu
void
new_from_main_menu (void)
{
	reset_all();
	board.save_data();
	update_main_menu();
}

// Jumps straight into a new game after winning a previous one
void
new_game (void)
{
	reset_all();
	hide_dialog("congratulations_dialog");
	open_game();
}

// Called from close_game and handle_user to update the buttons on the main menu
// based on whether or not a user has a paused game
void
update_main_menu (void)
{
	Gtk::Box* menu_screen_box;
	Gtk::Button* begin_button;
	Gtk::Button* new_game_button;
	Gtk::Widget* new_game_button_box;
	Gtk::Widget* how_to_play_button_box;
	Gtk::Box* button_box_box;

	builder -> get_widget ("menu_screen_box", menu_screen_box);
	builder -> get_widget ("begin_button", begin_button);
	builder -> get_widget ("new_game_button_box", new_game_button_box);
	builder -> get_widget ("how_to_play_button_box", how_to_play_button_box);
	builder -> get_widget ("button_box_box", button_box_box);
	builder -> get_widget ("new_game_button", new_game_button);

	// Change begin to resume, and add new game button, if there is a game in
	// progress
	if ( board.get_total_time() != 0) {
		begin_button -> set_label("Resume Game");
		button_box_box -> reorder_child(*how_to_play_button_box, 2);
		button_box_box -> reorder_child(*new_game_button_box, 1);
		new_game_button -> show();
	} else {
		begin_button -> set_label("Begin");
		new_game_button -> hide();
		button_box_box -> reorder_child(*how_to_play_button_box, 1);
	}

	return;
}

// Callback used when the "X" is hit on the main menu window. This ensures that
// the users data is saved, and then calls quit on the application to quit the game
bool
quit (GdkEventAny* event, Glib::RefPtr<Gtk::Application> app)
{
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

// Customize TextView for each cell on the board
void
initialize_board (void)
{
    // Create CssProvider
    auto css_provider = Gtk::CssProvider::create();
    css_provider -> load_from_path ("res/styles.css");

	int i, j;
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
                	sigc::ptr_fun(&on_inserted), buffer, i, j
				)
			);

			cell -> get_buffer() -> signal_deleted_text().connect(
				sigc::bind(
					sigc::ptr_fun(&on_removed), i, j
				)
			);
        }
    }

	return;
}

// Update GUI to match internal board state
void
populate_board (void)
{
	Gtk::Entry* cell;

	int i, j;
	for (i=0; i<9; i++) {
		std::string iStr = std::to_string(i);

		for (j=0; j<9; j++) {
			std::string jStr = std::to_string(j);

			// Used to ensure the GUI matches the internal state
			std::string cell_value = std::to_string( board.get_number(i, j));
			std::string cell_name = "row_" + iStr + "_" + jStr;
			bool reserved_cell = board.check_reserved(i, j);

			builder -> get_widget(cell_name, cell);

			if ( cell_value.compare("0") != 0) cell -> set_text(cell_value);

			if (reserved_cell) {
				cell -> set_editable(false);
				cell -> get_style_context() -> add_class("reserved");

			} else {
				cell -> set_editable(true);
				cell -> get_style_context() -> remove_class("reserved");
			}

		}
	}
}

// Set every entry back to blank when reset button is hit. Excludes buttons set by the game.
void
reset_board (void)
{
    int i, j;
    for (i=0; i<9; i++) {
        for (j=0; j<9; j++) {
			if ( !board.check_reserved(i, j) ) {
				// ID of Gtk::Entry based on position in matrix
	            gchar* cell_name = (gchar *) g_malloc(31);
	            g_snprintf(cell_name, 31, "row_%d_%d", i, j);

	            // Get current Gtk::Entry from the builder
	            Gtk::Entry*  cell;
	            builder -> get_widget (cell_name, cell);

	            // Clear this Gtk::Entry's text
	            cell -> set_text("");

				board.set_number(0, i, j);
			}
        }
   }

   return;
}

// Erases the reserved cells in the gui only
void
reset_reserved (void)
{
	std::vector< std::vector< int>> reserved = board.get_reserved();
	for (auto i : reserved) {
		gchar* cell_name = (gchar *) g_malloc(31);
		g_snprintf(cell_name, 31, "row_%d_%d", i[0], i[1]);

		Gtk::Entry* cell;
		builder -> get_widget (cell_name, cell);

		cell -> set_editable(true);
		cell -> set_text("");

		board.set_number(0, i[0], i[1]);
	}
}

// Resets the gui and internal board state
void
reset_all (void)
{
	reset_board();     // Resets external playing board
	reset_reserved();  // Clears reserved cells from GUI
	board.reset();
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

// Call this every time a number is erased from the board
void
on_removed (guint position, guint n_chars, int outer, int inner)
{
	insert_to_board('0', outer, inner);
}

// Check if a board is a win. If it is, check if this is the fastest score and
// show a congratulatory dialog. If it is not, show a dialog saying it's not, and
// give the option to continue
void
check_win (void)
{
	board.set_checking_win(true);  // Stop time while performing checks

	bool winner = board.is_win();

	if (winner) {
		bool new_record = board.new_record();  // Check to see if new record
		board.reset_time();  // Reset time since not in a paused game

		if (new_record) {
			Gtk::Label *fastest_time_time_label;
			builder -> get_widget ("fastest_time_time_label", fastest_time_time_label);
			fastest_time_time_label -> set_text( board.get_fastest_time());

		}

		open_congratulations();

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

void
open_congratulations (void)
{
	Gtk::Dialog* congratulations_dialog;
	builder -> get_widget ("congratulations_dialog", congratulations_dialog);
	congratulations_dialog -> show();
}

void
close_congratulations (void)
{
	Gtk::Dialog* congratulations_dialog;
	builder -> get_widget ("congratulations_dialog", congratulations_dialog);
	congratulations_dialog -> hide();
}

// Wrapper for boards timeout handler to match handler for Glib::signal_timeout
bool
timeout_handler (void)
{
	if ( board.get_checking_win()) return true;

	Gtk::Label* current_time_time_label;
	builder -> get_widget ("current_time_time_label", current_time_time_label);

	current_time_time_label -> set_text( board.timeout_handler_helper());
	return true;
}

// Called when the "lets go" button is clicked after entering a username
void
handle_user (void)
{
	Gtk::Entry* username_entry;
	Gtk::Label* welcome_label;
	Gtk::Label* fastest_time_time_label;

	builder -> get_widget ("username_entry", username_entry);
	builder -> get_widget ("welcome_label", welcome_label);
	builder -> get_widget ("fastest_time_time_label", fastest_time_time_label);

	// User must enter a name to continue
	if ( !username_entry -> get_text_length()) {
		return;
	}

	// Set username in board. Set fastest time
	Glib::ustring username = username_entry -> get_text();
	board.set_username(username);

	Glib::ustring user_fastest = board.get_fastest_time();
	fastest_time_time_label -> set_text(user_fastest);

	// Set welcome message on main menu
	gchar* welcome_message = (gchar *) g_malloc(120);
	g_snprintf(welcome_message, 120,"Welcome, %s!", username.c_str());
	welcome_label -> set_text(welcome_message);

	update_main_menu();
	switch_stack_page("Main Menu");

	return;
}

// Called to switch the stack page
void
switch_stack_page (Glib::ustring page)
{
	Gtk::Stack* application_stack;
	builder -> get_widget ("application_stack", application_stack);
	application_stack -> set_visible_child(page);
	return;
}

// Called from the exit button in the winning dialog
void
quit_wrapper (Gtk::Window* window)
{
	window -> close();
	return;
}

// Hide the dialog whose name is given as the argument
void
hide_dialog (Glib::ustring dialog)
{
	Gtk::Dialog* hide;
	builder -> get_widget (dialog, hide);
	hide -> hide();
	return;
}


/*
 *	TODO
 *		- Undo / redo
 *		- Color for reserved squares?
 */

int
main(int argc, char **argv)
{
    // Required. Initialize gtkmm, check command line arguments, initialize variables
    auto app = Gtk::Application::create (argc, argv, "alexeast.sudoku");

    /* Pointers for widgets to be loaded from builder
     *
     */

    // Window pointers
    Gtk::Window* window;

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
	Gtk::Button* new_game_button;
	Gtk::Button* switch_user_button;
	Gtk::Button* exit_button;
	Gtk::Button* main_menu_button;
	Gtk::Button* winning_new_game_button;

    // Grid pointers
    Gtk::Grid* board_container_grid;
	Gtk::Grid* player_info_grid;
	Gtk::Grid* menu_grid;

	// Entry pointers
	Gtk::Entry* username_entry;

	// Box pointers
	Gtk::Box* menu_screen_box;
	Gtk::Box* username_entry_box;
	Gtk::Box* button_box_box;

	// Stack pointers
	Gtk::Stack* application_stack;

	// Label pointers
	Gtk::Label* welcome_label;
	Gtk::Label* game_title_label;
	Gtk::Label* source_code_label;
	Gtk::Label* username_label;
	Gtk::Label* player_info_title_label;
	Gtk::Label* current_time_header_label;
	Gtk::Label* current_time_time_label;
	Gtk::Label* fastest_time_header_label;
	Gtk::Label* fastest_time_time_label;
	Gtk::Label* note_label;
	Gtk::Label* you_won_label;
	Gtk::Label* almost_there_label;

	// Dialog pointers
	Gtk::Dialog* sorry_dialog;
	Gtk::Dialog* congratulations_dialog;
	Gtk::Dialog* how_to_play_dialog;



    /* Create builder from Glade file. Load necessary widgets.
     *
     */

    // Window widgets
    builder  = Gtk::Builder::create_from_file ("res/GUI.glade");
    builder -> get_widget ("application_window", window);

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
	builder -> get_widget ("new_game_button", new_game_button);
	builder -> get_widget ("switch_user_button", switch_user_button);
	builder -> get_widget ("exit_button", exit_button);
	builder -> get_widget ("main_menu_button", main_menu_button);
	builder -> get_widget ("winning_new_game_button", winning_new_game_button);

    // Grid widgets
    builder -> get_widget ("board_container_grid", board_container_grid);
	builder -> get_widget ("player_info_grid", player_info_grid);
	builder -> get_widget ("menu_grid", menu_grid);

	// Entry widgets
	builder -> get_widget ("username_entry", username_entry);

	// Box widgets
	builder -> get_widget("menu_screen_box", menu_screen_box);
	builder -> get_widget("username_entry_box", username_entry_box);
	builder -> get_widget("button_box_box", button_box_box);

	// Stack widgets
	builder -> get_widget("application_stack", application_stack);

	// Label widgets
	builder -> get_widget("welcome_label", welcome_label);
	builder -> get_widget("game_title_label", game_title_label);
	builder -> get_widget("source_code_label", source_code_label);
	builder -> get_widget("username_label", username_label);
	builder -> get_widget("player_info_title_label", player_info_title_label);
	builder -> get_widget("current_time_header_label", current_time_header_label);
	builder -> get_widget("current_time_time_label", current_time_time_label);
	builder -> get_widget("fastest_time_header_label", fastest_time_header_label);
	builder -> get_widget("fastest_time_time_label", fastest_time_time_label);
	builder -> get_widget("note_label", note_label);
	builder -> get_widget("you_won_label", you_won_label);
	builder -> get_widget("almost_there_label", almost_there_label);

	// Dialog widgets
	builder -> get_widget("sorry_dialog", sorry_dialog);
	builder -> get_widget("how_to_play_dialog", how_to_play_dialog);
	builder -> get_widget("congratulations_dialog", congratulations_dialog);



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
	finish_later_button -> signal_clicked().connect(  // Save time and return to menu
		sigc::bind<bool>( sigc::ptr_fun(&close_game), true)
	);

	continue_button  -> signal_leave().connect(  // Cursor normal
      sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "continue_button")
    );
    continue_button  -> signal_enter().connect(  // Cursor clickable
      sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "continue_button")
    );
    continue_button -> signal_clicked().connect(  // Close 'almost there' dialog
      sigc::ptr_fun(&close_sorry)
    );
    continue_button -> signal_clicked().connect(  // Start time when back to game
      sigc::mem_fun(board, &Board::start)
    );

	hint_button  -> signal_leave().connect(  // Cursor normal
      sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "hint_button")
    );
    hint_button  -> signal_enter().connect(  // Cursor clickable
      sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "hint_button")
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

	new_game_button -> signal_enter().connect(  // Cursor clickable
		sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "new_game_button")
	);
	new_game_button -> signal_leave().connect(  // Cursor normal
		sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "new_game_button")
	);
	new_game_button -> signal_clicked().connect(  // Reset time and update main menu
		sigc::ptr_fun(&new_from_main_menu)
	);

	switch_user_button -> signal_enter().connect(  // Cursor clickable
		sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "switch_user_button")
	);
	switch_user_button -> signal_leave().connect(  // Cursor normal
		sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "switch_user_button")
	);
	switch_user_button -> signal_clicked().connect(  // Switch to player info page
		sigc::bind<Glib::ustring>( sigc::ptr_fun(&switch_stack_page), "Player Info")
	);
	switch_user_button -> signal_clicked().connect(  // Reset board state & GUI
		sigc::ptr_fun(&reset_all)
	);

	exit_button -> signal_enter().connect(  // Cursor clickable
		sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "exit_button")
	);
	exit_button -> signal_leave().connect(  // Cursor normal
		sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "exit_button")
	);
	exit_button -> signal_clicked().connect(  // Exit button quits game
		sigc::bind<Gtk::Window*>( sigc::ptr_fun(&quit_wrapper), window)
	);

	main_menu_button -> signal_enter().connect(  // Cursor clickable
		sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "main_menu_button")
	);
	main_menu_button -> signal_leave().connect(  // Cursor normal
		sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "main_menu_button")
	);
	main_menu_button -> signal_clicked().connect(  // Go to main menu
		sigc::bind<bool>( sigc::ptr_fun(&close_game), false)
	);

	winning_new_game_button -> signal_enter().connect(  // Cursor clickable
		sigc::bind<Glib::ustring>( sigc::ptr_fun(&set_pointer), "winning_new_game_button")
	);
	winning_new_game_button -> signal_leave().connect(  // Cursor normal
		sigc::bind<Glib::ustring>( sigc::ptr_fun(&restore_pointer), "winning_new_game_button")
	);
	winning_new_game_button -> signal_clicked().connect(
		sigc::ptr_fun(&new_game)
	);

	// Entry signals
	username_entry -> signal_activate().connect(
		// Hitting enter is the same as clicking lets go button
		sigc::ptr_fun(&handle_user)
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
	new_game_button -> get_style_context() ->
		add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	switch_user_button -> get_style_context() ->
		add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	exit_button -> get_style_context() ->
		add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	main_menu_button -> get_style_context() ->
		add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	winning_new_game_button -> get_style_context() ->
		add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	got_it_button -> get_style_context() ->
		add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	hint_button -> get_style_context() ->
		add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	continue_button -> get_style_context() ->
		add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Add stylesheet to windows
    window -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Add stylesheet to grids
    board_container_grid -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	player_info_grid -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	menu_grid -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

	// Add stylesheet to boxes
	menu_screen_box -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	username_entry_box -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	button_box_box -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

	// Add stylesheet to stacks
	application_stack -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

	// Add stylesheet to labels
	welcome_label -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	game_title_label -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	source_code_label -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	username_label -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	player_info_title_label -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	current_time_header_label -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	current_time_time_label -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	fastest_time_header_label -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	fastest_time_time_label -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	note_label -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	almost_there_label -> get_style_context() ->
		add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	you_won_label -> get_style_context() ->
		add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

	// Add stylesheet to dialogs
	sorry_dialog -> get_style_context() ->
	   add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	congratulations_dialog -> get_style_context() ->
         add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	how_to_play_dialog -> get_style_context() ->
        add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);



    /* Initial setup
     *
     */
	 initialize_board();

    if (window) {
		app -> run(*window);
	}

    delete window;

    return 0;
}
