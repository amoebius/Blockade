#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <queue>
#include <unistd.h>
#include <algorithm>
using std::random_shuffle;
using std::queue;

#include "blockade.h"

int boardSize;
int myID;
bool isBlocked[MAX_SIZE][MAX_SIZE];
bool theyBlocked = false;
int turnsTheyMoved = 0;
int dickishness;

struct Point {
   int row, col;
   Point() {}
   Point(int row, int col) {
      this->row = row;
      this->col = col;
   }
};

struct BFSState {
   Point p;
   int len;
   BFSState() {}
   BFSState(int row, int col, int len) {
      this->p.row= row;
      this->p.col = col;
      this->len = len;
   }
};

Point playerPos[2];
int target[2];

int dx[] = {0, 1, 0, -1};
int dy[] = {-1, 0, 1, 0};
int d[] = {0, 1, 2, 3};

int BFS(Point p, int targetRow) {
   bool seen[MAX_SIZE][MAX_SIZE];
   for (int i = 0; i < MAX_SIZE; i++)
      for (int j = 0; j < MAX_SIZE; j++)
         seen[i][j] = false;
   queue<BFSState> q;
   q.push(BFSState(p.row, p.col, 0));
   while (!q.empty()) {
      BFSState s = q.front();
      if (s.p.row == targetRow) return s.len;
      q.pop();
      for (int dir = 0; dir < 4; dir++) {
         BFSState next = BFSState(s.p.row + dy[dir], s.p.col + dx[dir], s.len+1);
         if (next.p.row < 0 || next.p.row >= boardSize || next.p.col < 0 || next.p.col >= boardSize) continue;
         if (isBlocked[next.p.row][next.p.col]) continue;
         if (seen[next.p.row][next.p.col]) continue;
         seen[next.p.row][next.p.col] = true;
         q.push(next);
      }
   }
   return SHRT_MIN;
}

bool canPlaceBlockHere(int row, int col) {
   if (isBlocked[row][col]) return false;
   if (playerPos[myID].row == row && playerPos[myID].col == col) return false;
   if (playerPos[!myID].row == row && playerPos[!myID].col == col) return false;
   isBlocked[row][col] = true;
   if (BFS(playerPos[myID], target[myID]) == SHRT_MIN || BFS(playerPos[!myID], target[!myID]) == SHRT_MIN) {
      isBlocked[row][col] = false;
      return false;
   }
   isBlocked[row][col] = false;
   return true;
}

bool isMoveValid(int dir, Point p) {
   int nr = p.row + dy[dir];
   int nc = p.col + dx[dir];
   if (nc < 0 || nc >= boardSize || nr < 0 || nr >= boardSize) return false;
   if (isBlocked[nr][nc]) return false;
   return true;
}

int getAnyValidMove() {
   for (int dir = 0; dir < 4; dir++) {
      if (isMoveValid(dir, playerPos[myID])) return dir;
   }
}

void clientRegister(void) {
   setName("Deckard");
   setColour(128, 128, 128);
   srand(time(NULL));
}

void clientInit(int boardS, int playerID) {
   target[1] = 0;
   target[0] = boardS-1;
   theyBlocked = false;
   turnsTheyMoved = 0;
   dickishness = 0;

   boardSize = boardS;
   myID = playerID;
   for (int i = 0; i < boardS; i++) {
      for (int j = 0; j < boardS; j++) {
         isBlocked[i][j] = false;
      }
   }
}

void clientPlayerPosition(int pid, int x, int y) {
   playerPos[pid].row = y;
   playerPos[pid].col = x;
}

void clientSquareIsBlocked(int pid, int x, int y) {
   if (pid == !myID) turnsTheyMoved--;
   isBlocked[y][x] = true;
   if (pid == !myID) {
      theyBlocked = true;
      dickishness += 25;
   }
}

int bestrow = -1, bestcol = -1;
int mbr;

void blockAtBestPlaces(int size) {
   bestrow = -1;
   bestcol = -1;
   mbr = INT_MIN/3;
   int add = 1;
   int start = 0;
   int end = boardSize;

   for (int i = start; i != end-size+1; i += add) {
      for (int j = 0; j < end-size+1; j++) {
         bool doContinue=false;
         for (int x = i; x < i+size; x++) {
            for (int y = j; y < j+size; y++) {
               if (isBlocked[x][y]) doContinue=true;
               if (x == playerPos[myID].row && y == playerPos[myID].col) doContinue = true;
               if (x == playerPos[!myID].row && y == playerPos[!myID].col) doContinue = true;
            }
         }
         if (doContinue) continue;
         for (int x = i; x < i+size; x++) {
            for (int y = j; y < j+size; y++) {
               isBlocked[x][y] = true;
            }
         }
         int mylen;
         if ((mylen = BFS(playerPos[myID], target[myID])) == SHRT_MIN) {
            for (int x = i; x < i+size; x++) {
               for (int y = j; y < j+size; y++) {
                  isBlocked[x][y] = false;
               }
            }
            continue;
         }
         int theirlen;
         if ((theirlen = BFS(playerPos[!myID], target[!myID])) == SHRT_MIN) {
            for (int x = i; x < i+size; x++) {
               for (int y = j; y < j+size; y++) {
                  isBlocked[x][y] = false;
               }
            }
            continue;
         }
         for (int x = i; x < i+size; x++) {
            for (int y = j; y < j+size; y++) {
               isBlocked[x][y] = false;
            }
         }
         int diff = theirlen - mylen;
         if (diff > mbr) {
            mbr = diff;
            bestrow = i;
            bestcol = j;
         }
      }
   }
}

void blockAtBestPlace() {
   bestrow = -1;
   bestcol = -1;
   mbr = INT_MIN/3;
   int add = -1;
   int start = boardSize-1;
   int end = -1;
   if (myID == 0) {
      start = 0;
      end = boardSize;
      add = 1;
   }

   for (int i = start; i != end; i += add) {
      for (int j = 0; j < boardSize; j++) {
         if (isBlocked[i][j]) continue;
         if (i == playerPos[myID].row && j == playerPos[myID].col) continue;
         if (i == playerPos[!myID].row && j == playerPos[!myID].col) continue;
         isBlocked[i][j] = true;
         int mylen;
         if ((mylen = BFS(playerPos[myID], target[myID])) == SHRT_MIN) {
            isBlocked[i][j] = false;
            continue;
         }
         int theirlen;
         if ((theirlen = BFS(playerPos[!myID], target[!myID])) == SHRT_MIN) {
            isBlocked[i][j] = false;
            continue;
         }
         isBlocked[i][j] = false;
         int diff = theirlen - mylen;
         if (diff > mbr) {
            mbr = diff;
            bestrow = i;
            bestcol = j;
         }
      }
   }
}

void clientDoTurn() {
   turnsTheyMoved++;
   int best = getAnyValidMove();
   int bestlen = SHRT_MAX;
   blockAtBestPlace();
   if (mbr > BFS(playerPos[!myID], target[!myID]) - BFS(playerPos[myID], target[myID])) {
      for (int i = 0; i < boardSize; i++) {
         for (int j = 0; j < boardSize; j++) {
            if (isBlocked[i][j]) printf("#");
            else printf(".");
         }
         printf("\n");
      }
      printf("\n");
      if (bestrow == -1) movePosition(getAnyValidMove());
      else {
         blockSquare(bestcol, bestrow);
      }
      return;
   }
   for (int i = 2; i <= 3; i++) {
      blockAtBestPlaces(2);
      if (mbr > BFS(playerPos[!myID], target[!myID]) - BFS(playerPos[myID], target[myID]) && canPlaceBlockHere(bestrow, bestcol)) {
         printf("Blocking best places\n");
         printf("Row: %d Col %d\n", bestrow, bestcol);
         printf("%d\n", isBlocked[bestrow][bestcol]);
         printf("Their Row: %d Col %d\n", playerPos[!myID].row, playerPos[!myID].col);
         isBlocked[bestrow][bestcol] = true;
         for (int i = 0; i < boardSize; i++) {
            for (int j = 0; j < boardSize; j++) {
               if (isBlocked[i][j]) printf("#");
               else printf(".");
            }
            printf("\n");
         }
         printf("\n");
         if (bestrow == -1) movePosition(getAnyValidMove());
         else {
            blockSquare(bestcol, bestrow);
         }
         return;
      }
   }
   random_shuffle(d, d+4);
   for (int i = 0; i < 4; i++) {
      int dir = d[i];
      if (!isMoveValid(dir, playerPos[myID])) continue;
      Point p = Point(playerPos[myID].row + dy[dir], playerPos[myID].col + dx[dir]);
      int len = BFS(p, target[myID]);
      if (len < bestlen) {
         bestlen = len;
         best = dir;
      }
   }
   movePosition(best);
   theyBlocked = false;
}
