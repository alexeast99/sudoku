/*
* Last Modified: 12/21/2020
* Author: Alex Eastman
* Contact: alexeast@buffalo.edu
* Summary: Board class with function prototypes and intializer.
*/

#include <vector>
#include <string>
#include <fstream>
#include <glibmm/ustring.h>
#include <glibmm/keyfile.h>

class Board {

  private:

    /* A 3 by 3 board where the outer array specifies the row, starting at 0, and
    *  the inner array specifies the column, also starting at 0. The value is
    *  the number in the square at this position on the board.
    */
    std::vector< std::vector<int> > game_board =
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

    /* Represents each 9-cell block on the board. Each inner vector is one block.
     * Blocks are numbered starting at 0 in the upper left, 3 directly below that,
     * and 6 directly below 3. This is used to simplify checking board validity.
     * Order in each block does not matter.
     */
    std::vector< std::vector<int> > blocks =
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

    /* A lookup table giving the index of the inner vector for the blocks
     * variable. Since we don't care about the exact position in the block,
     * having the blocks index is good enough
     */
     std::vector< std::vector<int> > blocksLookup =
      {
        {0, 1, 2},
        {3, 4, 5},
        {6, 7, 8}
      };

    /* Contains the positions of numbers that were set by the game, and are
    *  therefore un-changeable. The index is the position in the outer array,
    *  and the value at that index is the position in the inner array.
    *  Contains IF a space is reserved, not WHAT is there.
    */
    int reserved[10];

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

};
