/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "PathFinder.h"

#include "LevelMap.h"
#include "Move.h"
#include "Queue.h"

//#include <cstdio>

static constexpr int PATH_WALL = 32767;

void PathFinder::BFS(int _x, int _y)
{
    Queue<int, 10> xq;
    Queue<int, 10> yq;
    Queue<int, 10> dq;
    int x, y, d;

    xq.enqueue(_x);
    yq.enqueue(_y);
    dq.enqueue(1);

    while (!xq.empty()) {
        x = xq.dequeue();
        y = yq.dequeue();
        d = dq.dequeue();

        if (x < 0 || x > Map::MAX_X || y < 0 || y > Map::MAX_Y || dist[y][x])
            continue;
        dist[y][x] = d;

        xq.enqueue(x);
        xq.enqueue(x);
        xq.enqueue(x - 1);
        xq.enqueue(x + 1);

        yq.enqueue(y - 1);
        yq.enqueue(y + 1);
        yq.enqueue(y);
        yq.enqueue(y);

        dq.enqueue(d + 1);
        dq.enqueue(d + 1);
        dq.enqueue(d + 1);
        dq.enqueue(d + 1);
    }
}

Move *PathFinder::search(const Map *_map, int _x, int _y)
{
    int xpos = _map->xpos();
    int ypos = _map->ypos();
    if (xpos == _x && ypos == _y)
        return nullptr;

    for (int y = 0; y <= Map::MAX_Y; y++) {
        for (int x = 0; x <= Map::MAX_X; x++) {
            if (_map->empty(x, y))
                dist[y][x] = 0;
            else
                dist[y][x] = PATH_WALL;
        }
    }

    BFS(_x, _y);

#if 0
  for (int y=0; y<=Map::MAX_Y; y++) {
    for (int x=0; x<=Map::MAX_X; x++) {
      //if (x==_x && y==_y) {printf ("++ "); continue;}
      //if (x==xpos && y==ypos) {printf ("@@ "); continue;}
      if (dist[y][x] == PATH_WALL) {printf ("## "); continue;}
      printf ("%02d ", dist[y][x]);
    }
    printf ("\n");
  }
#endif

    int d;
    auto *move = new Move(xpos, ypos);
    int oldX, oldY;
    for (;;) {
        oldX = xpos;
        oldY = ypos;

        if (xpos == _x && ypos == _y) {
            move->finish();
            // printf ("move->finish ()\n");
            return move;
        }

        d = dist[ypos][xpos];

        while (ypos - 1 >= 0 && dist[ypos - 1][xpos] < d) {
            ypos--;
            d = dist[ypos][xpos];
        }
        if (oldY != ypos) {
            move->step(xpos, ypos);
            // printf ("step (%d, %d)\n", xpos, ypos);
            continue;
        }

        while (ypos + 1 <= Map::MAX_Y && dist[ypos + 1][xpos] < d) {
            ypos++;
            d = dist[ypos][xpos];
        }
        if (oldY != ypos) {
            move->step(xpos, ypos);
            // printf ("step (%d, %d)\n", xpos, ypos);
            continue;
        }

        while (xpos - 1 >= 0 && dist[ypos][xpos - 1] < d) {
            xpos--;
            d = dist[ypos][xpos];
        }
        if (oldX != xpos) {
            move->step(xpos, ypos);
            // printf ("step (%d, %d)\n", xpos, ypos);
            continue;
        }

        while (xpos + 1 <= Map::MAX_X && dist[ypos][xpos + 1] < d) {
            xpos++;
            d = dist[ypos][xpos];
        }
        if (oldX != xpos) {
            move->step(xpos, ypos);
            // printf ("step (%d, %d)\n", xpos, ypos);
            continue;
        }

        delete move;
        return nullptr;
    }
}

Move *PathFinder::drag(int /* x1 */, int /* y1 */, int /* x2 */, int /* y2 */)
{
    return nullptr;
}

bool PathFinder::canDrag(int /* x */, int /* y */) const
{
    return false;
}

bool PathFinder::canWalkTo(int /* x */, int /* y */) const
{
    return false;
}

bool PathFinder::canDragTo(int /* x */, int /* y */) const
{
    return false;
}

void PathFinder::updatePossibleMoves()
{
}

void PathFinder::updatePossibleDestinations(int /* x */, int /* y */)
{
}
