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
 * ipipe input = myPipe.getIn();   // Gets the object representing the input end of the pipe.
 * opipe output = myPipe.getOut(); // Gets the object representing the output end of the pipe.
 * int input_file_descriptor = input.file(); // Gets the file descriptor.
 * int output_file_descriptor = output.file(); // ^
 * input.bind(); // Binds to stdin.
 * output.bind(); // Binds to stdout.
 *
 * See definitions below for further functionality (open(), close(), inFile(), outFile(), shared underlying streams on copying, etc.)



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
#include <ext/stdio_filebuf.h> // non-standard, see above note
typedef __gnu_cxx::stdio_filebuf<char> fdbuffer;


class ipipe {

private:
	class ipipe_ref {
	public:
		explicit ipipe_ref(int fd);
		~ipipe_ref();
		void inc();
		void dec();
		const int file();

		template <typename T>
		std::istream& operator >> (T& rhs) {
			return stream >> rhs;
		}

		operator std::istream& ();

	private:
		int instances, fd;
		fdbuffer buffer;
		std::istream stream;
	};

public:
	ipipe();
	explicit ipipe(int fd);
	ipipe(const ipipe& other);
	ipipe& operator = (const ipipe& other);
	~ipipe();
	void open(int fd);
	void close();
	void bind();
	const int file();

	operator std::istream& ();

	template <typename T>
	std::istream& operator >> (T& rhs) {
		if(!isOpen) throw std::ios_base::failure("Input pipe not open.");
		return *pipe >> rhs;
	}

private:
	ipipe_ref *pipe;
	bool isOpen;
};


class opipe {

private:
	class opipe_ref {
	public:
		explicit opipe_ref(int fd);
		~opipe_ref();
		void inc();
		void dec();
		const int file();

		template <typename T>
		std::ostream& operator << (const T& rhs) {
			return stream << rhs;
		}

		operator std::ostream& ();

	private:
		int instances, fd;
		fdbuffer buffer;
		std::ostream stream;
	};

public:
	opipe();
	explicit opipe(int fd);
	opipe(const opipe& other);
	opipe& operator = (const opipe& other);
	~opipe();
	void open(int fd);
	void close();
	void bind();
	const int file();

	operator std::ostream& ();

	template <typename T>
	std::ostream& operator << (const T& rhs) {
		if(!isOpen) throw std::ios_base::failure("Output pipe not open.");
		return *pipe << rhs;
	}

private:
	opipe_ref *pipe;
	bool isOpen;
};


class iopipe {
public:
	iopipe();
	explicit iopipe(int);
	iopipe(int in_fd, int out_fd);
	iopipe(ipipe in, opipe out);
	iopipe(const iopipe& other);
	iopipe& operator = (const iopipe& other);
	const ipipe getIn();
	const opipe getOut();
	const int inFile();
	const int outFile();
	void open();
	void open(int in_fd, int out_fd);
	void close();

	operator std::istream& ();
	operator std::ostream& ();

	template <typename T>
	std::ostream& operator << (const T& rhs) {
		return out << rhs;
	}

	template <typename T>
	std::istream& operator >> (T& rhs) {
		return in >> rhs;
	}

private:
	bool isOpen;
	ipipe in;
	opipe out;
};


#endif //__IOPIPE_HPP
