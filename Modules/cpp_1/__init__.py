#!/usr/bin/python

_headers = ['blockade.h', 'ioblock/ioblock.hpp']
_source = ['blockade.cpp', 'ioblock/ioblock.cpp']
_scripts = ['compile.sh', 'run.sh']

files = _headers + _source + _scripts
run = 'run.sh %s'
