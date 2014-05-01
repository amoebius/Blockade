Modules
=======

A series of different interfaces that users can choose between in implementing their bot.

Also located here (for now) are libraries for blocking the interaction of bot code with standard input and standard output.  This code is not used as a security measure (as of course it's possible for malicious bot code to do whatever it wants to the client process), but will prevent debugging output from interfering with arbiter-client communications - in fact redirecting standard output to standard error so that bot output can be logged for debugging.

At present, the only supported languages are C++ and Python (I'm sorry C people, but I'm not lowering myself to your language when it will compile as valid C++ anyway...).  This is easily extensible under the current framework.  Each module simply requires:
 - A client library.
 - A build & run script.


Current Modules
---------------

- C++
 - CPP Zero (cpp_0)
  - My best attempt at a clean interface.
 - CPP PHAIS (cpp_1)
  - The interface originally provided in PHAIS, for legacy.

- Python
 - Python2 Zero (py2_0)
  - Interface adapted from Matthew Stark's (matts1) python blockade client.
 - Python3 Zero (py3_0)
  - Identical interface to Python2 Zero, and identical implementation.


