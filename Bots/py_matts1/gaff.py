name = lambda: "Gaff"
color = lambda: (200, 215, 215)

from collections import deque
from random import randint

d = ((0,1), (0,-1), (1,0), (-1,0))
moves = dict(zip('DURL', d))

dickishness = 1

c = lambda b: map(list,b)
def b(board, (x,y)):
	nboard = c(board)
	nboard[y][x] = True
	return nboard

def dist(board, (x,y), goal):
	q = deque()
	q.append((0,x,y))
	if board[y][x]: return -1
	board[y][x] = True
	s = len(board)
	while q:
		dist,x,y = q.popleft()
		if y == goal: return dist
		for dx,dy in d:
			if 0 <= x+dx < s and 0 <= y+dy < s:
				if not board[y+dy][x+dx]:
					board[y+dy][x+dx] = True
					q.append((dist+1,x+dx,y+dy))
	return -1

def randmin(iterable, key = lambda x: x[0] if type(x) is tuple else x):
	iterator = iter(iterable)
	low = next(iterator)
	count = 1
	for elem in iterator:
		if key(elem) < key(low):
			low = elem
			count = 1
		elif key(elem) == key(low):
			if randint(0,count) == 0: low = elem
			count += 1
	return low


def move(board, (x,y), (ox,oy)):
	
	s = len(board)
	dmove, move = randmin((d,m) for d,m in ((dist(c(board), (x+dx,y+dy), s-1), move) for move, (dx,dy) in moves.items() if 0 <= x+dx < s and 0 <= y+dy < s) if d >= 0)
	
	try:
		dblock, block = randmin((mydist - theirdist, block) for mydist, theirdist, block in ((dist(b(board,(bx,by)), (x,y), s-1), dist(b(board,(bx,by)), (ox,oy), 0), (bx,by)) for by in range(s) for bx in range(s) if not board[by][bx]) if mydist >= 0 and theirdist >= 0)
		dblock = dblock - (dmove+1 - dist(c(board), (ox,oy), 0))
	except StopIteration:
		return move

	return move if 0 <= dickishness * dblock else block
