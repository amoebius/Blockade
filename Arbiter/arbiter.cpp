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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "arbiter.hpp"
#include "pipe/duopipe.hpp"
#include "ChildProcess.hpp"

// For-loop macro - for i = 0 to n-1:
#define fo(i,n) for(int i=0, _n=(n); i < _n; ++i)


const int MIN_SIZE = 5, MAX_SIZE = 50, DEFAULT_SIZE = 25, MAX_TURN_TIME = 1500;
bool blocked[MAX_SIZE][MAX_SIZE];
int board_size;

const int UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3, NUM_DIRECTIONS = 4;
const int dx[] = {0, 1, 0, -1}, dy[] = {-1, 0, 1, 0};

const string HELP_PROMPT = "-h";
const string str_turn = "turn", str_move = "move", str_block = "block", str_nothing = "nothing", str_error = "error", str_end = "end";
const string directions[] = { "up", "right", "down", "left" };

int main(int argc, char* argv[]) {

	string programs[2];
	board_size = DEFAULT_SIZE;
	ofstream *logfiles[2] = {NULL, NULL};

	if (argc < 2 || argc > 6) {
		cerr << "Invalid number of arguments specified." << endl;
		showUsage(cerr);
		cerr << "Run `arbiter " << HELP_PROMPT << "` for more options." << endl;
		return EXIT_FAILURE;
	}

	if(argc == 2) {
		fo(i,2) programs[i] = argv[1];
		if(programs[0] == HELP_PROMPT) {
			showHelp(cout);
			return EXIT_SUCCESS;
		}

	} else {
		fo(i,2) programs[i] = argv[i+1];

		if(argc >= 4) {
			board_size = atoi(argv[3]);
			if(board_size < MIN_SIZE || board_size > MAX_SIZE) {
				cerr << "Invalid board size.  Specified board size must be an integer between " << MIN_SIZE << " and " << MAX_SIZE << endl;
				return EXIT_FAILURE;
			}
		}

		if(argc >= 5) {
			logfiles[0] = new ofstream(argv[4]);
			if(logfiles[0] == NULL || !logfiles[0]->is_open()) {
				cerr << "Unable to open '" << argv[4] << "' for writing.  Aborting." << endl;
				return EXIT_FAILURE;
			}
		}

		if(argc >= 6) {
			logfiles[1] = new ofstream(argv[5]);
			if(logfiles[1] == NULL || !logfiles[1]->is_open()) {
				cerr << "Unable to open '" << argv[5] << "' for writing.  Aborting." << endl;
				return EXIT_FAILURE;
			}

		}
	}

	ChildProcess bot[2] = { ChildProcess(programs[0], NULL, MAX_TURN_TIME), ChildProcess(programs[1], NULL, MAX_TURN_TIME) };
	int player_id[2], player_x[2], player_y[2];

	fo(i,2) player_x[i] = board_size / 2;
	player_y[0] = 0;
	player_y[1] = board_size - 1;

	int turn = 0, winner;
	bool game_running = true;

	string names[2];
	fo(i,2) if(!(bot[i] >> names[i])) {
		cout << i << " Error: Unable to read bot name." << endl;
		game_running = false;
		winner = 1 - i;
		break;
	}

	int color_red[2], color_green[2], color_blue[2];
	if(game_running) {
		fo(i,2) if(!(bot[i] >> color_red[i] >> color_green[i] >> color_blue[i])) {
			cout << i << " Error: Unable to read bot color." << endl;
			game_running = false;
			winner = 1 - i;
			break;
		}
	}

	if(game_running) {
		fo(i,2) bot[i] << board_size << ' ' << i << endl;
		fo(i,2) fo(j,2) bot[i] << player_x[j] << ' ' << player_y[j] << endl;

		cout << names[0] << " versus " << names[1] << " size " << board_size << endl;
		fo(i,2) cout << i << " starts at " << player_x[i] << ' ' << player_y[i] << endl;
		fo(i,2) cout << i << " RGB " << color_red[i] << ' ' << color_green[i] << ' ' << color_blue[i] << endl;

		fo(i,2) fo(j,2) bot[i] << str_move << ' ' << j << ' ' << player_x[j] << ' ' << player_y[j] << endl;
	}

	while(game_running) {

		bot[turn] << str_turn << endl;

		string response;
		if(bot[turn] >> response) {

			if(response == str_move) {

				int direction;
				if((bot[turn] >> direction) && direction >= 0 && direction < NUM_DIRECTIONS) {

					int x = player_x[turn] + dx[direction], y = player_y[turn] + dy[direction];

					if(outside(x,y)) {
						cout << turn << " Invalid: Attempted to move " << directions[direction] << " off the board to (" << x << ", " << y << ")." << endl;
						game_running = false;
						winner = 1 - turn;

					} else if(blocked[y][x]) {
						cout << turn << " Invalid: Attempted to move " << directions[direction] << " into the blocked square at (" << x << ", " << y << ")." << endl;
						game_running = false;
						winner = 1 - turn;

					} else {
						cout << turn << " moved " << directions[direction] << " to " << x << ' ' << y << endl;
						if(turn) {
							if(y == 0) {
								game_running = false;
								winner = 1;
							}
						} else {
							if(y == board_size - 1) {
								game_running = false;
								winner = 0;
							}
						}
						player_x[turn] = x;
						player_y[turn] = y;

						fo(i,2) bot[i] << str_move << ' ' << turn << ' ' << x << ' ' << y << endl;
					}

				} else {
					cout << turn << " Error: Failed to read move direction or invalid direction given by bot." << endl;
					game_running = false;
					winner = 1 - turn;
				}

			} else if(response == str_block) {

				int x, y;
				if(bot[turn] >> x >> y) {

					if(outside(x,y)) {
						cout << turn << " Invalid: Tried to block square (" << x << ", " << y << "), which is not on the board." << endl;
						game_running = false;
						winner = 1 - turn;

					} else if(blocked[y][x]) {
						cout << turn << " Invalid: Square (" << x << ", " << y << ") is already blocked." << endl;
						game_running = false;
						winner = 1 - turn;

					} else {
						blocked[y][x] = true;
						if(canReach(player_x[0], player_y[0], board_size-1) && canReach(player_x[1], player_y[1], 0)) {
							cout << turn << " blocked " << x << ' ' << y << endl;
							fo(i,2) bot[i] << str_block << ' ' << turn << ' ' << x << ' ' << y << endl;
						} else {
							cout << turn << " Invalid: Blocking square (" << x << ", " << y << ") blocks the path of a player." << endl;
							game_running = false;
							winner = 1 - turn;
						}
					}

				} else {
					cout << turn << " Error: Failed to read block position." << endl;
					game_running = false;
					winner = 1 - turn;
				}

			} else if(response == str_nothing) {
				cout << turn << " Invalid: No move provided." << endl;
				game_running = false;
				winner = 1 - turn;

			} else if(response == str_error) {
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

	fo(i,2) bot[i] << str_end << endl;

	fo(i,2) if(logfiles[i]) (*logfiles[i]) << ((istream&)bot[i].err()).rdbuf() << endl << "-- End of Log --" << endl;

	return EXIT_SUCCESS;
}


const bool outside(int x, int y) {
	return x < 0 || y < 0 || x >= board_size || y >= board_size;
}

bool seen[MAX_SIZE][MAX_SIZE];

const bool _canReach(int x, int y, int to) {
	if(y == to) return true;
	if(seen[y][x] || outside(x,y)) return false;
	seen[y][x] = true;
	fo(i,4) if(_canReach(x + dx[i], y + dy[i], to)) return true;
	return false;
}

const bool canReach(int x, int y, int destination_y) {
	fo(i, board_size) fo(j, board_size) seen[i][j] = false;
	return _canReach(x, y, destination_y);
}


void showUsage(ostream& stream) {
	stream << "Usage:\n" << "arbiter first_bot [second_bot = first_bot] [board_size = " << DEFAULT_SIZE << "] [first_bot_logfile] [second_bot_logfile]" << endl;
}

void showHelp(ostream& stream) {
	showUsage(stream);
	stream << "first_bot:          The relative or absolute path of the first bot executable." << endl;
	stream << "second_bot:         The relative or absolute path of the second bot executable." << endl;
	stream << "board_size:         The size of the board to use (defaults to " << DEFAULT_SIZE << ")." << endl;
	stream << "first_bot_logfile:  The relative or absolute path of a file to log the output of the first bot to." << endl;
	stream << "second_bot_logfile: The relative or absolute path of a file to log the outupt of the second bot to." << endl;
}
