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

#include <string.h>

#include "MapDelta.h"


MapDelta::MapDelta (Map *m) {
  source_ = m;
  ended_ = true;
  start ();
}

void
MapDelta::start () {
  assert (ended_);
  ((Map &) *this) = *source_;

#if 0
  memcpy (map_, source_->currentMap_, (MAX_Y+3)*(MAX_X+3)*sizeof (int));
  for (int y=1; y<MAX_Y+2; y++) {
    for (int x=1; x<MAX_X+2; x++) {
      map_[y][x] = source_->currentMap_[y][x];
    }
  }
  xpos_ = source_->xpos_;
  ypos_ = source_->ypos_;
#endif

  ended_ = false;
}

void
MapDelta::end () {
  assert (!ended_);
  for (int y=0; y<=MAX_Y; y++) {
    for (int x=0; x<=MAX_X; x++) {
      map (x, y, map (x, y) != source_->map (x, y));
    }
  }
  if (xpos_ != source_->xpos_ || ypos_ != source_->ypos_) {
    map (xpos_, ypos_, 1);
    map (source_->xpos_, source_->ypos_, 1);
  }
  ended_ = true;
}
