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
#include <algorithm>
#include <utility>
using namespace std;
typedef pair<int,int> pr;
typedef pair<int,pr> state;
#define _x second.first
#define _y second.second
#define _dir first

int width;
int myID;
bool seen[MAX_SIZE][MAX_SIZE];
bool isBlocked[MAX_SIZE][MAX_SIZE];
int px[2], py[2];

int dx[] = {0, 1, 0, -1};
int dy[] = {-1, 0, 1, 0};

void clientRegister() {
	setName("BFS_Bot");
	setColour(200,70,255);
}

void clientInit(int boardSize, int playerID) {
	width = boardSize;
	myID = playerID;
	fo(x,width) fo(y,width) isBlocked[x][y] = false;
	cout<<"SETTING ID TO "<<myID<<'\n';
}


void clientPlayerPosition(int pid, int x, int y) {
	px[pid]=x;
	py[pid]=y;
}

void clientSquareIsBlocked(int pid, int x, int y) {
	cout<<"Square ("<<x<<", "<<y<<") has been blocked\n";
	isBlocked[x][y] = true;
}

bool inBounds(int x, int y) {
	return x>=0 && x<width && y>=0 && y<width;
}
int endRow() {
	return myID ? 0 : width-1;
}
int homeRow() {
	return myID ? width-1 : 0;
}

int BFS() {
	fo(x,width) fo(y,width) seen[x][y] = isBlocked[x][y];
	int x = px[myID], y = py[myID];
	seen[x][y] = true;
	queue<state> q;
	fo(i,4) if(inBounds(x+dx[i],y+dy[i]) && !seen[x+dx[i]][y+dy[i]]) seen[x+dx[i]][y+dy[i]] = true, q.push(state(i,pr(x+dx[i],y+dy[i])));
	while(q.size()) {
		state s = q.front(); q.pop();
		if(s._y==endRow()) return s._dir;
		x = s._x;
		y = s._y;
		fo(i,4) if(inBounds(x+dx[i],y+dy[i]) && !seen[x+dx[i]][y+dy[i]]) seen[x+dx[i]][y+dy[i]] = true, q.push(state(s._dir,pr(x+dx[i],y+dy[i])));
	}

}

void clientDoTurn() {
	int dir = BFS();
	cout<<"Moving in dir "<<dir<<'\n';
	movePosition(dir);
}

