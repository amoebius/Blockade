import os
import sys

STDIN_FILENO = 0
STDOUT_FILENO = 1
STDERR_FILENO = 2

infd = os.dup(STDIN_FILENO)
outfd = os.dup(STDOUT_FILENO)
devnull = os.open("/dev/null", os.O_RDONLY)


blocked = False

def block():
	global blocked
	if blocked: return
	blocked = True

	sys.stdout.flush()
	os.dup2(devnull, STDIN_FILENO)
	os.dup2(STDERR_FILENO, STDOUT_FILENO)

def unblock():
	global blocked
	if not blocked: return
	blocked = False

	sys.stdout.flush()
	os.dup2(infd, STDIN_FILENO)
	os.dup2(outfd, STDOUT_FILENO)


class blocker_class(object):

	def __enter__(self):
		block()
		return blocker

	def __exit__(self, object_type, exception, traceback):
		unblock()
		return exception is None

blocker = blocker_class()
