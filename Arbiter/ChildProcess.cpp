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


ChildProcess::ChildProcess(string filename, char * const argv[]) : filename(filename) {
	duopipe link;
	iopipe errLink;

	pid = fork();
	if(pid == 0) {
		link.bind_back();
		link.close();
		errLink.get_out().bindErr();
		execv(filename.c_str(), argv);
	}

	pipe = link.front();
	errPipe = errLink.get_in();
	instances = new int(1);
}

ChildProcess::ChildProcess(const ChildProcess& other) :
	filename(other.filename), pid(other.pid), pipe(other.pipe), errPipe(other.errPipe), instances(other.instances) {

	++(*instances);
}

ChildProcess& ChildProcess::operator = (const ChildProcess& other) {
	if(isOpen()) --(*instances);
	if(!instances) close();
	(string &)(const string &)filename = other.filename; // Yes this is really dodgy, but it's semantic...
	pid = other.pid;
	pipe = other.pipe;
	errPipe = other.errPipe;
	instances = other.instances;
	++(*instances);
	return *this;
}

ChildProcess::ChildProcess() : filename(), pid(0), pipe(0), errPipe(0), instances(NULL) {}

ChildProcess::~ChildProcess() {
	if(isOpen()) --(*instances);
	if(!(*instances)) close();
}

pid_t ChildProcess::getPID() const {
	return pid;
}

const iopipe& ChildProcess::getPipe() const {
	return pipe;
}

const ipipe& ChildProcess::err() const {
	return errPipe;
}

ChildProcess::operator const iopipe& () const {
	return pipe;
}

const bool ChildProcess::isOpen() const {
	return pipe.is_open();
}

void ChildProcess::close() {
	if(isOpen()) {
		pipe.close();
		errPipe.close();
		kill(pid, SIGKILL);
	}
}
