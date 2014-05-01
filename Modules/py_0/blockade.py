#!/usr/bin/python
import sys
import traceback
import ioblock


# Functions for IO
def read(var_type = None):
	try:
		if var_type is None:
			return sys.stdin.readline()
		else:
			return list(map(var_type, sys.stdin.readline().split()))

	except Exception as e:
		panic("Failed to read data from the arbiter.")


def write(*variables):
	try:
		sys.stdout.write(' '.join(map(str, variables)) + '\n')
		sys.stdout.flush()
	
	except Exception as e:
		panic("Failed to write data to the arbiter.")


def log(message = ""):
	sys.stderr.write(str(message) + '\n')
	sys.stderr.flush()

def panic(message, exception = None):
	log("-----------------------------------------------------------")
	log("The module has encountered an error and will terminate.")
	log("-----------------------------------------------------------")
	log("Description: " + message)
	if exception is not None:
		log()
		traceback.print_exc(10, sys.stderr)
	log("-----------------------------------------------------------")
	sys.exit(1)



if len(sys.argv) != 2:
	panic("The module requires exactly one parameter - the python file (without extension) that implements the 'move' function.")


try:
	with ioblock.blocker:
		bot = __import__(sys.argv[1])
	
except Exception as e:
	panic("Error importing the bot:", e)


for req in ('move', 'name', 'color'):
	if not hasattr(bot, req):
		panic("The bot does not provide the required '%s' function." % req)


try:
	with ioblock.blocker:
		name = bot.name()
	
	if type(name) is not str:
		panic('The name provided by the bot is of type "%s" - not a string.' % str(type(name)))
	if not all(c.isalnum() or c == '_' for c in name):
		panic('The name provided by the bot contains invalid characters - only letters, numbers and underscores are permitted.')
	
	if len(name) > 20:
		name = name[:20]
	if len(name) == 0:
		name = 'Bot'
	
	write(name)

except Exception as e:
	panic("Error in retreiving the bot's name:", e)


try:
	with ioblock.blocker:
		r, g, b = bot.color()

	# Check if color is valid:
	for comp_name, comp in (('red', r), ('green', g), ('blue', b)):
		if type(comp) is not int:
			panic("The color provided by the bot is invalid - component '%s' had type '%s' instead of 'int'."
			      % (comp_name, str(type(comp))))
		if comp < 0 or comp > 255:
			panic("The color provided by the bot is invalid - component '%s' had value '%d', which was not in the range 0-255."
			      % (comp_name, comp))

	# Send color to the arbiter!
	write(r,g,b)

except Exception as e:
	panic("Error in retreiving the bot's color:", e)


try:
	size, pid = read(int)
except Exception as e:
	panic("Error in reading the board size and this bot's id:", e)

try:
	x0, y0 = read(int)
	x1, y1 = read(int)
except Exception as e:
	panic("Error in reading the positions of each bot:", e)


pos = [(x0, size-1 - y0), (x1, size-1 - y1)] if pid else [(x0,y0), (x1,y1)]

grid = [[False] * size for i in range(size)]

moves = [dict((s,i) for i,s in enumerate('URDL')), dict((s,i) for i,s in enumerate('DRUL'))]


running = True
while running:

	command = read(str)

	if command[0] == 'move':
		player, x, y = map(int, command[1:])
		if pid == 1: y = size-1 - y
		pos[player] = (x,y)

	elif command[0] == 'block':
		player, x, y = map(int, command[1:])
		if pid == 1: y = size-1 - y
		grid[y][x] = True

	elif command[0] == 'turn':
		
		try:
			with ioblock.blocker:
				action = bot.move(list(map(list,grid)), pos[pid][0], pos[pid][1], pos[1-pid][0], pos[1-pid][1])
		except Exception as e:
			panic("Error in retrieving the bot's move:", e)

		if type(action) not in (str, tuple):
			panic("Invalid action returned:  Action must be a string or a tuple, but had type '%s'." % str(type(action)))

		if type(action) is str:
			if action not in moves[0]:
				panic("Invalid movement '%s' returned:  Moves must be one of U, D, L, or R." % action)
			
			write("move", moves[pid][action])

		else:
			if len(action) != 2 or not all(type(i) is int for i in action):
				panic("Invalid block action '%s' specified.  Block actions must be a tuple with two integers, the x and y coordinates of the square - (x, y)." % str(action))
			
			x,y = action
			if pid: y = size-1 - y
			write("block", x, y)

	elif command[0] == 'end':
		running = False

	else:
		panic("Unrecognised command '%s' received in the module.  Please report this error." % command[0])
