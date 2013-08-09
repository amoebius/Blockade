//---------------
// Blockade.cpp |
//------------------------------------------------------------------
// Implements the interface between the client and the arbiter
// satisfying the "blockade.h" interface and the arbiter interface.
//------------------------------------------------------------------
// Author:  James Payor
//------------------------------------------------------------------

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

// Move information:
static const int nothing = 0, moving = 1, blocking = 2;
static int playerMove, moveDir, moveX, moveY;
static string errorMsg;

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
		if(!isalnum(test) && test != ' ' && test != '_') {
			// Invalid character, truncate:
			maxLen = i;
			break;
		}
	}

	// Send these values to the Arbiter:
	cout << player_name.substr(0,maxLen) << endl;
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
			playerMove = nothing;
			errorMsg = "No move was made by the client.";
			clientDoTurn();

			if(playerMove == moving) {
				cout << str_move << ' ' << moveDir << endl;

			} else if(playerMove == blocking) {
				cout << str_block << ' ' << moveX << ' ' << moveY << endl;

			} else { // No move was made - indicate this.
				cout << str_nothing << endl;
			}

		} else { // command == str_end, or invalid command:
			break; // Exit
		}
	}

	return EXIT_SUCCESS;
}

// Registration functions:
void setName(const char* name) {
	player_name = name;
}
void setColour(int r, int g, int b) {
	red = r;
	green = g;
	blue = b;
}

// Move-making functions:
void movePosition(int dir) {
	playerMove = moving;
	moveDir = dir;
}
void blockSquare(int x, int y) {
	playerMove = blocking;
	moveX = x;
	moveY = y;
}
