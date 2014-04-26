import os
STDIN_FILENO = 0
STDOUT_FILENO = 1
STDERR_FILENO = 2

infd = os.dup(STDIN_FILENO)
outfd = os.dup(STDOUT_FILENO)
devnull = os.open("/dev/null", os.O_RD)

blocked = False

def block():
	if blocked: return
	blocked = True

	os.dup2(devnull, infd)
	os.dup2(STDERR_FILENO, STDOUT_FILENO)

def unblock():
	if not blocked: return
	blocked = False

	os.dup2(infd, STDIN_FILENO)
	os.dup2(outfd, STDOUT_FILENO)
