/* Last Modified: 01/17/21
 * Author: Alex Eastman
 * Contact: alexeast@buffalo.edu
 * Summary: Definitions for function prototypes found in board.h . See board.h
 *          for explanation of functions
 */


#include "board.h"
#include <time.h>
#include <iostream>
#include <algorithm>
#include <stdlib.h>

// Initializes the board
Board::Board (void)
{
    start_time = 0;
    total_time = 0;
    fastest_time = 0; // This is retrieved from keyfile based on username
	username = "";
	load_from_user_data = false;
	reserved_set = false;
	reserved.resize(RESERVED);

	int i;
	for (i=0; i<RESERVED; i++) {
		reserved[i] = {0, 0, 0};
	}

	user_data.load_from_file("data/user_data.txt");
}

void Board::set_number (int number, int outer, int inner)
{
    // Can't use push back bc want to set specific position
    game_board[outer][inner] = number;

    // Get the block to insert into. Get position within block to insert into
    int blockIndex = blocksLookup.at(outer/3).at(inner/3);
    int blockPosition = blocksLookup.at(outer%3).at(inner%3);

    // Insert into block
    blocks[blockIndex][blockPosition] = number;

	return;
}

int Board::get_number (int outer, int inner)
{
    return game_board.at(outer).at(inner);
}

bool Board::check_reserved (int outer, int inner)
{
	bool rsrv = false;

    int i;
	for (i=0; i<RESERVED; i++) {
		std::vector<int> coordinate = reserved[i];
		if (coordinate[0] == outer && coordinate[1] == inner) {
			rsrv = true;
			break;
		}
	}

	return rsrv;
}

void Board::start (void)
{
	if (start_time != 0)  // Hitting begin while a game is open should not do anything
		return;

	time_t current_time;
	current_time = time(NULL);  // Gets the current time
	start_time = current_time;
	checking_win = false;

	return;
}

long Board::get_total_time (void)
{
    return total_time;
}

void Board::set_total_time (void)
{
    time_t current_time;
    current_time = time(NULL);  // Gets the current time

	// Add old time if game was paused
    total_time = total_time + (current_time - start_time);
    start_time = 0;  // Set start time to 0 to signify that a game is not open

	return;
}

bool Board::is_win (void)
{
	int i, j;
	std::vector< std::vector<int> > sorted(9, std::vector<int>(9));

	for (i=0; i<9; i++) {

		std::vector<int> column;
    	std::vector<int> row = game_board.at(i);
    	std::vector<int> block = blocks.at(i);

    	// Populate column with values at i'th column across all rows
    	for (j=0; j<9; j++) {
			column.push_back(game_board.at(j).at(i));
		}

		// Remove duplicates and check size. Wrong size means duplicate, therefore invalid.
    	std::sort (row.begin(), row.end());
    	row.erase ( std::unique(row.begin(), row.end()), row.end());
    	if (row.size() != 9 && std::find(row.begin(), row.end(), 0) != row.end()) {
      		printf("Failed at row\n");
      		return false;
    	}

    	std::sort (block.begin(), block.end());
    	block.erase ( std::unique(block.begin(), block.end()), block.end());
    	if (block.size() != 9 && std::find(block.begin(), block.end(), 0) != block.end()) {
      		printf("Failed at block\n");
      		return false;
    	}

    	std::sort (column.begin(), column.end());
    	column.erase( std::unique(column.begin(), column.end()), column.end());
    	if (column.size() != 9 && std::find(column.begin(), column.end(), 0) != column.end()) {
      		printf("Failed at column\n");
      		return false;
    	}

  }

  return true;
}

bool Board::new_record (void)
{
	bool record = total_time < fastest_time;
	if (record || fastest_time == 0) {
		fastest_time = total_time;
  	}
	return record;
}

Glib::ustring Board::formatted_time (double t)
{
	// Time already in seconds. Get minutes then subtract to get seconds
	int minutes = t / 60;
	int seconds = t - (minutes * 60);

	Glib::ustring minutes_string;
	Glib::ustring seconds_string;

	if (seconds < 10)
    	seconds_string = "0" + std::to_string(seconds);
  	else
    	seconds_string = std::to_string(seconds);
  	minutes_string = std::to_string(minutes);

	return minutes_string + " Minutes " + seconds_string + " Seconds";

}

Glib::ustring Board::get_fastest_time (void)
{
	return formatted_time (fastest_time);
}

void Board::reset_time (void)
{
	total_time = 0;
	return;
}

void Board::reset_board (void)
{
	game_board = initial;
	blocks = initial;
	return;
}

void Board::reset_reserved (void)
{
	reserved_set = false;

	int i;
	for (i=0; i<RESERVED; i++) {
		reserved[i] = {0, 0, 0};
	}
}

void Board::reset (void)
{
	reset_time();
	reset_board();
	reset_reserved();

	// Reset will be called when a new game is requested. No matter what the
	// scenario is, board data should not be loaded from the keyfile when a new
	// game is requested
	load_from_user_data = false;

	return;
}

Glib::ustring Board::timeout_handler_helper (void)
{
	time_t current_time;
	current_time = time(NULL);
	return formatted_time (current_time - start_time + total_time);
}

Glib::ustring Board::get_username (void)
{
	return username;
}

void Board::set_username (Glib::ustring name)
{
	username = name;

	if (user_data.has_group(name)) {  // If this user has played before

		bool has_fastest = user_data.has_key(name, "fastest_time");
		bool has_paused = user_data.has_key(name, "paused_time");

		if (has_fastest) {  // User has a fastest time
			fastest_time = user_data.get_double(name, "fastest_time");
		}

		// If user is in the middle of a game
		if (has_paused && user_data.get_double(name, "paused_time") != 0) {
			total_time = user_data.get_double(name, "paused_time");
			load_from_user_data = true;
			set_reserved();
			load_board_state();

		}


	} else {
		user_data.set_double(name, "fastest_time", 0);
		user_data.set_double(name, "paused_time", 0);

	}

	return;
}

void Board::save_board_state (void)
{
	std::vector<int> game_row;
	std::vector<int> coordinate;
	std::string game_row_string = "";
	std::string coordinate_string = "";

	int i, j;
	for (i=0; i<9; i++) {
		// Save reserved coordinates
		if (i < RESERVED) {
			coordinate = reserved[i];
			coordinate_string.push_back(coordinate[0] + '0');  // +'0' to convert to char
			coordinate_string.push_back(coordinate[1] + '0');
			coordinate_string.push_back(coordinate[2] + '0');
			user_data.set_string(username, "r" + std::to_string(i), coordinate_string);
			coordinate_string.clear();
		}

		// Save i'th row of the game board
		game_row = game_board[i];
		for (j=0; j<9; j++) {
			char tile = game_row[j] + '0';
			game_row_string.push_back(tile);
		}
		user_data.set_string(username, std::to_string(i), game_row_string);
		game_row_string.clear();


	}

	return;
}

void Board::save_time (void)
{
	user_data.set_double(username, "paused_time", total_time);
	user_data.set_double(username, "fastest_time", fastest_time);
	return;
}

void Board::save_data (void)
{
	save_board_state();
	save_time();
	user_data.save_to_file("data/user_data.txt");
	return;
}

bool Board::get_checking_win (void)
{
	return checking_win;
}

void Board::set_checking_win (bool checking)
{
	set_total_time();  // Store the current game time internally
	checking_win = checking;
	return;
}

void Board::load_board_state (void)
{
	if (!load_from_user_data) return;

	std::string key;
	std::string row;

	int i, j;
	for (i=0; i<9; i++) {
		key = std::to_string(i);
		if (user_data.has_key(username, key)) {
			row = user_data.get_string(username, std::to_string(i));

			for (j=0; j<9; j++) {
				int tile = row[j] - '0';
				set_number(tile, i, j);
			}
		}
	}

	return;
}

void Board::set_reserved (void)
{
	// 'reserved' vector is stored exactly the same as the game board vector,
	// except that an 'r' is prepended to the number representing the row
	std::string key = "r";
	std::string row;

	int i;
	for (i=0; i<RESERVED; i++) {
		key = key + std::to_string(i);
		row = user_data.get_string(username, key);

		reserved[i][0] = row[0] - '0';
		reserved[i][1] = row[1] - '0';
		reserved[i][2] = row[2] - '0';

		key.pop_back();
	}

	reserved_set = true;
	return;
}

void Board::generate_reserved (void)
{
	// If this user already has reserved tiles, don't set
	if (reserved_set) return;

	srand( time(NULL));  // Initialize random seed

	int i;
	for (i=0; i<RESERVED; i++) {
		// Generate between 0 and 9
		int row = rand() % 10;
		int col = rand() % 10;

		// Must be between 0 and 8 since reserved has size 9
		while (row == 9) row = rand() % 10;
		while (col == 9) col = rand() % 10;

		if ( !check_reserved(row, col)) {
			reserved[i][0] = row;
			reserved[i][1] = col;
			reserved[i][2] = blocksLookup.at(row/3).at(col/3);
		}
		else i -= 1;

	}

	reserved_set = true;
	populate_reserved();
	return;
}

void Board::populate_reserved (void)
{
	srand( time(NULL)); // Initialize random seed

	// Breaks rules if same row as other reserved with same number or if same
	// column as other reserved with same number or if same block as other
	// reserved with same number

	// Two loops since we must compare each generated value with each previously
	// generated value

	int i, j;
	for (i=0; i<RESERVED; i++) {
		int value = rand() % 10;
		while (value == 0) value = rand() % 10;

		std::vector<int> coordinate = reserved[i];
		int x1 = coordinate[0];
		int y1 = coordinate[1];
		int z1 = coordinate[2]; // Block index

		for (j=0; j<RESERVED; j++) {
			if (i != j) {
				std::vector<int> compare = reserved[j];
				int x2 = compare[0];
				int y2 = compare[1];
				int z2 = compare[2];

				int comp_value = get_number(x2, y2);

				bool same_value = comp_value == value;
				bool same_row = x1 == x2;
				bool same_col = y1 == y2;
				bool same_block = z1 == z2;

				// Breaks rule, regenerate value
				if ((same_row || same_col || same_block) && same_value) {
					i -= 1;

				// Doesn't break rules, add to internal board state
				} else {
					set_number(value, x1, y1);
				}

			}
		}

	}
	if (username != "") save_data();
	return;
}

std::vector< std::vector< int>> Board::get_reserved (void)
{
	return reserved;
}
