/* -------------------------------------------
 * Blockade Modules - ioblock.h
 * -------------------------------------------
 * Implements useful methods for temporarily
 * blocking reading and writing to stdin and
 * stdout, to prevent modules from intefering
 * with arbiter-module communication.
 * -------------------------------------------
 * Author:  James Payor (github.com/amoebius)
 * -------------------------------------------
 */

#ifndef __IOBLOCK_H
#define __IOBLOCK_H

#include <unistd.h>

static int infd = -1, outfd = -1;

void ioblock() {

	if(infd < 0 || outfd < 0) {
		infd = dup(STDIN_FILENO);
		outfd = dup(STDOUT_FILENO);
	}

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
}

void iounblock() {
	dup2(infd, STDIN_FILENO);
	dup2(outfd, STDOUT_FILENO);
}


#endif //__IOBLOCK_H
