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
using namespace std;

#include "blockade.h"

// Command strings:
static const string str_move("move"), str_block("block"), str_turn("turn"), str_end("end"), str_nothing("nothing");

// Global information:
static int boardSize, myPID, player_x[2], player_y[2];

// Self-explanatory methods for extracting information from the "action" bit-hacked datastructure:
static const int moving = 0, blocking = 1, invalid = 2;
static int getActionType(action act);
static int getMoveDir(action act);
static int getBlockX(action act);
static int getBlockY(action act);


int main() {
	// Allow the client to set a name and colour:
	string player_name = getName();
	// Extract color components with bithax:
	color col = getColor();
	int red = (col>>16) & 255, green = (col>>8) & 255, blue = col & 255;

	// Limit the name length:
	int maxLen = min<int>(20, player_name.size());

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
	for(int id=0; id<2; ++id) cin >> player_x[id] >> player_y[id];

	newGame();


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
			setSquare(playerID, x, y);

		} else if (command == str_turn) {
			// Make a move:
			action turn = getMove();
			int actionType = getActionType(turn);

			if(actionType == moving) {
				cout << str_move << ' ' << getMoveDir(turn) << endl;
			} else if (actionType == blocking) {
				cout << str_block << ' ' << getBlockX(turn) << ' ' << getBlockY(turn) << endl;
			} else { // Invalid move:
				cout << str_nothing << endl;
			}

		} else { // command == str_end, or invalid command:
			break; // Exit
		}
	}

	return EXIT_SUCCESS;
}


// Converts RGB components into a single integer:
color makeColor(int r, int g, int b) {
	return (r<<16) + (g<<8) + b;
}

// Converts a direction to an integer in (-1, -2, -3, -4):
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

// Extracts the movement direction, mapping (-1,-2,-3,-4) to (0,1,2,3):
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
