/* Last Modified: 12/21/2020
 * Author: Alex Eastman
 * Contact: alexeast@buffalo.edu
 * Summary: Definitions for function prototypes found in board.h . See board.h
 *          for explanation of functions
 */


#include "board.h"
#include <time.h>
#include <iostream>
#include <vector>
#include <algorithm>

// Initializes the board
Board::Board (void)
{
    start_time = 0;
    total_time = 0;
    fastest_time = 0; // This is retrieved from keyfile based on username
	username = "";
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

// TODO: this
bool Board::check_reserved (int outer, int inner)
{
    return false;
    // return (reserved[outer] == inner) ? true : false;
}

void Board::start (void)
{
  if (start_time != 0)  // Hitting begin while a game is open should not do anything
      return;
  time_t current_time;
  current_time = time(NULL);  // Gets the current time
  start_time = current_time;

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

	std::cout << "Total time: " << total_time << "\n";
	user_data.set_double(username, "paused_time", total_time);

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
	  user_data.set_double(username, "fastest_time", total_time);
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
	user_data.set_double(username, "paused_time", total_time);
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
		bool mid_game = user_data.has_key(name, "paused_time");

		if (has_fastest) {  // User has a fastest time
			fastest_time = user_data.get_double(name, "fastest_time");
		}
		if (mid_game) {  // If user is in the middle of a game
			total_time = user_data.get_double(name, "paused_time");
		}

	} else {
		user_data.set_double(name, "fastest_time", 0);
		user_data.set_double(name, "paused_time", 0);
	}
	return;
}

void Board::save_data (void)
{
	user_data.save_to_file("data/user_data.txt");
	return;
}
