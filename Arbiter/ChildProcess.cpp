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

	pid = fork();
	if(pid == 0) {
		link.bind_back();
		execv(filename.c_str(), argv);
	}

	pipe = link.front();
	instances = new int(1);
}

ChildProcess::ChildProcess(const ChildProcess& other) : filename(other.filename), pid(other.pid), pipe(other.pipe), instances(other.instances) {
	++(*instances);
}

ChildProcess& ChildProcess::operator = (const ChildProcess& other) {
	filename = other.filename;
	pid = other.pid;
	pipe = other.pipe;
	instances = other.instances;
	++(*instances);
}

ChildProcess::~ChildProcess() {
	--(*instances);
	if(!(*instances)) close();
}

pid_t ChildProcess::getPID() const {
	return pid;
}

const iopipe& ChildProcess::getPipe() const {
	return pipe;
}

ChildProcess::operator const iopipe& () const {
	return pipe;
}

const bool ChildProcess::is_open() const {
	return pipe.is_open();
}

void ChildProcess::close() {
	if(!is_open()) {
		pipe.close();
		kill(pid, SIGKILL);
	}
}
