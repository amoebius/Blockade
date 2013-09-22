Blockade
========

A website for submitting and running 'Blockade' bots.
See http://tinyurl.com/los8svs for example gameplay between bots.

Currently implemented:
 - Communication protocol with game 'Arbiter'
 - Implementation of interface for standard C++ "blockade.h", from April 2013 PHAIS
 - Nicer alternative C++ interface
 - Initial arbiter (does not handle subprocess errors or sandboxing)
 - Visualiser (command-line, testing only)
 - Functor-based threading library (well stolen from another project)

 
TODO:
 - Arbiter
  - Handle subprocess errors, timeouts
  - Terminate excessively long games, and programs that take too long to move
 - Sandboxer
  - Apparently non-trivial!  Currently sandboxing is just ignored
  - 'libsandbox' and 'pysandbox' are options
 - Python interface
  - Standard PHAIS interface
  - matts1 (see github user) interface
  - Moebius interface
 - Website...
 - User accounts and storage......
 - Tournament system...........
 - Other languages?
 - Anything else you can think of?