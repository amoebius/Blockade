Blockade
========

A module for running 'Blockade' bots against each other.
See http://tinyurl.com/los8svs for example gameplay between bots.

Currently implemented:
 - Arbiter:
  - Communication protocol
  - Initial arbiter
  - Initial subprocess handler (lacking sandboxing and error handling)
  - Subprocess communication
 - Interfaces:
  - "blockade.h", from April 2013 PHAIS
  - Nicer alternative C++ interface
  - Python interface
 - Visualiser (command-line, testing only)
 - Functor-based threading library (well stolen from another project)

 
TODO:
 - Arbiter
  - Handle subprocess errors, timeouts
  - Terminate excessively long games, and programs that take too long to move
  - Capture bot output through modules' stderr
 - Sandboxer
  - Apparently non-trivial!  Currently sandboxing is just ignored
  - 'libsandbox' and 'pysandbox' are options
 - Modules
  - Redirect stdout to stderr for capturing
  - Python module
   - Block stdin/stdout for userland, and redirect to stderr
 - Alternative python interface?
 - Website:
  - User accounts and storage......
  - Tournament system...........
 - Support for other languages?