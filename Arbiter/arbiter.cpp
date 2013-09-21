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
#include "pipe/duopipe.hpp"
#include "ChildProcess.hpp"
#include "threading/threading.hpp"
#include "threading/atomic.hpp"


int main(int argc, char* argv[]) {

	string programs[2];
	if(argc == 2) {
		programs[0] = programs[1] = argv[1];
	} else if(argc == 3) {
		for(int i=0; i<2; ++i) programs[i] = argv[i+1];
	} else {
		cerr << "Invalid number of arguments specified.  Usage:\n" << "arbiter first_bot_location [second_bot_location]\n";
		return EXIT_FAILURE;
	}

	iopipe p;
	ipipe a = p;
	opipe b = p;
	try {
		b << "HEY";
	} catch ios_base::failure e {
		cout << e.what();
	}
	string q;
	a >> q;
	cout << q;
	return EXIT_SUCCESS;

/*
	duopipe s;
	s.bind_front();
	iopipe t = s.back();
	string r;
	t << "enter: ";
	return EXIT_SUCCESS;

	ChildProcess bots[2] = {ChildProcess(programs[0]), ChildProcess(programs[1])};
	string res;
	bots[0] << "HELLO";
	bots[0] >> res;
	cout << res << "\n";
	bots[1] << "WORLD";
	bots[1] >> res;
	cout << res << "\n";

	return EXIT_SUCCESS;
*/
}
