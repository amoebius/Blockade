#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "blockade.h"
#define TOP_PID 0
#define BOT_PID 1

#define range(i,a,b) for(int i=(a),_b=(b); i<_b; i++)
#define fo(i,n) range(i,0,n)

#include <cassert>
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <utility>
using namespace std;
typedef pair<int,int> pr;
typedef pair<int,pr> state;
#define _x second.first
#define _y second.second
#define _d first

int width;
int me,them;
bool isBlocked[MAX_SIZE][MAX_SIZE];
int px[2], py[2];
int pdist[2][MAX_SIZE][MAX_SIZE], vdist[2];
const int oo = 1<<28;

int dx[] = {0, 1, 0, -1, 1, -1, 1, -1};
int dy[] = {-1, 0, 1, 0, 1, 1, -1, -1};

void clientRegister() {
	setName("Moebius_Omega");
	srand(time(NULL));
	setColour(rand()%255,rand()%255,255);
}

void clientInit(int boardSize, int playerID) {
	width = boardSize;
	me = playerID;
	them = !me;
	fo(x,width) fo(y,width) isBlocked[x][y] = false;
	fo(i,2) fo(x,width) fo(y,width) pdist[i][x][y] = oo;
	fo(x,width) pdist[1][x][0] = pdist[0][x][width-1] = 0;
}

void clientPlayerPosition(int pid, int x, int y) {
	px[pid]=x;
	py[pid]=y;
}

inline void setBlocked(int x, int y, bool blocked) {
	if(isBlocked[x][y] != blocked) fo(id,2) vdist[id] = min(vdist[id],pdist[id][x][y]);
	isBlocked[x][y] = blocked;
}

void clientSquareIsBlocked(int pid, int x, int y) {
	setBlocked(x,y,true);
}

inline bool inBounds(int x, int y) {
	return x>=0 && x<width && y>=0 && y<width;
}
inline int endRow() {
	return me ? 0 : width-1;
}
inline int homeRow() {
	return me ? width-1 : 0;
}
inline int goalRow(int id) {
	return id ? 0 : width-1;
}

int seen[MAX_SIZE][MAX_SIZE], seen_check;
int player_dist(int id) {
	seen_check++;
	int x = px[id], y = py[id], nx,ny;
	if(pdist[id][x][y]<=vdist[id]) return pdist[id][x][y];
	pdist[id][x][y] = oo;
	queue<pr> q; stack<pr> s;
	q.push(pr(x,y));
	seen[x][y] = seen_check;
	while(q.size()) {
		x = q.front().first; y = q.front().second;
		q.pop();
		if(pdist[id][x][y]>vdist[id]) {
			pdist[id][x][y] = oo;
			fo(i,4) {
				nx=x+dx[i]; ny=y+dy[i];
				if(inBounds(nx,ny) && !isBlocked[nx][ny] && seen[nx][ny]!=seen_check) q.push(pr(nx,ny)), seen[nx][ny]=seen_check;
			}
			s.push(pr(x,y));
		} else break;
	}
	while(s.size()) {
		x = s.top().first; y = s.top().second;
		s.pop();
		fo(i,4) {
			nx = x+dx[i]; ny = y+dy[i];
			if(!isBlocked[nx][ny] && inBounds(nx,ny)) if(pdist[id][x][y] > pdist[id][nx][ny]+1) pdist[id][x][y] = pdist[id][nx][ny]+1;
		}
	}
	return pdist[id][px[id]][py[id]];
}

void recreate_dist(int id) {
	seen_check++;
	int x = px[id], y = py[id], nx,ny;
	queue<pr> q; stack<pr> s;
	q.push(pr(x,y));
	seen[x][y] = seen_check;
	pdist[id][x][y] = oo;
	while(q.size()) {
		x = q.front().first; y = q.front().second;
		q.pop();
		if(!pdist[id][x][y]) continue;
		pdist[id][x][y] = oo;
		fo(i,4) {
			nx=x+dx[i]; ny=y+dy[i];
			if(inBounds(nx,ny) && !isBlocked[nx][ny] && seen[nx][ny]!=seen_check) q.push(pr(nx,ny)), seen[nx][ny]=seen_check;
		}
		if(pdist[id][x][y]>vdist[id]) s.push(pr(x,y));
	}
	while(s.size()) {
		x = s.top().first; y = s.top().second;
		s.pop();
		fo(i,4) {
			nx = x+dx[i]; ny = y+dy[i];
			if(!isBlocked[nx][ny] && inBounds(nx,ny)) if(pdist[id][x][y] > pdist[id][nx][ny]+1) pdist[id][x][y] = pdist[id][nx][ny]+1;
		}
	}
	vdist[id] = oo;
}

const int BLOCKING = 2*oo;
int diffdist(int id) {
	int d1 = player_dist(!id), d2 = player_dist(id);
	if(d1==oo || d2==oo) return BLOCKING;
	return d1-d2;
}

const int MAX_DEPTH = 2;
int evaluate(int id, int bx=-1, int by=-1, int bx2=-1, int by2=-1, int depth=1) {
	if(py[id]==goalRow(id)) return oo;
	if(py[!id]==goalRow(!id)) return -oo;
	if(depth==MAX_DEPTH) return diffdist(id);

	int bestval = -BLOCKING, num=1,nx,ny;
	range(i,0,8) {
		if(bx!=-1) {
			nx = bx+dx[i]; ny = by+dy[i];
			if(inBounds(nx,ny) && !isBlocked[nx][ny] && (nx!=px[me]||ny!=py[me]) && (nx!=px[them]||ny!=py[them])) {
				setBlocked(nx,ny,true);
				bestval = max(bestval,-evaluate(!id,bx2,by2,nx,ny,depth+1));
				setBlocked(nx,ny,false);
			}
		}
		nx = px[id]+dx[i]; ny = py[id]+dy[i];
		if(inBounds(nx,ny) && !isBlocked[nx][ny] && (nx!=px[me]||ny!=py[me]) && (nx!=px[them]||ny!=py[them])) {
			setBlocked(nx,ny,true);
			bestval = max(bestval,-evaluate(!id,bx2,by2,nx,ny,depth+1));
			setBlocked(nx,ny,false);
		}
		nx = px[!id]+dx[i]; ny = py[!id]+dy[i];
		if(inBounds(nx,ny) && !isBlocked[nx][ny] && (nx!=px[me]||ny!=py[me]) && (nx!=px[them]||ny!=py[them])) {
			setBlocked(nx,ny,true);
			bestval = max(bestval,-evaluate(!id,bx2,by2,nx,ny,depth+1));
			setBlocked(nx,ny,false);
		}
	}
	fo(i,4) {
		px[id]+=dx[i]; py[id]+=dy[i];
		if(inBounds(px[id],py[id]) && !isBlocked[px[id]][py[id]]) bestval = max(bestval, -evaluate(!id,bx2,by2,-1,-1,depth+1));
		px[id]-=dx[i]; py[id]-=dy[i];
	}
	if(bestval==-BLOCKING) {
		return BLOCKING;
	}
	return bestval;
}

int action,best_value,block_x,block_y,dir;
const int ACTION_BLOCK = 1, ACTION_MOVE = 0;
int bestMove() {
	best_value = -oo;
	int num = 1;
	action = ACTION_BLOCK;
	fo(tx,width) fo(ty,width) if(!isBlocked[tx][ty] && (tx!=px[me]||ty!=py[me]) && (tx!=px[them]||ty!=py[them])) {
		setBlocked(tx,ty,true);
		fo(i,2) recreate_dist(i);
		int value = -evaluate(them,-1,-1,tx,ty);
		if(value > best_value) {
			best_value = value;
			block_x = tx; block_y = ty;
			num=2;
		} else if(value == best_value) {
			if(rand()%num == 0) {
				num++;
				block_x=tx;
				block_y=ty;
			}
		}
		setBlocked(tx,ty,false);
	}
	int dd[4]; fo(i,4) dd[i]=i;
	random_shuffle(dd,dd+4);
	fo(i,4) {
		int nx = px[me]+dx[dd[i]], ny = py[me]+dy[dd[i]];
		if(inBounds(nx,ny) && !isBlocked[nx][ny]) {
			int ox = px[me], oy = py[me];
			px[me]=nx; py[me]=ny;
			int value = -evaluate(them);
			if(value > best_value) {
				best_value = value;
				action = ACTION_MOVE;
				dir = dd[i];
			}
			px[me]=ox; py[me]=oy;
		}
	}
}


void clientDoTurn() {

	fo(i,2) recreate_dist(i);

	/*
	fo(i,2) if(i==me) {
		fo(y,width) {
			fo(x,width) if(pdist[i][x][y]<oo) cout<<pdist[i][x][y]<<' '; else cout<<"- ";
			cout<<'\n';
		}
		cout<<"\n\n";
	} cout<<"\n\n";
	*/

	bestMove();
	cout<<best_value<<'\n';
	cout<<player_dist(me)<<'\n';
	if(action==ACTION_MOVE) {
		movePosition(dir);
	} else {
		blockSquare(block_x, block_y);
	}

}


