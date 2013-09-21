/* -------------------------------------------
 * iopipe - iopipe.hpp
 * -------------------------------------------
 * Declares the interface for the 'iopipe' class, which manages unix pipe io via a
 * stream interface.
 * -------------------------------------------
 * Author:  James Payor (github.com/amoebius)
 * -------------------------------------------
 */

/* tl;dr |
 * -------
 * Classes:
 * ipipe - represents the input end of a pipe
 * opipe - represents the output end of a pipe
 * iopipe - represents a full pipe, and can instantiate new pipes
 *
 * Usage:
 *
 * iopipe myPipe; // Creates a new pipe.
 * myPipe << "Hello_World!" << endl; // Write to a pipe.
 * string message;
 * myPipe >> message; // Read from a pipe.
 *
 * ipipe input = myPipe.get_in();   // Gets the object representing the input end of the pipe.
 * opipe output = myPipe.get_out(); // Gets the object representing the output end of the pipe.
 * int input_file_descriptor = input.file(); // Gets the file descriptor.
 * int output_file_descriptor = output.file(); // Gets the file descriptor.
 * input.bind(); // Binds to stdin.
 * output.bind(); // Binds to stdout.
 *
 * See definitions below for further functionality.
 * (open(), close(), in_file(), outFile(), shared underlying streams on copying, etc.)



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

#ifndef __IOPIPE_HPP
#define __IOPIPE_HPP


#include <iostream>
#include "ipipe.hpp"
#include "opipe.hpp"


namespace cpipe {

	/* iopipe:
	 * Constructs pipes and corresponding ipipe and opipe objects, providing stream-like
	 * reading and writing to a pipe.
	 ************************************************************************************/
	class iopipe {
	public:
		// Construct an iopipe bound to a new pipe (this will instantiate a pipe and deal with the
		// corresponding file descriptors, binding them to an ipipe and opipe):
		iopipe();
		// Construct an iopipe NOT bound to a new pipe - i.e. not bound to any file / closed:
		explicit iopipe(int);

		// Construct an iopipe bound to the given input and output file descriptors:
		iopipe(int in_fd, int out_fd);
		// Construct an iopipe from a separate ipipe and opipe, useful for combining two pipe streams
		// into one interface:
		iopipe(ipipe in, opipe out);

		// Copy constructor: (detaches from / closes existing files)
		iopipe(const iopipe& other);
		// Assignment:       (detaches from / closes existing files)
		iopipe& operator = (const iopipe& other);

		// Creates a new pipe, and binds the ipipe and opipe objects to the read/write ends of the pipe:
		void open();
		// Binds the iopipe to the given input and output file descriptors:
		void open(int in_fd, int out_fd);
		// Detaches from / closes the files that the iopipe is bound to:
		void close();
		// Indicates whether the iopipe is currently bound to a set of file descriptors:
		const bool is_open() const;

		// Returns the ipipe this iopipe refers to:
		const ipipe& get_in() const;
		// Returns the opipe this iopipe refers to:
		const opipe& get_out() const;
		// Returns the input file descriptor bound to this iopipe:
		const int in_file() const;
		// Returns the output file descriptor bound to this iopipe:
		const int out_file() const;

		// Binds standard input to this iopipe:
		void bind_in();
		// Binds standard output to this iopipe:
		void bind_out();
		// Binds standard input and standard output to this iopipe:
		void bind();

		// Cast to an istream:
		operator std::istream& () const;
		// Cast to an ostream:
		operator std::ostream& () const;

		// Cast to an ipipe:
		operator ipipe () const;
		// Cast to an opipe:
		operator opipe () const;

		// Provide the stream insertion operator:
		template <typename T>
		inline std::ostream& operator << (const T& rhs) const {
			return out << rhs;
		}

		// Provide the stream extraction operator:
		template <typename T>
		inline std::istream& operator >> (T& rhs) const {
			return in >> rhs;
		}

	private:
		// The open status of the iopipe:
		bool isOpen;
		// The input portion of the iopipe:
		ipipe in;
		// The output portion:
		opipe out;
	};

} // namespace pipe

#endif //__IOPIPE_HPP
