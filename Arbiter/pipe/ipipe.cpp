/* -------------------------------------------
 * ipipe - ipipe.cpp
 * -------------------------------------------
 * Implements the 'ipipe' class, which manages unix pipe input via a stream interface.
 * -------------------------------------------
 * Author:  James Payor (github.com/amoebius)
 * -------------------------------------------
 */

/* NOTE:  (IMPORTANT)
 * This module needs a hacky method to convert a pipe file descriptor into istreams
 * ostreams, or replicate that functionality.  The author envisages two methods:
 *  1) Use stringstreams to replicate high-level stream io, and write/read from the pipe
 *     to fill/empty the stringstreams.
 *  2) Use non-standard classes to build a stream around a POSIX file descriptor.
 *
 * THIS implementation uses OPTION 2.  There is a chance that it will not complie given
 * that "__gnu_cxx::stdio_filebur<char>" is used.  If this is the case, the functionality
 * can be replicated with stringstreams, but for now this has not been implemented, as
 * wrapping the file descriptors directly simplifies matters considerably.
 *
 */


#include "ipipe.hpp"
#include <unistd.h>


namespace cpipe {

	/***********************************************************************************************************/
	/*  "ipipe_ref" Implementation:                                                                            */
	/***********************************************************************************************************/

	// Construct from a file descriptor through a simple initialiser list:
	ipipe::ipipe_ref::ipipe_ref(int fd) : instances(1), fd(fd), buffer(fd, std::ios::in), stream(&buffer) {}

	// On destruction, close the buffer and the file descriptor:
	ipipe::ipipe_ref::~ipipe_ref() {
		buffer.close();
		::close(fd);
	}

	// Increment the reference count:
	void ipipe::ipipe_ref::inc() {
		++instances;
	}

	// Decrement the reference count:
	void ipipe::ipipe_ref::dec() {
		--instances;
		if(!instances) delete this; // Delete this when orphaned!
	}

	// To cast to an istream, simply return the stream:
	ipipe::ipipe_ref::operator std::istream& () {
		return stream;
	}

	// Returns the file descriptor bound to the object:
	const int ipipe::ipipe_ref::file() const {
		return fd;
	}

	/***********************************************************************************************************/




	/***********************************************************************************************************/
	/*  "ipipe" Implementation:                                                                                */
	/***********************************************************************************************************/

	// Constructs a 'closed' ipipe - sets isOpen to false:
	ipipe::ipipe() : pipe(NULL), isOpen(false) {}

	// Constructs an ipipe from the given file descriptor - creates a new ipipe_ref, and marks the ipipe open:
	ipipe::ipipe(int fd) : pipe(new ipipe_ref(fd)), isOpen(true) {}

	// Copy constructor - copies the state of the other object, and increments the ipipe_ref's reference count if necessary:
	ipipe::ipipe(const ipipe& other) : pipe(other.pipe), isOpen(other.isOpen) {
		if(isOpen) pipe->inc();
	}

	// Assignment operator - close any current file, then update as with the copy constructor:
	ipipe& ipipe::operator = (const ipipe& other) {
		close();
		pipe = other.pipe;
		isOpen = other.isOpen;
		if(isOpen) pipe->inc();
	}

	// Destructor - close any current file:
	ipipe::~ipipe() {
		close();
	}

	// Open the given file descriptor, closing an existing file first, then imitates the ipipe(int fd) constructor:
	void ipipe::open(int fd) {
		close();
		pipe = new ipipe_ref(fd);
		isOpen = true;
	}

	// Close the ipipe - decrement the ipipe_ref's reference count if open, and update open state:
	void ipipe::close() {
		if(isOpen) {
			pipe->dec();
			pipe = NULL;
			isOpen = false;
		}
	}

	// Cast to an istream - return the ipipe_ref casted to an istream:
	ipipe::operator std::istream& () const {
		return *pipe;
	}

	// Returns the ipipe_ref's file descriptor:
	const int ipipe::file() const {
		return pipe->file();
	}

	// Binds standard output to this file descriptor - straightforward invokation of the 'dup2' api method:
	void ipipe::bind() {
		dup2(STDOUT_FILENO, pipe->file());
		close();
	}

	// Returns the open status of the ipipe:
	const bool ipipe::is_open() const {
		return isOpen;
	}

	/***********************************************************************************************************/

} // namespace pipe
