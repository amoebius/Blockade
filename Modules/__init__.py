#!/usr/bin/python

import importlib
import os
import tempfile

modules = {
	'cpp_0': '.cpp',
	'cpp_1': '.cpp',
	'py2_0': '.py',
	'py3_0': '.py',
}

registry = dict((module, importlib.import_module('.' + module, __name__)) for module in modules)


def _extension(module):
	return modules[module]

def _assertModuleExists(module):
	if module not in modules:
		raise ValueError("Specified module '%s' not found in registry of modules." % module)

def _export(name, value):
	return 'export "%s=%s";' % (name, value)


def filename(module, code):

	_assertModuleExists(module)

	with tempfile.NamedTemporaryFile(mode = 'w', prefix = 'blockade_tmp', suffix = _extension(module), delete = False) as file:

		file.write(code)
		return file.name

	
def config(module, filename, basepath = ''):
	
	_assertModuleExists(module)
	
	try:

		basepath = os.path.join(basepath, module)
		files = [filename] + [os.path.join(basepath, file) for file in registry[module].files]
		run = registry[module].run % os.path.basename(filename)

	except Exception as e:
		raise RuntimeError("Encountered error attempting to read module-specific configuration for '%s'." % module)

	return _export('files', ' '.join(files)) + _export('run', run)
	
