/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "Map.h"

class Move;

class PathFinder
{
public:
    Move *search(Map *_map, int _x, int _y);
    Move *drag(int x1, int y1, int x2, int y2);
    bool canDrag(int x, int y) const;
    bool canWalkTo(int x, int y) const;
    bool canDragTo(int x, int y) const;
    void updatePossibleMoves();
    void updatePossibleDestinations(int x, int y);

protected:
    int dist[MAX_Y + 1][MAX_X + 1];

    void BFS(int _x, int _y);
};

#endif /* PATHFINDER_H */
