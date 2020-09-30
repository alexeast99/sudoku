/* Last Modified: 09/30/2020
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
    return game_board.at(outer).at(inner);
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
  std::vector< std::vector<int> > sorted(9, std::vector<int>(9));

  for (i=0; i<9; i++) {

    // Remove duplicates and check size. Wrong size means duplicate, therefore invalid.
    std::vector<int> row = game_board.at(i);
    std::sort (row.begin(), row.end());
    row.erase( std::unique(row.begin(), row.end()), row.end());
    if (row.size() != 9)
      return false;

    // Create new vector with value at the j'th row at the i'th column and repeat above
    std::vector<int> column;
    for (j=0; j<9; j++) {
      column.push_back(game_board.at(j).at(i));
    }
    std::sort (column.begin(), column.end());
    column.erase( std::unique(row.begin(), row.end()), row.end());
    if (column.size() != 9)
      return false;
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
