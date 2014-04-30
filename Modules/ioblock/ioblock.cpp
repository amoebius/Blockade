/* -------------------------------------------
 * Blockade Modules - ioblock.cpp
 * -------------------------------------------
 * Implements a very hacky, but very useful
 * method for blocking reading and writing to
 * stdin and stdout, to prevent bots from
 * intefering with the arbiter-module stream.
 * Standard output is redirected to standard
 * error, so that logging output can still be
 * read.
 * -------------------------------------------
 * Author:  James Payor (github.com/amoebius)
 * -------------------------------------------
 */


#include "ioblock.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>

namespace ioblock {

	int infd, outfd, devnull;
	bool initialised = false, blocked;

	void init() {
		if(initialised) return;
		initialised = true;

		infd = dup(STDIN_FILENO);
		outfd = dup(STDOUT_FILENO);
		devnull = open("/dev/null", O_RDONLY);
		blocked = false;
	}

	void clean() {
		if(!initialised) return;
		unblock();

		close(infd);
		close(outfd);
		close(devnull);
		initialised = false;
	}

	void block() {
		if(blocked) return;
		blocked = true;

		fflush(stdout);
		dup2(devnull, STDIN_FILENO);
		dup2(STDERR_FILENO, STDOUT_FILENO);
	}

	void unblock() {
		if(!blocked) return;
		blocked = false;

		fflush(stdout);
		dup2(infd, STDIN_FILENO);
		dup2(outfd, STDOUT_FILENO);
	}

}; // namespace ioblock
