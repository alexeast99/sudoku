/*
* Date: 09/14/2020
* Author: Alex Eastman
* Summary: Contains definition of board class for sudoku game. Boards are single-
*          unit, 3x3 collections of squares. A sudoku game can be made up of one
*          or 3 boards. Each square holds exactly one number.
*/

using namespace std;

class Board {

  private:

    /* A 3 by 3 board where the outer array specifies the row, starting at 0, and
    *  the inner array specifies the column, also starting at 0. The value is
    *  the number in the square at this position on the board.
    */
    std::vector< std::vector <int> >
      game_board(3);

    /* Contains the positions of numbers that were set by the game, and are
    *  therefore un-changeable. The index is the position in the outer array,
    *  and the value at that index is the position in the inner array.
    */
    std::vector< int >
      reserved(9);

  public:

    /* (number, outer, inner) Sets number in this board where outer specifies the
    *  position in the outer array and inner specifies the position in the inner
    *  array. Returns void.
    */
    void set_number (int, int, int);

    /* (outer, inner) Gets number where outer specifies the position in the outer
    *  array and inner specifies the position in the inner array. Returns the
    *  number at the specified position.
    */
    int get_number (int, int);

    /* (outer, inner) References index outer in reserved, and compares inner to
    *  the value retrieved. If inner == value, this is a reserved position. Returns
    *  true if the position is reserved, false otherwise.
    */
    bool check_reserved (int, int);
}
