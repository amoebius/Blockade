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

#ifndef __CHILDPROCESS_HPP
#define __CHILDPROCESS_HPP


#include <unistd.h>
#include <string>
#include <vector>
#include <cstddef>

#include "pipe/duopipe.hpp"
#include "threading/threading.hpp"


class ChildProcess {

public:
	explicit ChildProcess(std::string filename, std::vector<std::string> argv, int timeout = -1);
	explicit ChildProcess(std::string filename, int timeout = -1) : ChildProcess(filename, std::vector<std::string>(1, filename), timeout) {}
	ChildProcess(const ChildProcess& other);
	ChildProcess();
	ChildProcess& operator = (const ChildProcess& other);
	~ChildProcess();
	const pid_t get_pid() const;
	const cpipe::duopipe& get_duopipe() const;
	const cpipe::iopipe& get_err_pipe() const;
	const cpipe::opipe& in() const;
	const cpipe::ipipe& out() const;
	const cpipe::ipipe& err() const;
	operator const cpipe::iopipe&() const;
	const bool is_open() const;
	void set_timeout(int timeout);
	void kill();

	const std::string filename;


	template <typename T>
	inline ChildProcess& operator >> (T& rhs) {
		if (!read_success || !pipe.is_open()) {
			read_success = false;
		} else {
			Threading::Threaded<ReadFunctor<T> > readOperation = Threading::Create(ReadFunctor<T>(out(), rhs));
			read_success = readOperation.join(timeout, false);
			if(!read_success) readOperation.cancel();
		}
		return *this;
	}

	template <typename T>
	inline cpipe::opipe& operator << (const T& rhs) {
		return pipe << rhs;
	}

	// Support std::endl and other manipulators:
	inline cpipe::opipe& operator << (std::ostream&(*manipulator)(std::ostream&)) {
		return pipe << manipulator;
	}


	inline operator bool () {
		return read_success;
	}


private:

	void close_pipes();

	// Child process id:
	pid_t pid;

	// Indicates whether the last read was successful.
	bool read_success;

	// Timeout for reads from child stdout.
	// <= 0: Blocking read.
	//  > 0: Try to read for the given number of milliseconds.
	int timeout;
	// NB: Only applies to read operations applied directly to this object (i.e. ChildProcess >> x).

	// Reference count for this subprocess:
	int *instances;

	// The original duopipe:
	cpipe::duopipe original_duopipe;
	// The original stderr iopipe:
	cpipe::iopipe original_err_pipe;

	// Child stdin and stdout pipes:
	cpipe::iopipe pipe;
	// Child stderr pipe:
	cpipe::ipipe err_pipe;

	
	// Helper functors for reads with timeouts:
	template <typename T>
	class ReadFunctor {
		std::istream& stream;
		T& result;
	public:
		ReadFunctor(std::istream& stream, T& result) : stream(stream), result(result) {}
		const bool operator () () {
			return stream >> result;
		}
	};

};


#endif //__CHILDPROCESS_HPP
