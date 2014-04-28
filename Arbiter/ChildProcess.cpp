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

#include <sys/types.h>
#include <signal.h>
#include <cstdlib>
#include <istream>
#include <iostream>

#include "ChildProcess.hpp"
#include "pipe/duopipe.hpp"


ChildProcess::ChildProcess(std::string filename, std::vector<std::string> argv, int timeout) : filename(filename), timeout(timeout), read_success(true) {
	
	duopipe link;
	iopipe errLink;

	pid = fork();
	if(pid == 0) {
		// Redirect IO:
		link.bind_back();
		link.close();
		errLink.bind_err();
		errLink.close();
	
		// Convert the arguments into the required format:
		char **arguments = new char* [argv.size()+1];
		for(int i = 0; i < argv.size(); ++i) arguments[i] = (char *)argv[i].c_str();
		arguments[argv.size()] = NULL;

		execv(filename.c_str(), arguments);
		// Welp.  We really shouldn't be here.  Abandonnnn ship.
		std::cerr << "Failed to start bot '" << filename << "'...  Aborting subprocess." << std::endl;
		sleep(500);
		exit(EXIT_FAILURE);
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
		delete instances;
	}
	(std::string &)(const std::string &)filename = other.filename; // Yes this is really dodgy, but it's semantic...
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

const opipe& ChildProcess::in() const {
	return pipe.get_out();
}

const ipipe& ChildProcess::out() const {
	return pipe.get_in();
}

const ipipe& ChildProcess::err() const {
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
		::kill(pid, SIGKILL);
	}
}