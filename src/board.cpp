/* Last Modified: 09/19/2020
 * Author: Alex Eastman
 * Contact: alexeast@buffalo.edu
 * Summary: Definitions for function prototypes found in board.h
 */


#include "board.h"
#include "time.h"
#include "iostream"

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
    current_time = time(NULL);
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
    current_time = time(NULL);
    total_time = current_time - start_time;
    start_time = 0;  // Set start time to 0 to signify that a game is not open
}

Board::Board (void)
{
    start_time = 0;
}
