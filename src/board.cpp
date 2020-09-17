#include "include/board.h"

Board::set_number (int number, int outer, int inner)
{
  game_board[outer][inner] = number;
}

Board::get_number (int outer, int inner)
{
  return game_board[outer][inner];
}

Board::check_reserved (int outer, int inner)
{
  return (reserved[outer] == inner) ? true : false;
}

Board::set_game_type (bool type)
{
  game_type = type;
  if (type && game_board.size() != 3){
      game_board.resize(3);
      for (std::vector<int>::iterator it = game_board.begin();
           it != game_board.end(); ++it)
        {
          it.resize(3);
        }
  } else if (!type && game_board.size() != 9){
    game_board.resize(9);
    for (std::vector<int>::iterator it = game_board.begin();
         it != game_board.end(); ++it)
      {
        it.resize(9);
      }
  }
}

Board::get_game_type ()
{
  return game_type;
}
