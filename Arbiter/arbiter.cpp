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
#include <ctime>
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

#include "arbiter.hpp"
#include "pipe/duopipe.hpp"
#include "ChildProcess.hpp"
#include "threading/threading.hpp"
#include "threading/atomic.hpp"

// For-loop macro - for i = 0 to n-1:
#define fo(i,n) for(int i=0, _n=(n); i < _n; ++i)

const int MAX_SIZE = 50;
bool blocked[MAX_SIZE][MAX_SIZE];
int board_size;

const int UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3;
const int dx[] = {0, 1, 0, -1}, dy[] = {-1, 0, 1, 0};

const string str_turn = "turn", str_move = "move", str_block = "block", str_nothing = "nothing";
const string directions = { string("up"), string("right"), string("down"), string("left") };

const bool canReach(int x, int y, int destination_y);

inline const bool outside(int x, int y) {
	return x < 0 || y > 0 || x >= board_size || y >= board_size;
}

int main(int argc, char* argv[]) {

	string programs[2];
	board_size = 25;
	if(argc == 2) {
		fo(i,2) programs[i] = argv[1];

	} else if(argc == 3) {
		fo(i,2) programs[i] = argv[i+1];

	} else if(argc == 4) {
		fo(i,2) programs[i] = argv[i+1];
		board_size = atoi(argv[3]);
		if(board_size < 5 || board_size > MAX_SIZE) {
			cerr << "Invalid board size.  Specified board size must be an integer between 5 and " << MAX_SIZE << endl;
		}

	} else {
		cerr << "Invalid number of arguments specified.  Usage:\n" << "arbiter first_bot_location [second_bot_location] [board_size = 25]" << endl;
		return EXIT_FAILURE;
	}


	srand(time(NULL));
	if(rand() % 2) swap(programs[0], programs[1]);

	ChildProcess bot[2] = { ChildProcess(programs[0]), ChildProcess(programs[1]) };
	int player_id[2], player_x[2], player_y[2];

	fo(i,2) player_x[i] = board_size / 2;
	player_y[0] = 0;
	player_y[1] = board_size - 1;

	int turn = 0;

	string names[2];
	fo(i,2) bot[i] >> names[i];

	int color_red[2], color_green[2], color_blue[2];
	fo(i,2) bot[i] >> color_red[i] >> color_green[i] >> color_blue[i];

	fo(i,2) bot[i] << board_size << ' ' << i << endl;
	fo(i,2) fo(j,2) bot[i] << player_x[j] << ' ' << player_y[j] << endl;

	cout << names[0] << " versus " << names[1] << endl;
	fo(i,2) cout << i << " RGB " << color_red[i] << ' ' << color_green[i] << ' ' << color_blue[i] << endl;

	bool game_running = true;
	int winner;
	while(game_running) {

		bot[turn] << str_turn << endl;

		string response;
		bot[turn] >> response;

		if(response == str_move) {

			int direction;
			bot[turn] >> direction;

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
				cout << turn << "Moved " << directions[direction] << " to " << x << ' ' << y << endl;
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
			}

		} else if(response == str_block) {

			int x, y;
			bot[turn] >> x >> y;

			if(blocked[y][x]) {
				cout << turn << " Invalid: Square (" << x << ", " << y << ") is already blocked." << endl;
				game_running = false;
				winner = 1 - turn;

			} else {
				blocked[y][x] = true;
				if(canReach(player_x[0], player_y[0], board_size-1) && canReach(player_x[1], player_y[1], 0)) {
					cout << turn << " Blocked " << x << ' ' << y << endl;
				} else {
					cout << turn << " Invalid: Blocking square (" << x << ", " << y << ") blocks the path of a player." << endl;
					game_running = false;
					winner = 1 - turn;
				}
			}

		} else if(response == str_nothing) {
			cout << turn << " Invalid: No move provided." << endl;
			game_running = false;
			winner = 1 - turn;

		} else {
			cout << "-1 Error: Invalid move response given - '" << response << "'." << endl;
			return EXIT_FAILURE;
		}

		turn = 1 - turn;
	}

	cout << turn << " Wins" << endl;
	return EXIT_SUCCESS;
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
