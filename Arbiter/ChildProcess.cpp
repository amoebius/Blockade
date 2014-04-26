/* -------------------------------------------
 * Arbiter - ChildProcess.cpp
 * -------------------------------------------
 * Implements a rudimentary class representing
 * a child process for use in running bots for
 * the arbiter.
 * -------------------------------------------
 * Author:  James Payor (github.com/amoebius)
 * -------------------------------------------
 */

#include "ChildProcess.hpp"
#include "pipe/duopipe.hpp"
#include <signal.h>
using namespace std;


ChildProcess::ChildProcess(string filename, char * const argv[], int timeout) : filename(filename), timeout(timeout) {
	duopipe link;
	iopipe errLink;

	pid = fork();
	if(pid == 0) {
		link.bind_back();
		link.close();
		errLink.bind_err();
		errLink.close();
		execv(filename.c_str(), argv);
	}

	pipe = link.front();
	err_pipe = errLink.get_in();
	instances = new int(1);
}

ChildProcess::ChildProcess(const ChildProcess& other) :
	filename(other.filename), pid(other.pid), pipe(other.pipe), err_pipe(other.err_pipe), timeout(other.timeout), instances(other.instances) {

	++(*instances);
}

ChildProcess& ChildProcess::operator = (const ChildProcess& other) {
	if(is_open()) --(*instances);
	if(!instances) {
		kill();
		free instances;
	}
	(string &)(const string &)filename = other.filename; // Yes this is really dodgy, but it's semantic...
	pid = other.pid;
	pipe = other.pipe;
	err_pipe = other.err_pipe;
	timeout = other.timeout;
	instances = other.instances;
	++(*instances);
	return *this;
}

ChildProcess::ChildProcess() : filename(), pid(0), pipe(0), err_pipe(0), instances(NULL) {}

ChildProcess::~ChildProcess() {
	if(is_open()) {
		--(*instances);
		if(!(*instances)) kill();
	}
}

pid_t ChildProcess::getPID() const {
	return pid;
}

const iopipe& ChildProcess::getPipe() const {
	return pipe;
}

const opipe& ChildProcess::stdin() const {
	return pipe.get_out();
}

const ipipe& ChildProcess::stdout() const {
	return pipe.get_in();
}

const ipipe& ChildProcess::stderr() const {
	return err_pipe;
}

ChildProcess::operator const iopipe& () const {
	return pipe;
}

const bool ChildProcess::is_open() const {
	return pipe.is_open();
}

void ChildProcess::set_timeout(int timeout) {
	this->timeout = timeout;
}

void ChildProcess::kill() {
	if(is_open()) {
		pipe.close();
		err_pipe.close();
		kill(pid, SIGKILL);
	}
}


template <typename T>
class ReadFunctor {
	istream& stream;
	T& result;
	readTo(istream& stream, T& result) : stream(stream), result(result) {}
	const bool operator () () {
		return stream >> result;
	}
};

template <typename T>
ChildProcess& operator >> <T> (T& rhs) {
	Threaded<ReadFunctor<T> > readOperation = Threading::Create(ReadFunctor(stdin(), rhs));
	read_result = readOperation.join(timeout, false);
	if(!read_result) readOperation.cancel();
	return *this;
}