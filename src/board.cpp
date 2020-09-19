/* Last Modified: 09/19/2020
 * Author: Alex Eastman
 * Contact: alexeast@buffalo.edu
 * Summary: Definitions for function prototypes found in board.h
 */


#include "board.h"

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

