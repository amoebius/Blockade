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
using namespace cpipe;


ChildProcess::ChildProcess(std::string filename, const std::vector<std::string>& argv, int timeout)
	: filename(filename), read_success(true), timeout(timeout), instances(new int(1)),
	  original_duopipe(), original_err_pipe(), pipe(original_duopipe.front()), err_pipe(original_err_pipe.get_in()) {
	
	pid = fork();
	if(pid == 0) {
		// Redirect IO:
		original_duopipe.bind_back();
		original_err_pipe.bind_err();
		close_pipes();

		// Convert the arguments into the required format:
		const char * arguments[argv.size()+1];
		for(int i = 0; i < argv.size(); ++i) arguments[i] = argv[i].c_str();
		arguments[argv.size()] = NULL;

		// Execute!
		execv(filename.c_str(), (char * const *)arguments);

		// Welp.  We really shouldn't be here.  Abandonnnn ship.
		std::cerr << "Failed to start subprocess.  Aborting." << std::endl;
		exit(EXIT_FAILURE);
	}

}

ChildProcess::ChildProcess(const ChildProcess& other)
	: filename(other.filename), pid(other.pid), read_success(other.read_success), timeout(other.timeout), instances(other.instances),
	  original_duopipe(other.original_duopipe), original_err_pipe(other.original_err_pipe), pipe(other.pipe), err_pipe(other.err_pipe) {

	++(*instances);
}

ChildProcess& ChildProcess::operator = (const ChildProcess& other) {
	if(&other == this) return *this;

	close_object();
	
	(std::string &)(const std::string &)filename = other.filename; // Yes this is really dodgy, but it's semantic...
	pid = other.pid;
	read_success = other.read_success;
	timeout = other.timeout;
	instances = other.instances;
	original_duopipe = other.original_duopipe;
	original_err_pipe = other.original_err_pipe;
	pipe = other.pipe;
	err_pipe = other.err_pipe;

	++(*instances);
	return *this;
}

ChildProcess::ChildProcess()
	: filename(), pid(0), read_success(false), timeout(), instances(NULL),
	  original_duopipe(0), original_err_pipe(0), pipe(0), err_pipe() {}

ChildProcess::~ChildProcess() {
	close_object();
}

const pid_t ChildProcess::get_pid() const {
	return pid;
}

const duopipe& ChildProcess::get_duopipe() const {
	return original_duopipe;
}

const iopipe& ChildProcess::get_err_pipe() const {
	return original_err_pipe;
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
		close_pipes();
		delete instances;
		::kill(pid, SIGKILL);
	}
}

void ChildProcess::close_pipes() {
	pipe.close();
	err_pipe.close();
	original_duopipe.close();
	original_err_pipe.close();
}

void ChildProcess::close_object() {
	if(is_open()) {
		if(--(*instances) == 0) {
			kill();
		}
	}
}