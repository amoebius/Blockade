name = lambda: "Gaff"
color = lambda: (200, 215, 215)

from collections import deque
from random import randint

d = ((0,1), (0,-1), (1,0), (-1,0))
moves = dict(zip('UDLR', d))

dickishness = 2

c = lambda b: map(list,b)
def b(board, (x,y)):
	nboard = c(board)
	nboard[y][x] = True
	return nboard

def dist(board, (x,y), goal):
	q = deque()
	q.append((0,x,y))
	board[y][x] = True
	s = len(board)
	while q:
		d,x,y = q.popleft()
		if y == goal: return d
		for dx,dy in d:
			if 0 <= x+dx < s and 0 <= y+dy < s:
				if not board[y+dy][x+dx]:
					q.append((d+1,x+dx,y+dy))
	return -1

def randmin(iterable):
	iterator = iter(iterable)
	low = next(iterator)
	count = 1
	for elem in iterator:
		if elem < low:
			low = elem
			count = 1
		elif elem == low:
			if randint(0,count) == 0: low = elem
			count += 1
	return low


def move(board, (x,y), (ox,oy)):
	
	s = len(board)
	dmove, move = randmin((d,m) for d,m in ((dist(c(board), (x+dx,y+dy), s-1), move) for move, (dx,dy) in moves.items() if 0 <= x+dx < s and 0 <= y+dy < s) if d >= 0)
	dmove -= dist(c(board), (ox,oy), 0)
	dblock, block = randmin((theirdist - mydist, block) for mydist, theirdist, block in ((dist(b(board,(bx,by)), (x,y), s-1), dist(b(board,(bx,by)), (ox,oy), 0), (bx,by)) for by in range(s) for bx in range(x) if not board[by][bx]) if mydist >= 0 and theirdist >= 0)

	return move if dmove > dickishness * dblock else block