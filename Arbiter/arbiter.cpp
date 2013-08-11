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

#include <cstddef>
#include <iostream>
#include <string>
using namespace std;

#include "arbiter.hpp"
#include "iopipe.hpp"


int main(int argc, char* argv[]) {

	string programs[2];
	if(argc == 1) {
		programs[0] = programs[1] = argv[0];
	} else if(argc == 2) {
		for(int i=0; i<2; ++i) programs[i] = argv[i];
	} else {
		cerr << "Invalid number of arguments specified.  Usage:\n" << "arbiter first_bot_location [second_bot_location]\n";
		return EXIT_FAILURE;
	}

	int pipes[2][2][2];



	return EXIT_SUCCESS;
}
