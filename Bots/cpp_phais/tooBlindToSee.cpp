#include <queue>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "blockade.h"
#define TRUE  1
#define FALSE 0
#define BLOCKED 1
#define CLEAR   0

#define CONTROL 0
#define CHAOS   1

using namespace std;

int board_size;
int my_id;

int dx[] = {0,1,0,-1};
int dy[] = {-1,0,1,0};

struct player_t {
    int x,y;
    player_t() {}
    player_t(int x, int y) {
        this->x = x;
        this->y = y;
    }
};

struct bfs_state_t {
    int x,y,dir;
    bfs_state_t() {}
    bfs_state_t(int x, int y, int dir) {
        this->x = x;
        this->y = y;
        this->dir = dir;
    }
};

player_t control;
player_t chaos;

int moved;

int map[MAX_SIZE][MAX_SIZE];

int clamp(int val);
int bfs_to_end(int player);
int bfs_gets_there(int player, int x_block, int y_block);

void clientRegister() {
    setName("tooBlindToSee");
    setColour(0x30, 0x30, 0x30);
    srand(time(NULL));
}

void clientInit(int boardSize, int playerID) {
    moved = FALSE;
    board_size = boardSize;
    my_id = playerID;
    int x,y;
    for (x = 0; x < MAX_SIZE; x++) {
        for (y = 0; y < MAX_SIZE; y++) {
            map[x][y] = CLEAR;
        }
    }
}

void clientPlayerPosition(int pid, int x, int y) {
    if (pid == my_id) {
        control = player_t(x, y);
    } else {
        chaos = player_t(x, y);
    }
}

void clientSquareIsBlocked(int pid, int x, int y) {
    map[x][y] = BLOCKED;
}
void clientDoTurn() {
    if (!moved) {
        moved = TRUE;
        blockSquare(control.x-1, control.y);
        return;
    }

    int x, y;

    int best_optimalness = 0;
    int best_x = 0;
    int best_y = 0;
    int chaos_original = bfs_gets_there(CHAOS, -1, -1);
    int control_original = bfs_gets_there(CONTROL, -1, -1);
    for (x = 0; x < board_size; x++) {
        for (y = 0; y < board_size; y++) {
            int optimalness = (bfs_gets_there(CHAOS, x, y) -
                               chaos_original) -
                (bfs_gets_there(CONTROL, x, y) -
                 control_original);
            if (optimalness > best_optimalness &&
                bfs_gets_there(CHAOS, x, y) != -1 &&
                bfs_gets_there(CONTROL, x, y) != -1) {

                best_optimalness = optimalness;
                best_x = x;
                best_y = y;
            }
        }
    }
    if (best_optimalness > 0) {
        //printf("BLOCKING\n");
        blockSquare(clamp(best_x), clamp(best_y));
    } else {
        int dir = bfs_to_end(CONTROL);
        //printf("MOVING\n");
        movePosition(dir);
    }
}

int seen[MAX_SIZE][MAX_SIZE];

int bfs_to_end(int player) {
    queue<bfs_state_t> q;
    for (int x = 0; x < board_size; x++) {
        for (int y = 0; y < board_size; y++) {
            seen[x][y] = FALSE;
        }
    }
    int offset = rand()%4;
    for (int j = offset; j < 4+offset; j++) {
        int i = j%4;
        player_t start = player == CONTROL ? control : chaos;
        if (start.x+dx[i] >= 0 && start.x+dx[i] < board_size &&
            start.y+dy[i] >= 0 && start.y+dy[i] < board_size &&
            map[start.x+dx[i]][start.y+dy[i]] == CLEAR &&
            !seen[start.x+dx[i]][start.y+dy[i]]) {
            seen[start.x+dx[i]][start.y+dy[i]] = TRUE;
            q.push(bfs_state_t(start.x+dx[i], start.y+dy[i], i));
        }
    }

    while (!q.empty()) {
        bfs_state_t cur = q.front();
        q.pop();
        if ((my_id == 0 && cur.y == (player == CONTROL ? board_size-1 : 0)) ||
            (my_id == 1 && cur.y == (player == CONTROL ? 0 : board_size-1))) {
            return cur.dir;
        }
        offset = rand()%4;
        for (int j = offset; j < 4+offset; j++) {
            int i = j%4;
            if (cur.x+dx[i] >= 0 && cur.x+dx[i] < board_size &&
                cur.y+dy[i] >= 0 && cur.y+dy[i] < board_size &&
                map[cur.x+dx[i]][cur.y+dy[i]] == CLEAR &&
                !seen[cur.x+dx[i]][cur.y+dy[i]]) {
                seen[cur.x+dx[i]][cur.y+dy[i]] = TRUE;
                q.push(bfs_state_t(cur.x+dx[i], cur.y+dy[i], cur.dir));
            }
        }
    }
    fprintf(stderr, "Ben, it's happening again.\n");
    return -1;
}

int bfs_gets_there(int player, int x_block, int y_block) {
    queue<bfs_state_t> q;
    for (int x = 0; x < board_size; x++) {
        for (int y = 0; y < board_size; y++) {
            seen[x][y] = FALSE;
        }
    }
    for (int i = 0; i < 4; i++) {
        player_t start = player == CONTROL ? control : chaos;
        if (start.x == x_block && start.y == y_block) {
            return -1;
        }
        if (start.x+dx[i] >= 0 && start.x+dx[i] < board_size &&
            start.y+dy[i] >= 0 && start.y+dy[i] < board_size &&
            map[start.x+dx[i]][start.y+dy[i]] == CLEAR &&
            !(start.x+dx[i] == x_block && start.y+dy[i] == y_block) &&
            !seen[start.x+dx[i]][start.y+dy[i]]) {
            seen[start.x+dx[i]][start.y+dy[i]] = TRUE;
            q.push(bfs_state_t(start.x+dx[i], start.y+dy[i], 1));
        }
    }

    while (!q.empty()) {
        bfs_state_t cur = q.front();
        q.pop();
        if ((my_id == 0 && cur.y == (player == CONTROL ? board_size-1 : 0)) ||
            (my_id == 1 && cur.y == (player == CONTROL ? 0 : board_size-1))) {
            return cur.dir;
        }
        for (int i = 0; i < 4; i++) {
            if (cur.x+dx[i] >= 0 && cur.x+dx[i] < board_size &&
                cur.y+dy[i] >= 0 && cur.y+dy[i] < board_size &&
                map[cur.x+dx[i]][cur.y+dy[i]] == CLEAR &&
                !(cur.x+dx[i] == x_block && cur.y+dy[i] == y_block) &&
                !seen[cur.x+dx[i]][cur.y+dy[i]]) {
                seen[cur.x+dx[i]][cur.y+dy[i]] = TRUE;
                q.push(bfs_state_t(cur.x+dx[i], cur.y+dy[i], cur.dir+1));
            }
        }
    }
    return -1;
}

int clamp(int val) {
    if (val < 0) {
        val = 0;
    } else if (val >= board_size) {
        val = board_size - 1;
    }
    return val;
}
