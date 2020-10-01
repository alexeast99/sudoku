/* Last Modified: 10/01/2020
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
    // Can't use push back bc want to set specific position
    game_board[outer][inner] = number;

    // Don't care about position, place into correct block vector
    int block = blocksLookup.at(outer/3).at(inner/3);
    blocks.at(block).push_back(number);
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
    if (row.size() != 9 && std::find(row.begin(), row.end(), 0) == row.end())
      return false;

    std::sort (block.begin(), block.end());
    block.erase ( std::unique(block.begin(), block.end()), block.end());
    if (block.size() != 9 && std::find(block.begin(), block.end(), 0) == block.end())
      return false;

    std::sort (column.begin(), column.end());
    column.erase( std::unique(column.begin(), column.end()), column.end());
    if (column.size() != 9 && std::find(column.begin(), column.end(), 0) == column.end())
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
