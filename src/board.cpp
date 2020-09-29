/* Last Modified: 09/29/2020
 * Author: Alex Eastman
 * Contact: alexeast@buffalo.edu
 * Summary: Definitions for function prototypes found in board.h . See board.h
 *          for explanation of functions
 */


#include "board.h"
#include <time.h>
#include <iostream>
#include <vector>

Board::Board (void)
{
    start_time = 0;
    fastest_time = 0; // TODO: get this from external source
}

void Board::set_number (int number, int outer, int inner)
{
    game_board[outer][inner] = number;
}

int Board::get_number (int outer, int inner)
{
    return game_board[outer][inner];
}

bool Board::check_reserved (int outer, int inner)
{
    return (reserved[outer] == inner) ? true : false;
}

void Board::start (void)
{
    if (start_time != 0)  // Hitting begin while a game is open should not do anything
        return;
    time_t current_time;
    current_time = time(NULL);  // Gets the current time
    start_time = current_time;
    std::cout << "Starting at time: " << start_time << "\n";
}

long Board::get_total_time (void)
{
    return total_time;
}

void Board::set_total_time (void)
{
    time_t current_time;
    current_time = time(NULL);  // Gets the current time
    total_time = current_time - start_time;
    start_time = 0;  // Set start time to 0 to signify that a game is not open
}

bool Board::is_win (void)
{
  int i, j;
  int *row;
  std::vector< std::vector<int> > sorted(9, std::vector<int>(9));

  // Iterate over entries in game_board. Add entry to inner vector in sorted based
  // on entry's value. Row is used to reduce computation on inner for loop
  for (i=0; i<9; i++) {
    row = game_board[i];
    for (j=0; j<9; j++) {
      int current = row[j];
      sorted.at(current - 1).push_back(current);
    }
  }

  // There should be exactly 9 of every number on the board
  for (i=0; i<9; i++) {
    if (sorted.at(i).size() != 9) return false;
  }

  return true;
}

bool Board::new_record (void)
{
  bool record = total_time < fastest_time;
  if (record)
    fastest_time = total_time;
  return record;
}

long Board::get_fastest_time (void)
{
  return fastest_time;
}
