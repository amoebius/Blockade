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


#include "pipe/iopipe.hpp"
#include <unistd.h>
#include <string>
#include <cstddef>
using namespace cpipe;

class ChildProcess {

public:
	explicit ChildProcess(std::string filename, char * const argv[] = NULL);
	ChildProcess(const ChildProcess& other);
	ChildProcess& operator = (const ChildProcess& other);
	~ChildProcess();
	pid_t getPID() const;
	const iopipe& getPipe() const;
	operator const iopipe&() const;
	const bool is_open() const;

	const std::string filename;

	template <typename T>
	inline std::istream& operator >> (T& rhs) {
		return pipe >> rhs;
	}

	template <typename T>
	inline std::ostream& operator << (T& rhs) {
		return pipe << rhs;
	}


private:
	void close();

	pid_t pid;
	iopipe pipe;
	int *instances;

};


#endif //__CHILDPROCESS_HPP
