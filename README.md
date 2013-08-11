Blockade
========

A website for submitting and running 'Blockade' bots.
See http://tinyurl.com/los8svs for example gameplay between bots.

Currently implemented:
 - Sketch of communication protocol with game 'Arbiter'
 - Implementation of interface for standard C++ "blockade.h", from April 2013 PHAIS
 - Nicer alternative C++ interface
 - Linux pipe iostream-like wrapper - completed (not really tested...)
 - Functor-based threading library (well stolen from another project)
 
TODO:
 - Arbiter
  - Libraries in place, straightforward implementation of protocol and game remaining.
 - Sandboxer
  - 'libsandbox' and 'pysandbox' appear to make this trivial - a 10-line python program that executes the specified program
 - Python interface
  - Standard PHAIS interface
  - matts1 (see github user) interface
  - Moebius interface
 - Website...
 - User accounts and storage......
 - Tournament system...........
 - Other languages?
 - Anything else you can think of?