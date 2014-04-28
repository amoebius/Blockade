from sys import argv

if len(argv) != 2:
	raise RuntimeError("The module requires exactly one parameter - the python file (without extension) that implements the 'move' function.")

bot = __import__(argv[1])


for req in ('move', 'name', 'color'):
	if not hasattr(bot, req):
		raise RuntimeError("The bot does not provide the required '%s' function." % req)


try:
	name = bot.name()
	
	if type(name) is not str:
		raise Exception('The provided name is not a string.')
	if not all(c.isalnum() or c == '_' for c in name):
		raise Exception('Name contains invalid characters - only letters, numbers and underscores are permitted.')
	
	if len(name) > 20:
		name = name[:20]
	if len(name) == 0:
		name = 'Bot'
	
	print name

except Exception as e:
	raise RuntimeError("Error in retreiving the bot's name: " + str(type(e)) + " - " + str(e))


try:
	r, g, b = bot.color()
	assert(type(component) is int and 0 <= component < 255 for component in (r,g,b))

	print '%d %d %d' % (r,g,b)

except Exception as e:
	raise RuntimeError("Error in retreiving the bot's color: " + str(type(e)) + " - " + str(e))

# TEMPPP
from sys import stderr
stderr.write("RECEIVING SIZE AND PID\n")
stderr.flush()
size, pid = map(int, raw_input().split())
stderr.write("RECEIVING X0 AND Y0\n")
stderr.flush()
x0, y0 = map(int, raw_input().split())
stderr.write("RECEIVING X1 AND Y1\n")
stderr.flush()
x1, y1 = map(int, raw_input().split())

pos = [(x0, size-1 - y0), (x1, size-1 - y1)] if pid else [(x0,y0), (x1,y1)]

grid = [[False] * size for i in range(size)]

moves = [dict((s,i) for i,s in enumerate('URDL')), dict((s,i) for i,s in enumerate('DRUL'))]


running = True
while running:

	stderr.write("AWAITING COMMAND\n")
	stderr.flush()

	command = raw_input().split()

	stderr.write("COMMAND RECEIVED: " + str(command) + "!\n")
	stderr.flush()

	if command[0] == 'move':
		player, x, y = map(int, command[1:])
		if pid: y = size-1 - y
		pos[player] = (x,y)

	elif command[0] == 'block':
		player, x, y = map(int, command[1:])
		if pid: y = size-1 - y
		grid[y][x] = True

	elif command[0] == 'turn':
		
		try:
			action = bot.move(map(list,grid), pos[pid], pos[1-pid])
		except Exception as e:
			raise RuntimeError("Error in retrieving the bot's move: " + str(type(e)) + " - " + str(e))

		if type(action) not in (str, tuple):
			raise RuntimeError("Invalid action returned:  Action must be a string or a tuple.")

		if type(action) is str:
			if action not in moves[0]:
				raise RuntimeError("Invalid movement '%s' specified.  Moves must be one of U, D, L, or R." % action)
			
			print "move %d" % moves[pid][action]

		else:
			if len(action) != 2 or not all(type(i) is int for i in action):
				raise RuntimeError("Invalid block action '%s' specified.  Block actions must be a tuple with two integers, the x and y coordinates of the square - (x, y)." % str(action))
			
			x,y = action
			if pid: y = size-1 - y
			print "block %d %d" % (x,y)

	else:
		running = False
