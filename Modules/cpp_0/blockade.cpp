/* -------------------------------------------
 * Moebius Blockade Interface for C++
 * -------------------------------------------
 * Implements the interface between the Arbiter and client as specified by the 'ArbiterProtocol' and
 * 'blockade.h' files for the 'Blockade' game.
 * -------------------------------------------
 * Author:  James Payor (github.com/amoebius)
 * -------------------------------------------
 */

#include <iostream>
#include <algorithm>
#include <string>
#include <cctype>
#include <cstddef>
#include <cstdlib>
using namespace std;

#include "blockade.hpp"
#include "ioblock.hpp"

// Command strings:
static const string str_move("move"), str_block("block"), str_turn("turn"), str_end("end"), str_nothing("nothing"), str_error("error");

// Global information:
static int boardSize, myPID, player_x[2], player_y[2];

// Self-explanatory methods for extracting information from the "action" bit-hacked datastructure:
static const int moving = 0, blocking = 1, invalid = 2;
static int getActionType(action act);
static int getMoveDir(action act);
static int getBlockX(action act);
static int getBlockY(action act);
static int getRed(color col);
static int getGreen(color col);
static int getBlue(color col);
static void clientAssert(bool condition, char * message);

int main() {

	// Initialise io blocking:
	ioblock::init();

	ioblock::block();
	// Allow the client to set a name and colour:
	string player_name = getName();
	// Extract color components with bithax:
	color col = getColor();
	ioblock::unblock();

	int red = getRed(col), green = getGreen(col), blue = getBlue(col);

	// Limit the name length:
	int maxLen = min<int>(16, player_name.size());

	// Search for invalid characters, and if they exist, truncate the name:
	for(int i=0; i<maxLen; ++i) {
		char test = player_name[i];
		if(!isalnum(test) && test != '_') {
			// Invalid character, truncate:
			maxLen = i;
			break;
		}
	}

	// Send these values to the Arbiter:
	if(maxLen) cout << player_name.substr(0,maxLen) << endl;
	else cout << "Bot" << endl;
	cout << red << ' ' << green << ' ' << blue << endl;

	// Read initial values from the Arbiter, and initialise the client:
	cin >> boardSize >> myPID;
	clientAssert(boardSize > 0, "Board size <= 0");
	clientAssert(myPID >= 0 && myPID < 2, "PID is insane");
	for(int id=0; id<2; ++id) cin >> player_x[id] >> player_y[id];

	ioblock::block();
	newGame();
	ioblock::unblock();


	// Main command loop:
	while (true) {

		// Read the command:
		string command;
		cin >> command;

		if (command == str_move) {
			
			// Update the player position:
			int playerID, x, y;
			cin >> playerID;
			cin >> player_x[playerID] >> player_y[playerID];

		} else if (command == str_block) {
			
			// Block the square:
			int playerID, x, y;
			cin >> playerID >> x >> y;
			
			ioblock::block();
			setSquare(playerID, x, y);
			ioblock::unblock();

		} else if (command == str_turn) {
			
			// Make a move:
			ioblock::block();
			action turn = getMove();
			ioblock::unblock();

			int actionType = getActionType(turn);

			if(actionType == moving) {
				cout << str_move << ' ' << getMoveDir(turn) << endl;
			} else if (actionType == blocking) {
				cout << str_block << ' ' << getBlockX(turn) << ' ' << getBlockY(turn) << endl;
			} else { // Invalid move:
				cout << str_nothing << endl;
			}

		} else if (command == str_end) {
			
			break; // Exit

		} else {

			// Invalid command.
			cout << str_error << endl;
			cerr << "Invalid command '" << command << "' received.  Aborting." << endl;
			return EXIT_FAILURE;

		}
	}

	return EXIT_SUCCESS;
}


// Converts RGB components into a single integer:
color makeColor(int r, int g, int b) {
	return (r<<16) + (g<<8) + b;
}

// Gets the 'red' component of the given color:
static int getRed(color col) {
	return (col>>16) & 255;
}
// Gets the 'green' component of the given color:
static int getGreen(color col) {
	return (col>>8) & 255;
}
// Gets the 'blue' component of the given color:
static int getBlue(color col) {
	return col & 255;
}


// Converts a direction to an integer in {-1, -2, -3, -4}:
action step(int dir) {
	return -1 - dir;
}

// Converts the coordinates into a postive integer:
action block(int x, int y) {
	return (x<<16) + y;
}

// Returns the type of action the number specifies:
static int getActionType(action act) {
	if(act < 0) return moving;
	else return blocking;
}

// Extracts the movement direction, mapping [-1, -4] to [0, 4):
static int getMoveDir(action act) {
	return -1 - act;
}

// Extracts the x coordinate - the higher 16 bits of the action:
static int getBlockX(action act) {
	return act>>16;
}
// Extracts the y coordinate - the lower 16 bits of the action:
static int getBlockY(action act) {
	return act & ((1<<16) - 1);
}


// Returns the player's ID:
int getMyID() {
	return myPID;
}
// Returns the opponent's ID:
int getTheirID() {
	return 1 - myPID;
}

// Returns the board size:
int getBoardSize() {
	return boardSize;
}

// Returns the specified player's x coordinate:
int getX(int id) {
	return player_x[id];
}
// Returns the specified player's y coordinate:
int getY(int id) {
	return player_y[id];
}


void clientAssert(bool condition, char * message) {
	if(!condition) {
		ioblock::unblock();
		cerr << "Error in client: " << message << endl;
		exit(EXIT_FAILURE);
	}
}


