/* -------------------------------------------
 * Arbiter - arbiter.cpp
 * -------------------------------------------
 * Implements the Arbiter for 'Blockade' - given the locations of two bots
 * as commandline arguments, it will have the bots play against each other
 * and log the game to standard output.
 * -------------------------------------------
 * Author:  James Payor (github.com/amoebius)
 * -------------------------------------------
 */


// TODO:
//  - Remove the error messages for sending things to the bots.  The timeouts on reading should handle this.


#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "arbiter.hpp"
#include "pipe/duopipe.hpp"
#include "threading/threading.hpp"
#include "ChildProcess.hpp"
using namespace cpipe;

// For-loop macro - for i = 0 to n-1:
#define fo(i,n) for(int i=0, _n=(n); i < _n; ++i)


const string sandboxer = "sandbox";
const int MAX_INIT_TIME = 10000, MAX_TURN_TIME = 1500;

const int MIN_SIZE = 5, MAX_SIZE = 50, DEFAULT_SIZE = 25;
bool blocked[MAX_SIZE][MAX_SIZE];
int board_size;

const int UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3, NUM_DIRECTIONS = 4;
const int dx[] = {0, 1, 0, -1}, dy[] = {-1, 0, 1, 0};

const string HELP_PROMPT = "?";
const string str_turn = "turn", str_move = "move", str_block = "block", str_nothing = "nothing", str_error = "error", str_end = "end";
const string directions[] = { "up", "right", "down", "left" };


int main(int argc, char* argv[]) {

	string modules[2], programs[2];
	board_size = DEFAULT_SIZE;
	string logfiles[2];
	
	if (argc == 2 && argv[1] == HELP_PROMPT) {
		showHelp(cout);
		return EXIT_SUCCESS;
	}

	if (argc < 5 || argc == 7 || argc > 8) {
		cerr << "Invalid number of arguments specified." << endl;
		showUsage(cerr);
		cerr << "Run `arbiter " << HELP_PROMPT << "` for more options." << endl;
		return EXIT_FAILURE;
	}

	fo (i, 2) {
		modules[i] = argv[2*i + 1], programs[i] = argv[2*i + 2];
	}

	if (argc >= 6) {
		board_size = atoi(argv[5]);
		if (board_size < MIN_SIZE || board_size > MAX_SIZE) {
			cerr << "Invalid board size.  Specified board size must be an integer between " << MIN_SIZE << " and " << MAX_SIZE << "." << endl;
			return EXIT_FAILURE;
		}
	}

	if (argc > 6) {
		logfiles[0] = argv[6];
		logfiles[1] = argv[7];
	}


	// Spawn sandboxed child processes, with timeouts:
	ChildProcess bot[2];
	fo (i, 2) {
		bot[i] = ChildProcess(sandboxer, {sandboxer, modules[i], programs[i], logfiles[i]}, MAX_INIT_TIME);
	}

	int player_id[2], player_x[2], player_y[2];

	fo (i, 2) {
		player_x[i] = board_size / 2;
	}
	player_y[0] = 0;
	player_y[1] = board_size - 1;

	int turn = 0, winner;
	bool game_running = true, handshake_succeeded = false;
	
	string names[2];
	fo (i, 2) {
		if (!(bot[i] >> names[i])) {
			cout << i << " Error: Unable to read bot name." << endl;
			game_running = false;
			winner = 1 - i;
			break;
		} else if (!nameValid(names[i])) {
			cout << i << " Invalid: Name supplied by the bot was invalid." << endl;
			game_running = false;
			winner = 1 - i;
			break;
		}
	}

	
	int color_red[2], color_green[2], color_blue[2];
	
	if (game_running) {
		
		// Now that each bot has been initialised, update the amount of time we allow them on each read:
		fo (i, 2) {
			bot[i].set_timeout(MAX_TURN_TIME);
		}

		fo (i, 2) {
			if (!(bot[i] >> color_red[i] >> color_green[i] >> color_blue[i])) {
				cout << i << " Error: Unable to read bot color." << endl;
				game_running = false;
				winner = 1 - i;
				break;
			}
		}
	}

	
	if (game_running) {
		
		// Mark that we got to the stage of sending commands:
		handshake_succeeded = true;
		
		// Log global information:
		cout << names[0] << " versus " << names[1] << " size " << board_size << endl;
		fo (i, 2) {
			cout << i << " starts at " << player_x[i] << ' ' << player_y[i] << endl;
		}
		fo (i, 2) {
			cout << i << " RGB " << color_red[i] << ' ' << color_green[i] << ' ' << color_blue[i] << endl;
		}

		// Send initial info:
		fo (i, 2) {
			bot[i] << board_size << ' ' << i << endl;
			fo (j, 2) bot[i] << player_x[j] << ' ' << player_y[j] << endl;
		}

	}


	while (game_running) {

		bot[turn] << str_turn << endl;

		string response;
		if (bot[turn] >> response) {

			if (response == str_move) {

				int direction;
				if ((bot[turn] >> direction) && direction >= 0 && direction < NUM_DIRECTIONS) {

					int x = player_x[turn] + dx[direction], y = player_y[turn] + dy[direction];

					if (outside(x,y)) {
						cout << turn << " Invalid: Attempted to move " << directions[direction] << " off the board to (" << x << ", " << y << ")." << endl;
						game_running = false;
						winner = 1 - turn;

					} else if (blocked[y][x]) {
						cout << turn << " Invalid: Attempted to move " << directions[direction] << " into the blocked square at (" << x << ", " << y << ")." << endl;
						game_running = false;
						winner = 1 - turn;

					} else {
						cout << turn << " moved " << directions[direction] << " to " << x << ' ' << y << endl;
						if (turn == 1) {
							if (y == 0) {
								game_running = false;
								winner = 1;
							}
						} else {
							if (y == board_size - 1) {
								game_running = false;
								winner = 0;
							}
						}
						player_x[turn] = x;
						player_y[turn] = y;

						fo (i, 2) {
							bot[i] << str_move << ' ' << turn << ' ' << x << ' ' << y << endl;
						}
					}

				} else {
					cout << turn << " Error: Failed to read move direction or invalid direction given by bot." << endl;
					game_running = false;
					winner = 1 - turn;
				}

			} else if (response == str_block) {

				int x, y;
				if (bot[turn] >> x >> y) {

					if (outside(x,y)) {
						cout << turn << " Invalid: Tried to block square (" << x << ", " << y << "), which is not on the board." << endl;
						game_running = false;
						winner = 1 - turn;

					} else if (blocked[y][x]) {
						cout << turn << " Invalid: Tried to block square (" << x << ", " << y << "), which is already blocked." << endl;
						game_running = false;
						winner = 1 - turn;

					} else if ((x == player_x[0] && y == player_y[0]) || (x == player_x[1] && y == player_y[1])) {
						cout << turn << " Invalid: Tried to block square (" << x << ", " << y << "), but a player is located in this square." << endl;
						game_running = false;
						winner = 1 - turn;
					} else {
						blocked[y][x] = true;
						if (canReach(player_x[0], player_y[0], board_size-1) && canReach(player_x[1], player_y[1], 0)) {
							cout << turn << " blocked " << x << ' ' << y << endl;
							fo (i, 2) {
								bot[i] << str_block << ' ' << turn << ' ' << x << ' ' << y << endl;
							}
						} else {
							cout << turn << " Invalid: Tried to block square (" << x << ", " << y << "), which blocks the path of a player." << endl;
							game_running = false;
							winner = 1 - turn;
						}
					}

				} else {
					cout << turn << " Error: Failed to read block position." << endl;
					game_running = false;
					winner = 1 - turn;
				}

			} else if (response == str_nothing) {
				cout << turn << " Invalid: No move provided." << endl;
				game_running = false;
				winner = 1 - turn;

			} else if (response == str_error) {
				cout << turn << " Error: Error in client." << endl;
				game_running = false;
				winner = 1 - turn;

			} else {
				cout << turn << " Error: Invalid move response given - '" << response << "'." << endl;
				game_running = false;
				winner = 1 - turn;			
			}

		} else {
			cout << turn << " Error: Unable to read bot's move response.  (Probably timed out.)" << endl;
			game_running = false;
			winner = 1 - turn;
		}

		turn = 1 - turn;
	}

	cout << winner << " wins" << endl;

	if (handshake_succeeded) {
		// Only send the "end" command if we got past the initial handshake:
		fo (i, 2) {
			bot[i] << str_end << endl;
		}
	}
	
	HT::Thread::Sleep(42); // Allow a modest amount of time for the processes to gracefully shutdown.
	
	// DIE:
	fo (i, 2) {
		bot[i].kill();
	}

	return EXIT_SUCCESS;
}



const bool nameValid(string name) {

	if (name.size() == 0 || name.size() > 20) {
		return false;
	}

	fo (i, name.size()) {

		if ((name[i] < 'a' || name[i] > 'z') &&
			(name[i] < 'A' || name[i] > 'Z') &&
			(name[i] < '0' || name[i] > '9') &&
			name[i] != '_') {

			return false;
		}
	}

	return true;

}


const bool outside(int x, int y) {
	return x < 0 || y < 0 || x >= board_size || y >= board_size;
}


bool seen[MAX_SIZE][MAX_SIZE];

const bool _canReach(int x, int y, int to) {
	if (outside(x,y) || seen[y][x] || blocked[y][x]) {
		return false;
	}
	if (y == to) {
		return true;
	}
	seen[y][x] = true;
	fo (i, NUM_DIRECTIONS) {
		if (_canReach(x + dx[i], y + dy[i], to)) {
			return true;
		}
	}
	return false;
}

const bool canReach(int x, int y, int destination_y) {
	fo (i, board_size) {
		fo (j, board_size) {
			seen[i][j] = false;
		}
	}
	return _canReach(x, y, destination_y);
}


void showUsage(ostream& stream) {
	stream << "Usage:  arbiter first_module first_bot second_module second_bot [board_size=" << DEFAULT_SIZE << " [first_bot_logfile second_bot_logfile]]" << endl;
}

void showHelp(ostream& stream) {
	showUsage(stream);
	stream << endl;
	stream << "first_module:       The name of the module to use for the first bot." << endl;
	stream << "first_bot:          The path of the first bot's source file." << endl;
	stream << endl;
	stream << "second_module:      The name of the module to use for the second bot." << endl;
	stream << "second_bot:         The path of the second bot's source file." << endl;
	stream << endl;
	stream << "board_size:         The size of the board to use (defaults to " << DEFAULT_SIZE << ")." << endl;
	stream << endl;
	stream << "first_bot_logfile:  The path of a file to log the output of the first bot to." << endl;
	stream << "second_bot_logfile: The path of a file to log the outupt of the second bot to." << endl;
	stream << endl;
}
