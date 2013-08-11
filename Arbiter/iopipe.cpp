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
 * that "__gnu_cxx::stdio_filebur<char>" is used.  If this is the case, the functionality
 * can be replicated with stringstreams, but for now this has not been implemented, as
 * wrapping the file descriptors directly simplifies matters considerably.
 *
 */


#include <unistd.h>
#include "iopipe.hpp"


// ipipe_ref

ipipe::ipipe_ref::ipipe_ref(int fd) : instances(1), fd(fd), buffer(fd, std::ios::in), stream(&buffer) {}

ipipe::ipipe_ref::~ipipe_ref() {
	buffer.close();
	close(fd);
}

void ipipe::ipipe_ref::inc() {
	++instances;
}

void ipipe::ipipe_ref::dec() {
	--instances;
	if(!instances) delete this;
}


// opipe_ref

opipe::opipe_ref::opipe_ref(int fd) : instances(1), fd(fd), buffer(fd, std::ios::out), stream(&buffer) {}

opipe::opipe_ref::~opipe_ref() {
	buffer.close();
	close(fd);
}

void opipe::opipe_ref::inc() {
	++instances;
}

void opipe::opipe_ref::dec() {
	--instances;
	if(!instances) delete this;
}


// ipipe

ipipe::ipipe() : pipe(NULL), isOpen(false) {}

ipipe::ipipe(int fd) : pipe(new ipipe_ref(fd)), isOpen(true) {}

ipipe::ipipe(const ipipe& other) : pipe(other.pipe) {
	pipe->inc();
}

ipipe& ipipe::operator = (const ipipe& other) {
	close();
	pipe = other.pipe;
	pipe->inc();
}

ipipe::~ipipe() {
	close();
}

void ipipe::open(int fd) {
	close();
	pipe = new ipipe_ref(fd);
	isOpen = true;
}

void ipipe::close() {
	pipe->dec();
}


// opipe

opipe::opipe() : pipe(NULL), isOpen(false) {}

opipe::opipe(int fd) : pipe(new opipe_ref(fd)), isOpen(true) {}

opipe::opipe(const opipe& other) : pipe(other.pipe) {
	pipe->inc();
}

opipe& opipe::operator = (const opipe& other) {
	close();
	pipe = other.pipe;
	pipe->inc();
}

opipe::~opipe() {
	close();
}

void opipe::open(int fd) {
	close();
	pipe = new opipe_ref(fd);
	isOpen = true;
}

void opipe::close() {
	if(isOpen) {
		pipe->dec();
		pipe = NULL;
		isOpen = false;
	}
}


// iopipe

iopipe::iopipe() : isOpen(false) {
	open();
}

iopipe::iopipe(int) {}

iopipe::iopipe(int in_fd, int out_fd) : in(in_fd), out(out_fd), isOpen(true) {}

iopipe::iopipe(ipipe in, opipe out) : in(in), out(out) {}

const ipipe iopipe::getIn() {
	return in;
}

const opipe iopipe::getOut() {
	return out;
}

void iopipe::open() {
	const int READ = 0, WRITE = 1;
	int pipe_fd[2];
	pipe(pipe_fd);
	in = ipipe(pipe_fd[READ]);
	out = opipe(pipe_fd[WRITE]);
	isOpen = true;
}

void iopipe::open(int in_fd, int out_fd) {
	in.open(in_fd);
	out.open(out_fd);
	isOpen = true;
}

void iopipe::close() {
	if(isOpen) {
		in.close();
		out.close();
		isOpen = false;
	}
}

iopipe& iopipe::operator = (const iopipe& other) {
	in = other.in;
	out = other.out;
	isOpen = other.isOpen;
	return *this;
}
