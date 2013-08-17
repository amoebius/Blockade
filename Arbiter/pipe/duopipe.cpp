/* -------------------------------------------
 * duopipe - duopipe.cpp
 * -------------------------------------------
 * Implements the 'duopipe' class, which wraps two unix pipes into an object
 * providing a two interlinked iopipes.
 * -------------------------------------------
 * Author:  James Payor (github.com/amoebius)
 * -------------------------------------------
 */

#include "iopipe.hpp"
#include "duopipe.hpp"

namespace pipe {

	/***********************************************************************************************************/
	/*  "duopipe" Implementation:                                                                               */
	/***********************************************************************************************************/

	// Instantiates two new pipes and builds a duopipe from them:
	duopipe::duopipe() : _front(int), _back(int) { // Don't create unnecessary pipes.
		// Create two new pipes:
		iopipe a,b;
		// Bind front to the read and write ends of a and b respectively:
		_front = iopipe(a,b);
		// Bind back to the write and read ends of a and b respectively:
		_back = iopipe(b,a);
	}

	// Reuses the pipes of another duopipe (copy constructor):
	duopipe::duopipe(const duopipe& other) : _front(other.front()), _back(other.back()) {}

	// Builds a duopipe from the front and back ends of the two specified pipes:
	duopipe::duopipe(iopipe first, iopipe second) : _front(first, second), _back(second, first) {}

	// Reuses the pipes of another duopipe for assignment operations:
	duopipe& duopipe::operator = (const duopipe& other) {
		_front = other.front();
		_back = other.back();
	}

	// Returns the front end, represented as an iopipe:
	const iopipe& duopipe::front() const {
		return _front;
	}
	// Returns the back end, represented as an iopipe:
	const iopipe& duopipe::back() const {
		return _back;
	}

	// Binds stdin and stdout to the front end of the duopipe:
	void bind_front() {
		_front.bind();
	}
	// Binds stdin and stdout to the back end of the duopipe:
	void bind_back() {
		_back.bind();
	}

	/***********************************************************************************************************/

} // namespace pipe
