/* -------------------------------------------
 * opipe - opipe.cpp
 * -------------------------------------------
 * Implements the 'opipe' class, which manages unix pipe output via a stream interface.
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


#include "opipe.hpp"
#include <unistd.h>
#include <signal.h>


namespace cpipe {

	/***********************************************************************************************************/
	/*  "opipe_ref" Implementation:                                                                            */
	/***********************************************************************************************************/

	// Construct from a file descriptor through a simple initialiser list:
	opipe::opipe_ref::opipe_ref(int fd) : instances(1), fd(fd), buffer(fd, std::ios::out), stream(&buffer) {}

	// On destruction, close the buffer and the file descriptor:
	opipe::opipe_ref::~opipe_ref() {
		buffer.close();
		::close(fd);
	}

	// Increment the reference count:
	void opipe::opipe_ref::inc() {
		++instances;
	}

	// Decrement the reference count:
	void opipe::opipe_ref::dec() {
		--instances;
		if(!instances) delete this;
	}

	// To cast to an ostream, simply return the stream:
	opipe::opipe_ref::operator std::ostream& () {
		return stream;
	}

	// Returns the file descriptor bound to the object:
	const int opipe::opipe_ref::file() const {
		return fd;
	}

	/***********************************************************************************************************/




	/***********************************************************************************************************/
	/*  "opipe" Implementation:                                                                                */
	/***********************************************************************************************************/

	// Constructs a 'closed' opipe - sets isOpen to false:
	opipe::opipe() : pipe(NULL), isOpen(false), write_success(false) {}

	// Constructs an opipe from the given file descriptor - creates a new opipe_ref, and marks the opipe open:
	opipe::opipe(int fd) : pipe(new opipe_ref(fd)), isOpen(true), write_success(true) {
		// STOP SIGPIPE >:C
		signal(SIGPIPE, SIG_IGN);
	}

	// Copy constructor - copies the state of the other object, and increments the ipipe_ref's reference count if necessary:
	opipe::opipe(const opipe& other) : pipe(other.pipe), isOpen(other.isOpen), write_success(other.write_success) {
		if(isOpen) pipe->inc();
	}

	// Assignment operator - close any current file, then update as with the copy constructor:
	opipe& opipe::operator = (const opipe& other) {
		if(&other != this) {
			close();
			pipe = other.pipe;
			isOpen = other.isOpen;
			write_success = other.write_success;
			if(isOpen) pipe->inc();
		}
		return *this;
	}

	// Destructor - close any current file:
	opipe::~opipe() {
		close();
	}

	// Open the given file descriptor, closing an existing file first, then imitates the opipe(int fd) constructor:
	void opipe::open(int fd) {
		close();
		pipe = new opipe_ref(fd);
		isOpen = true;
	}

	// Close the opipe - decrement the opipe_ref's reference count if open, and update open state:
	void opipe::close() {
		if(isOpen) {
			pipe->dec();
			pipe = NULL;
			isOpen = false;
		}
	}

	// Cast to an ostream - return the opipe_ref casted to an ostream:
	opipe::operator std::ostream& () const {
		return *pipe;
	}

	// Returns the opipe_ref's file descriptor:
	const int opipe::file() const {
		return pipe->file();
	}

	// Binds the file descriptor (by default standard output) to this pipe - 'straightforward' invokation of the 'dup2' api method:
	void opipe::bind(int fd) {
		dup2(pipe->file(), fd);
	}

	// Binds to stderr:
	void opipe::bind_err() {
		bind(STDERR_FILENO);
	}

	// Returns the open status of the opipe:
	const bool opipe::is_open() const {
		return isOpen;
	}

	/***********************************************************************************************************/

} // namespace cpipe
