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


namespace cpipe {

	/***********************************************************************************************************/
	/*  "duopipe" Implementation:                                                                               */
	/***********************************************************************************************************/

	// Instantiates two new pipes and builds a duopipe from them:
	duopipe::duopipe() : _front(0), _back(0), isOpen(true) { // Don't create unnecessary pipes.
		// Create two new pipes:
		iopipe a,b;
		// Bind front to the read and write ends of a and b respectively:
		_front = iopipe(a,b);
		// Bind back to the write and read ends of a and b respectively:
		_back = iopipe(b,a);
	}

	// Construct a duopipe NOT bound to any new pipes:
	duopipe::duopipe(int) : _front(0), _back(0), isOpen(false) {}

	// Reuses the pipes of another duopipe (copy constructor):
	duopipe::duopipe(const duopipe& other) : _front(other.front()), _back(other.back()), isOpen(other.isOpen) {}

	// Builds a duopipe from the front and back ends of the two specified pipes:
	duopipe::duopipe(iopipe first, iopipe second) : _front(first, second), _back(second, first), isOpen(true) {}

	// Reuses the pipes of another duopipe for assignment operations:
	duopipe& duopipe::operator = (const duopipe& other) {
		if(&other != this) {
			_front = other._front;
			_back = other._back;
			isOpen = other.isOpen;
		}
		return *this;
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
	void duopipe::bind_front() {
		_front.bind();
	}
	// Binds stdin and stdout to the back end of the duopipe:
	void duopipe::bind_back() {
		_back.bind();
	}

	// Indicates whether the duopipe is currently open:
	const bool duopipe::is_open() const {
		return isOpen;
	}
	// Closes both ends of the pipe:
	void duopipe::close() {
		if(isOpen) {
			_front.close();
			_back.close();
			isOpen = false;
		}
	}

	/***********************************************************************************************************/

} // namespace cpipe
