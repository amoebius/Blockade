/* -------------------------------------------
 * Blockade Modules - ioblock.cpp
 * -------------------------------------------
 * Implements a very hacky, but very useful
 * method for blocking reading and writing to
 * stdin and stdout, to prevent bots from
 * intefering with the arbiter-module stream.
 * -------------------------------------------
 * Author:  James Payor (github.com/amoebius)
 * -------------------------------------------
 */


#include "ioblock.hpp"
#include <unistd.h>
#include <fcntl.h>

namespace ioblock {

	int infd, outfd, errfd, devnull;
	bool initialised = false, blocked;

	void init() {
		if(initialised) return;
		initialised = true;

		infd = dup(STDIN_FILENO);
		outfd = dup(STDOUT_FILENO);
		errfd = dup(STDERR_FILENO);
		devnull = open("/dev/null", O_RDWR);
		blocked = false;
	}

	void clean() {
		if(!initialised) return;
		if(blocked) unblock();

		close(infd);
		close(outfd);
		close(errfd);
		close(devnull);
		initialised = false;
	}

	void block() {
		if(blocked) return;
		blocked = true;

		dup2(devnull, STDIN_FILENO);
		dup2(devnull, STDOUT_FILENO);
		dup2(devnull, STDERR_FILENO);
	}

	void unblock() {
		if(!blocked) return;
		blocked = false;

		dup2(infd, STDIN_FILENO);
		dup2(outfd, STDOUT_FILENO);
		dup2(errfd, STDERR_FILENO);
	}

}; // namespace ioblock
