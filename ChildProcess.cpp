#include "threading/threading.hpp"
#include "iopipe/iopipe.hpp"
#include <unistd.h>
#include <string>
using namespace std;
using namespace Threading;

namespace ChildProcess {

	class Runner {

		Runner(const string& filename, const iopipe& pipe) : filename(filename), pipe(pipe) {}
		threadvoid operator() () {
			pipe.bind();
			char **argv = new char*;
			argv[0] = NULL;
			execv(filename.c_str(), argv);
		}

	private:
		string filename;
		iopipe pipe;
	};

	iopipe Create(string filename) {
		iopipe read, write;
		Threading::Create(ChildRunner(filename, iopipe(write, read)));
		return iopipe(read, write);
	}

}
