/* -------------------------------------------
 * Arbiter - arbiter.hpp
 * -------------------------------------------
 * Declares the methods used by the Arbiter.
 * The Arbiter is responsible for running
 * games, playing off the given bots.
 * -------------------------------------------
 * Author:  James Payor (github.com/amoebius)
 * -------------------------------------------
 */

#ifndef __ARBITER_HPP
#define __ARBITER_HPP


#include <string>

const bool nameValid(std::string name);
const bool outside(int x, int y);
const bool canReach(int x, int y, int destination_y);

void showUsage(ostream& stream);
void showHelp(ostream& stream);



#endif //__ARBITER_HPP
