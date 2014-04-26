/* -------------------------------------------
 * Arbiter - arbiter.hpp
 * -------------------------------------------
 * Declares the methods used by the Arbiter.
 * The Arbiter is responsible for running games
 * -------------------------------------------
 * Author:  James Payor (github.com/amoebius)
 * -------------------------------------------
 */

#ifndef __ARBITER_HPP
#define __ARBITER_HPP


const bool outside(int x, int y);

const bool canReach(int x, int y, int destination_y);

void showHelp();


#endif //__ARBITER_HPP
