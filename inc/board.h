/*
* Last Modified: 09/14/2020
* Author: Alex Eastman
* Contact: alexeast@buffalo.edu
* Summary: Board class with function prototypes and intializer. 
*/

using namespace std;

class Board {

  private:

    /* A 3 by 3 board where the outer array specifies the row, starting at 0, and
    *  the inner array specifies the column, also starting at 0. The value is
    *  the number in the square at this position on the board.
    */
    int game_board[9][9];

    /* Contains the positions of numbers that were set by the game, and are
    *  therefore un-changeable. The index is the position in the outer array,
    *  and the value at that index is the position in the inner array.
    *  Contains IF a space is reserved, not WHAT is there. 
    */
    int reserved[10];

    /*
     * The starting time.
     */
    long start_time;

    
    /* Total game time. On 'Finish Later', time is calculated and stored here.
     * Upon starting again, the same process repeats except that total time is
     * no longer zero.
     */
    long total_time;

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

    /*
     * Returns the number in total_time.
     */
    long get_total_time (void);

    /*
     * Calculates the time from start_time to the current time. Stores in total_time.
     */
    void set_total_time (void);

    /*
     * Stores the current time in start_time.
     */
    void start (void);
};
