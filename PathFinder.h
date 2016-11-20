/*
 *  ksokoban - a Sokoban game by KDE
 *  Copyright (C) 1998  Anders Widell  <d95-awi@nada.kth.se>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "Map.h"
class Move;

#define PATH_WALL 32767

class PathFinder {
public:
  Move *search (Map *_map, int _x, int _y);
  Move* drag(int x1, int y1, int x2, int y2);
  bool canDrag(int x, int y) const;
  bool canWalkTo(int x, int y) const;
  bool canDragTo(int x, int y) const;
  void updatePossibleMoves();
  void updatePossibleDestinations(int x, int y);

protected:
  //static const int PATH_WALL=32767;

  int dist[MAX_Y+1][MAX_X+1];

  void BFS (int _x, int _y);


};

#endif  /* PATHFINDER_H */
