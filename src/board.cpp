/* Last Modified: 09/23/2020
 * Author: Alex Eastman
 * Contact: alexeast@buffalo.edu
 * Summary: Definitions for function prototypes found in board.h . See board.h
 *          for explanation of functions
 */


#include "board.h"
#include "time.h"
#include "iostream"

Board::Board (void)
{
    start_time = 0;
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

// TODO implement this.
bool Board::is_win (void)
{
  return true;
}
