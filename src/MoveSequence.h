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

#ifndef MOVESEQUENCE_H
#define MOVESEQUENCE_H

#include <assert.h>

class Move;
class LevelMap;

class MoveSequence {
public:
  MoveSequence (Move *_move, LevelMap *_map, bool _undo=false);

  bool newStep ();
  bool next ();

private:
  LevelMap *map_;
  Move     *move_;
  int       pos_;
  int       x_, xDest_, y_, yDest_, xd_, yd_;
  bool      push_;
  bool      undo_;

};

#endif  /* MOVESEQUENCE_H */
