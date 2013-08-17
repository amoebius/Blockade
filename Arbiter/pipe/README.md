Pipe Module
===========

Overview
--------

Classes
-------

This module is designed to conveniently wrap the functionality of unix pipes into a series of abstractions:

- ipipe
 - Represents the 'read' end of a pipe.
 - Wraps a std::istream around the pipe, and provides stream extraction ('>>' operator).
 - Supports binding standard input to the pipe.
 - Supports 'open' and 'close' operations.
 
- opipe
 - Represents the 'write' end of a pipe.
 - Wraps a std::ostream around the pipe, and provides stream insertion ('<<' operator).
 - Supports binding standard output to the pipe.
 - Supports 'open' and 'close' operations.
 
- iopipe
 - Represents both a read and a write end of some unix pipe.
 - Maintains an internal ipipe and opipe.
 - The default constructor will instantiate a new pipe, binding both ends to the iopipe.
 - Different file descriptors can be opened, and the iopipe can be 'closed'.
 - The ipipe and opipe ends can be specified, allowing the iopipe to reference different pipes internally.
 - In short:
  - Creates pipes (providing ipipe and opipe ends).
  - Allows '<<' and '>>'.
  - Can reference different pipes for input and output.
  
- duopipe
 - Represents a two-pipe interface that allows communication between to 'ends'.
  - Can be used for stream-like inter-thread communication, etc.
 - Instantiates two separate pipes, binding separate input and output to the 'front' end, and similarly to the 'back' end.
 - Allows standard input and output to be bound to either end.

Each class supports standard copying by construction and assignment, and should otherwise just work as expected (cleaning up after themselves with internal reference counting, closing unused pipes, etc).
Primarily intended for inter-thread communication, the duopipe is perhaps the most useful class, though an iopipe + ipipe + opipe combination allow for lower-level manipulation of the pipes.


Example Usage (Untested for now)
--------------------------------

    #include "pipe/duopipe.h"
    using namespace pipe;

    // Spawns a child process and returns an iopipe to communicate with it
	// via standard input and output:
	iopipe spawnChildProcess(const char * path, char * const argv[]) {
	    duopipe pipe;
	    if(fork() == 0) {
            pipe.bind_back();
            execv(path, argv);
        } else {
            return pipe.front();
        }
    }