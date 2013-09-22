/* -------------------------------------------
 * Blockade Modules - ioblock.h
 * -------------------------------------------
 * Implements a very hacky, but very useful
 * method for blocking reading and writing to
 * stdin and stdout , to prevent modules from
 * intefering with the arbiter-module stream.
 * -------------------------------------------
 * Author:  James Payor (github.com/amoebius)
 * -------------------------------------------
 */

#ifndef __IOBLOCK_H
#define __IOBLOCK_H

namespace ioblock {

	void init();
	void block();
	void unblock();

}

#endif //__IOBLOCK_H
