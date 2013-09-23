#include <iostream>
#include <queue>
#include <algorithm>

#include <cstdlib>
#include <ctime>

#include "blockade.h"

#define ME 1
#define YOU 2

using namespace std;

struct Node
{
    int r, c;
    Node(int a, int b): r(a), c(b) {}
};

int boardSize;
int myId, yourId;
int myR, myC, yourR, yourC;
int playerAt[70][70];
bool blocked[70][70];

const int dr[] = {-1, 0, 1, 0};
const int dc[] = {0, 1, 0, -1};
int dir[70][70];
int dist[70][70];

int turn;

int rMove[] = {0, 1, 2, 3};
int rRow[100];
int rCol[100];

//negative -> towards moving
//positive -> towards blocking
int something(int myD, int yourD)
{
    //yet to be implemented
    return 1 - turn / (boardSize * 5);
}

void move(int dir)
{
    if (myId == 0)
        movePosition((dir + 2) % 4);
    else
        movePosition(dir);
}

//bfs return dist, dir
pair<int, int> bfs(int r, int c, int g)
{
    for (int i = 0; i < boardSize; i++)
    for (int j = 0; j < boardSize; j++)
    {
        dir[i][j] = -2;
        dist[i][j] = -1;
    }

    queue<Node> q;
    q.push(Node(r, c));
    dir[r][c] = -1;
    dist[r][c] = 0;
    while (!q.empty())
    {
        Node node = q.front();
        q.pop();
        if (node.r == g)
            return make_pair(dist[node.r][node.c], dir[node.r][node.c]);
        random_shuffle(rMove, rMove + 4);
        for (int i = 0; i < 4; i++)
        {
            int nr = node.r + dr[rMove[i]];
            int nc = node.c + dc[rMove[i]];
            if (nr < 0 || nr >= boardSize || nc < 0 || nc >= boardSize)
                continue;
            if (blocked[nr][nc])
                continue;
            if (dir[nr][nc] != -2)
                continue;
            if (dir[node.r][node.c] == -1)
                dir[nr][nc] = rMove[i];
            else dir[nr][nc] = dir[node.r][node.c];
            dist[nr][nc] = dist[node.r][node.c] + 1;
            q.push(Node(nr, nc));
        }
    }
    return make_pair(-1, -1);
}

int virt(int c)
{
    if (myId == 0)
        return boardSize - c - 1;
    else
        return c;
}

void clientRegister()
{
    setName("AustiBot");
    setColour(255, 255, 0);
}

void clientInit(int s, int id)
{
    boardSize = s;
    myId = id;
    myR = boardSize - 1;
    myC = boardSize / 2;
    yourR = 0;
    yourC = boardSize / 2;
    yourId = !id;

    for (int i = 0; i < 70; i++)
    for (int j = 0; j < 70; j++)
    {
        playerAt[i][j] = 0;
        blocked[i][j] = 0;
        rRow[i] = i;
        rCol[i] = i;
    }
    swap(rRow[boardSize - 1], rRow[0]);
    playerAt[boardSize - 1][boardSize / 2] = ME;
    playerAt[0][boardSize / 2] = YOU;
    srand(time(0));
    turn = 0;
}

void clientPlayerPosition(int pid, int c, int r)
{
    c = virt(c);
    r = virt(r);
    if (pid == myId)
    {
        playerAt[myR][myC] &= !ME;
        playerAt[r][c] |= ME;
        myR = r;
        myC = c;
    }
    else
    {
        playerAt[yourR][yourC] &= !YOU;
        playerAt[r][c] |= YOU;
        yourR = r;
        yourC = c;
    }
}

void clientSquareIsBlocked(int pid, int c, int r)
{
    r = virt(r);
    c = virt(c);
    blocked[r][c] = true;
}

void block(int r, int c)
{
    blockSquare(virt(c), virt(r));
}

void clientDoTurn()
{
    if (turn == 0)
    {
        move(0);
        turn++;
        return;
    }
    cerr << turn << ' ';
    int moveDir = bfs(myR, myC, 0).second;
    int myD = bfs(myR, myC, 0).first;
    int yourD = bfs(yourR, yourC, boardSize - 1).first;
    int br, bc, delta = myD - yourD + something(myD, yourD);
    random_shuffle(rRow + 1, rRow + boardSize);
    random_shuffle(rCol, rCol + boardSize);
    for (int i = 0; i < boardSize; i++)
    for (int j = 0; j < boardSize; j++)
    {
        int r = rRow[i];
        int c = rCol[j];
        if (blocked[r][c]) continue;
        if (playerAt[r][c]) continue;
        blocked[r][c] = true;
        myD = bfs(myR, myC, 0).first;
        yourD = bfs(yourR, yourC, boardSize - 1).first;
        if (myD != -1 && yourD != -1)
        {
            int nDelta = myD - yourD;
            if (nDelta < delta || (yourD == 1 && moveDir != -1))
            {
                moveDir = -1;
                br = r;
                bc = c;
                delta = nDelta;
            }
        }
        blocked[r][c] = false;
    }
    if (moveDir == -1)
    {
        block(br, bc);
    }
    else
    {
        move(moveDir);
    }
    turn++;
}
