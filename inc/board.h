/*
* Last Modified: 01/12/21
* Author: Alex Eastman
* Contact: alexeast@buffalo.edu
* Summary: Board class with function prototypes and intializer.
*/

#include <vector>
#include <string>
#include <fstream>
#include <glibmm/ustring.h>
#include <glibmm/keyfile.h>

#define RESERVED 7

class Board {

  private:

	  /* Used to reset the game_board and blocks variables
	   *
	   */
	   std::vector< std::vector< int>> initial =
		 { {0, 0, 0, 0, 0, 0, 0, 0, 0},
		   {0, 0, 0, 0, 0, 0, 0, 0, 0},
		   {0, 0, 0, 0, 0, 0, 0, 0, 0},
		   {0, 0, 0, 0, 0, 0, 0, 0, 0},
		   {0, 0, 0, 0, 0, 0, 0, 0, 0},
		   {0, 0, 0, 0, 0, 0, 0, 0, 0},
		   {0, 0, 0, 0, 0, 0, 0, 0, 0},
		   {0, 0, 0, 0, 0, 0, 0, 0, 0},
		   {0, 0, 0, 0, 0, 0, 0, 0, 0}
		 };

    /* A 3 by 3 board where the outer array specifies the row, starting at 0, and
    *  the inner array specifies the column, also starting at 0. The value is
    *  the number in the square at this position on the board.
    */
    std::vector< std::vector< int>> game_board = initial;

    /* Represents each 9-cell block on the board. Each inner vector is one block.
     * Blocks are numbered starting at 0 in the upper left, 3 directly below that,
     * and 6 directly below 3. This is used to simplify checking board validity.
     * Order in each block does not matter.
     */
    std::vector< std::vector< int>> blocks = initial;

    /* A lookup table giving the index of the inner vector for the blocks
     * variable. Since we don't care about the exact position in the block,
     * having the blocks index is good enough
     */
     std::vector< std::vector< int>> blocksLookup =
      {
        {0, 1, 2},
        {3, 4, 5},
        {6, 7, 8}
      };

    /* Contains subarrays where the first two values are the x and y, or row and
	 * column, coordinates of reserved cells, and the third is the block that
	 * those coordinates are located in
     */
    std::vector< std::vector< int>> reserved;
	/* Whether or not the reserved member variable was set. Faster than checking
	 * entries in reserved
	 */
	 bool reserved_set;

	 /* Holds the index of the next inner array to set the coordinates of a
	  * reserved cell into
	  */
	 int reserved_index;

    /* The starting time.
     *
     */
    long start_time;


    /* Total game time.
     *
     */
    double total_time;

    /* The fastest time for a game. This must rely on external factors
     *
     */
    double fastest_time;

	 /* A keyfile for reading and writing user data. Initialized in the
	  * instantiation of the board object. Each user has a group in the keyfile,
	  * and therefore only one keyfile is needed for all users.
	  */
	 Glib::KeyFile user_data;

	 /* The username of whoever is currently playing
	  *
	  */
	 Glib::ustring username;

	 /* Used to stop time while the board is being checked
	  *
	  */
	 bool checking_win;

	 /* Used to determine whether or not data *from the board only*
	  * should be loaded from the keyfile. True if resuming a paused game,
	  * false in all other cases
	  */
	 bool load_from_user_data;

  public:

    /* Initialize with start_time = 0 to signify a game has not yet been started.
     * This is used so users cannot click "Begin" again to reset the start time.
     */
    Board (void);

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

    /* Returns the number in total_time.
     *
     */
    long get_total_time (void);

    /* Calculates the time from start_time to the current time. Stores in total_time.
     *
     */
    void set_total_time (void);

    /* Stores the current time in start_time.
     *
     */
    void start (void);

    /* Check that the current board configuration is a valid win. This means that
     * no row or column has any digit 1-9 more than once.
     */
    bool is_win (void);

     /* Check if the current time is faster than the last fastest time. If it is,
      * return true and set the fastest time in the keyfile
      */
    bool new_record (void);

	/* Helper function to return a formatted time as XX Minutes YY Seconds. Accepts
     * the time in seconds
     */
     Glib::ustring formatted_time (double);

    /* Gets the fastest time. Used for setting the fastest time in the GUI.
     * Time returned as XX minutes XX seconds
     */
  	Glib::ustring get_fastest_time (void);

	/* Resets the time for starting a new game
	 *
	 */
	 void reset_time (void);

	/* Resets the board to all 0's when a new game is started
	 *
	 */
	 void reset_board (void);

	 /* Resets the reserved_index, reserved_set, and reserved member variables
	  *
	  */
	  void reset_reserved (void);

	 /* A convenient method to reset everything
	  *
	  */
	  void reset (void);

	/* Called from main.cpp in timeout_handler. Returns a time as XX Minutes
	 * YY Seconds. timeout_handler updates GUI
	 */
   	 Glib::ustring timeout_handler_helper (void);

	/* Get the current username
	 *
	 */
	 Glib::ustring get_username (void);

	 /* Set the username for this session
	  *
	  */
	 void set_username (Glib::ustring);

	 /* Save the data in the private keyfile variable to the text file. This is
	  * called when the game is exited.
	  */
	 void save_data (void);

	 /* Used to check if the time is being updated so that the clock on the game
	  * board doesn't get improperly updated
	  */
	 bool get_checking_win (void);

	 /* Used to set if a win is being checked so that the time is not updated
	  *
	  */
	 void set_checking_win (bool);

	 /* Used to save the state of the board to the user data file for the
	  * the current user
	  */
	 void save_board_state (void);

	 /* Used to load the state of the board from the user data file for the
	  * current user
	  */
	 void load_board_state (void);

	 /* Load the previously generated coordinates for reserved cells from the
	  * keyfile and into the reserved member variable
	  */
	  void set_reserved (void);

	 /* Generates coordinates of reserved cells. Ensures that there are at least
	  * RESERVED cells reserved (ie if the same coords are generated it doesn't
	  * matter)
	  */
	  void generate_reserved (void);

	  /* Generate numbers for each of the reserved cells. Ensure that the
	   * numbers and coordinates follow the rules of sudoku1
	   */
	   void populate_reserved (void);

	  /* Returns the reserved member variable. Not to be confused with
	   * check_reserved
	   */
	   std::vector< std::vector< int>> get_reserved (void);

};
