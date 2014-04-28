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

#include "pipe/iopipe.hpp"
#include "threading/threading.hpp"
using namespace cpipe;


class ChildProcess {

public:
	explicit ChildProcess(std::string filename, std::vector<std::string> argv, int timeout = -1);
	explicit ChildProcess(std::string filename, int timeout = -1) : ChildProcess(filename, std::vector<std::string>(1, filename), timeout) {}
	ChildProcess(const ChildProcess& other);
	ChildProcess();
	ChildProcess& operator = (const ChildProcess& other);
	~ChildProcess();
	pid_t getPID() const;
	const iopipe& getPipe() const;
	const opipe& in() const;
	const ipipe& out() const;
	const ipipe& err() const;
	operator const iopipe&() const;
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
	inline opipe& operator << (const T& rhs) {
		return pipe << rhs;
	}

	// Support std::endl and other manipulators:
	inline opipe& operator << (std::ostream&(*manipulator)(std::ostream&)) {
		return pipe << manipulator;
	}


	inline operator bool () {
		return read_success;
	}


private:

	// Child process id:
	pid_t pid;

	// Child stdin and stdout pipes:
	iopipe pipe;
	// Child stderr pipe:
	ipipe err_pipe;

	// Indicates whether the last read was successful.
	bool read_success;

	// Timeout for reads from child stdout.
	// <= 0: Blocking read.
	//  > 0: Try to read for the given number of milliseconds.
	int timeout;
	// NB: Only applies to read operations applied directly to this object (i.e. ChildProcess >> x).

	int *instances;


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
