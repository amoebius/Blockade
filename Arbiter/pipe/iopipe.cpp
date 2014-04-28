/* -------------------------------------------
 * iopipe - iopipe.cpp
 * -------------------------------------------
 * Implements the 'iopipe' class, which manages unix pipe io via a stream interface.
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
 * that "__gnu_cxx::stdio_filebuf<char>" is used.  If this is the case, the functionality
 * can be replicated with stringstreams, but for now this has not been implemented, as
 * wrapping the file descriptors directly simplifies matters considerably.
 *
 */

#include <unistd.h>
#include "iopipe.hpp"


namespace cpipe {

	/***********************************************************************************************************/
	/*  "iopipe" Implementation:                                                                               */
	/***********************************************************************************************************/

	// Creates an iopipe attached to a newly instantiated pipe, setting open state to false and invoking open():
	iopipe::iopipe() : isOpen(false) {
		open();
	}

	// Creates an iopipe that is not attached to a new pipe, in a closed state:
	iopipe::iopipe(int) : isOpen(false) {}

	// Binds the given file descriptors to the new iopipe by invoking the relevant constructors, and sets state to 'open':
	iopipe::iopipe(int in_fd, int out_fd) : in(in_fd), out(out_fd), isOpen(true) {}

	// Binds the given ipipe and opipe to the iopipe, useful for combining two separate streams into a unified interface:
	iopipe::iopipe(ipipe in, opipe out) : in(in), out(out), isOpen(true) {}

	// Copy constructor - simply copy the other iopipe's member variables:
	iopipe::iopipe(const iopipe& other) : in(other.in), out(other.out), isOpen(other.isOpen) {}

	// Assignment - simply assign member variables to the other iopipe's members:
	iopipe& iopipe::operator = (const iopipe& other) {
		if(&other != this) {
			in = other.in;
			out = other.out;
			isOpen = other.isOpen;
		}
		return *this;
	}

	// Returns an ipipe bound to the input file the pipe employs:
	const ipipe& iopipe::get_in() const {
		return in;
	}
	// Returns an opipe bound to the output file the pipe employs:
	const opipe& iopipe::get_out() const {
		return out;
	}

	// Creates a new pipe, binding its ends to the iopipe's ipipe and opipe components:
	void iopipe::open() {
		const int READ = 0, WRITE = 1; // Constant indices into the 'pipe_fd' array.
		int pipe_fd[2];
		pipe(pipe_fd);                 // Create a new pipe.
		in = ipipe(pipe_fd[READ]);     // Bind the 'read' end to the ipipe.
		out = opipe(pipe_fd[WRITE]);   // Bind the 'write' end to the opipe.
		isOpen = true;                 // Update open state.
	}

	// Binds the given file descriptors to this iopipe, invoking the relevant 'open' methods:
	void iopipe::open(int in_fd, int out_fd) {
		in.open(in_fd);
		out.open(out_fd);
		isOpen = true;
	}

	// Closes the iopipe by invoking the relevant close methods and updating its state flag:
	void iopipe::close() {
		if(isOpen) {
			in.close();
			out.close();
			isOpen = false;
		}
	}

	// Cast to an istream by casting the bound ipipe to an istream:
	iopipe::operator std::istream& () const {
		return in;
	}
	// Cast to an ostream by casting the bound opipe to an ostream:
	iopipe::operator std::ostream& () const {
		return out;
	}

	// Cast to an ipipe by returning the input end of this iopipe:
	iopipe::operator ipipe () const {
		return in;
	}
	// Cast to an opipe by returning the output end of the iopipe:
	iopipe::operator opipe () const {
		return out;
	}

	// Return the input file descriptor:
	const int iopipe::in_file() const {
		return in.file();
	}
	// Return the output file descriptor:
	const int iopipe::out_file() const {
		return out.file();
	}

	// Return the open state of the iopipe:
	const bool iopipe::is_open() const {
		return isOpen;
	}

	// Bind standard input to the input of this iopipe by invoking the ipipe's own 'bind' method:
	void iopipe::bind_in() {
		in.bind();
	}
	// Bind standard output to the output of this iopipe by invoking the opipe's own 'bind' method:
	void iopipe::bind_out() {
		out.bind();
	}
	// Bind standard error to the output of this iopipe by invoking the opipe's own 'bind' method:
	void iopipe::bind_err() {
		out.bind_err();
	}
	// Binds both standard input and standard output to this pipe:
	void iopipe::bind() {
		in.bind();
		out.bind();
	}

	/***********************************************************************************************************/

} // namespace cpipe
