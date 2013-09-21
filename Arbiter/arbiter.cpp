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
#include <string>
using namespace std;

#include "arbiter.hpp"
#include "iopipe/iopipe.hpp"
#include "ChildProcess.hpp"
#include "threading/threading.hpp"
#include "threading/atomic.hpp"

class Bot {



private:
	iopipe pipe;

}



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

	iopipe bot[2] = {ChildProcess::Create(programs[0]), ChildProcess::Create(programs[1])};




	return EXIT_SUCCESS;
}
