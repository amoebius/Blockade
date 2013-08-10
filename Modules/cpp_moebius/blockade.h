#ifndef __BLOCKADE_H
#define __BLOCKADE_H

/* -------------------------------------------
 * Moebius Blockade Interface for C++
 * -------------------------------------------
 * Below are the descriptions of the functions provided by the interface and the functions that a bot must implement.
 * Following are prototypes and more in-depth description.
 * -------------------------------------------
 * Author:  James Payor (github.com/amoebius)
 * -------------------------------------------
 */

/* Functions provided by the module:
 *
 * int getMyID();       -  Returns the bot's player id (0 or 1).
 * int getTheirID();    -  Returns the other bot's player id (0 or 1).
 * int getBoardSize();  -  Returns the size of the board.
 * int getX(int id);    -  Returns the 'x' coordinate of the player with the specified id.
 * int getY(int id);    -  Returns the 'y' coordinate of the player with the specified id.
 *
 */

/* Helper functions provided by the module:
 *
 * color  makeColor(int red, int green, int blue);   -  Color from RGB components.
 * action step(int direction);                       -  'step' action from direction.
 * action block(int x, int y);                       -  'block' action from coordinates.
 *
 */

/* Functions to be implemented by the player's bot:
 *
 * string getName();                          -  Returns the bot's name.
 * color  getColor();                         -  Returns the bot's colour.
 * void   newGame();                          -  Initialises the bot at the beginning of a game.
 * void   setSquare(int pid, int x, int y);   -  Informs the bot that the square (x,y) has been blocked by the player 'pid'.
 * action getMove();                          -  Calling when it is the bot's turn to move, returns the move the bot will make.
 *
 */



// Constants:
#define UP                    0
#define RIGHT                 1
#define DOWN                  2
#define LEFT                  3

// Board Size Constraint:
#define MAX_SIZE              50

// Requried type definitions:
#include <string>
typedef int color;
typedef int action;



// Functions provided by the module:
//----------------------------------


// getMyID:
// Returns the bot's player id ('0' or '1').
int getMyID();

// getTheirID:
// Returns the opponent's player id ('0' or '1').
int getTheirID();

// getBoardSize:
// Returns the side-length of the square game board.
int getBoardSize();

// getX:
// Returns the 'x' coordinate of the player with the specified id.
int getX(int id);

// getY:
// Returns the 'y' coordinate of the player with the specified id.
int getY(int id);

// makeColor - helper function:
// Given red, green, and blue components of a colour (in the range 0-255 inclusive),
// returns a single integer representing the color:
color makeColor(int red, int green, int blue);

// step - helper function:
// Given a direction, returns the corresponding action to step one unit in that direction.
action step(int direction);

// block - helper function:
// Given a location, (x,y), returns the corresponding action to block that square.
action block(int x, int y);




// Functions to be implemented by the bot:
//----------------------------------------


// getName:
// Returns the bot's name, called once at the beginning of the game.
std::string getName();

// getColor:
// Returns the bot's color, called once at the beginning of the game.
// Must return a color creating by calling 'makeColor', e.g. "return makeColor(100,200,255);"
color getColor();

// newGame:
// Called once at the start of each game.  Use this routine to initialise your bot.
void newGame();

// setSquare:
// Called each time a player blocks a square (INCLUDING when you block a square).
// 'pid' specifies the player blocking the square, and (x,y) give you its coordinates.
void setSquare(int pid, int x, int y);

// getMove:
// Called when it is the bot's turn to move.
// The bot must return one of 'step(direction)', to move in the specified direction
// (UP, DOWN, LEFT, RIGHT), or 'block(x, y)' to block the square at coordinates (x,y).
action getMove();



#endif // __BLOCKADE_H
