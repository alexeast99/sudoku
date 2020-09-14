#include "include/board.h"

Board::set_number (int number, int outer, int inner) {
  game_board[outer][inner] = number;
}

Board::get_number (int outer, int inner) {
  return game_board[outer][inner];
}

Board::check_reserved (int outer, int inner) {
  return (reserved[outer] == inner) ? true : false;
}
