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

#ifndef MAPDELTA_H
#define MAPDELTA_H

#include <assert.h>

#include "Map.h"

class MapDelta : private Map {
public:
  MapDelta (Map *m);

  void start ();
  void end ();

  bool hasChanged (int x, int y) {
    assert (ended_);
    return map (x, y) == 1;
  }

private:
  Map      *source_;
  bool      ended_;
};

#endif  /* MAPDELTA_H */
