/*
 *  ksokoban - a Sokoban game for KDE
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <assert.h>
#include <string.h>

#include "Move.H"
#include "LevelMap.H"

Move::Move (int _startX, int _startY) {
  assert (_startX>=0 && _startX<=MAX_X && _startY>=0 && _startY<=MAX_Y);

  moves_ = new unsigned short[400];
  moves_[0] = _startX | (_startY<<8);
  moveIndex_ = 1;
  finished_ = false;

#ifndef NDEBUG
  lastX_ = _startX;
  lastY_ = _startY;
#endif
}

Move::~Move () {
  delete [] moves_;
}

void
Move::finish () {
  assert (!finished_);
  assert (moveIndex_ > 1);

  unsigned short *newMoves = new unsigned short[moveIndex_];
  memcpy (newMoves, moves_, moveIndex_*sizeof (unsigned short));
  delete [] moves_;
  moves_ = newMoves;

  finished_ = true;
}

char *
Move::save (char *s) {
  assert (finished_);
  int x=startX ();
  int y=startY ();
  int pos=1;

  int x2, y2, dist;
  char dir;
  bool push;
  while (pos<moveIndex_) {
    x2 = moves_[pos]&0x7f;
    y2 = (moves_[pos]>>8)&0x7f;
    push = (moves_[pos++]&0x80)==0x80;

    if (x2<x) {
      dir = push ? 'L' : 'l';
      dist = x-x2;
    } else if (x2>x) {
      dir = push ? 'R' : 'r';
      dist = x2-x;
    } else if (y2<y) {
      dir = push ? 'U' : 'u';
      dist = y-y2;
    } else if (y2>y) {
      dir = push ? 'D' : 'd';
      dist = y2-y;
    }

    *s++ = dir;
    if (dist>=10) {
      *s++ = '0' + (dist/10);
      dist %= 10;
    }
    *s++ = '0' + dist;
  }

  *s++ = '*';

  return s;
}

const char *
Move::load (const char *s) {
  assert (!finished_);
  int x=finalX ();
  int y=finalY ();

  int dist;
  bool p;
  char c;
  while ((c = *s++) != '*') {
    if (*s < '0' || *s > '9') return 0;
    dist = (*s++) - '0';
    if (*s >= '0' && *s <= '9') dist = 10*dist + (*s++) - '0';
    p = false;
    switch (c) {
    case 'L':
      p = true;
    case 'l':
      x -= dist;
      break;
    case 'R':
      p = true;
    case 'r':
      x += dist;
      break;
    case 'U':
      p = true;
    case 'u':
      y -= dist;
      break;
    case 'D':
      p = true;
    case 'd':
      y += dist;
      break;

    default:
      return 0;
    }

    if (x>=0 && x<=19 && y>=0 && y<=19) return 0;

    if (p) push (x, y);
    else move (x, y);
  }

  return s;
}

bool
Move::redo (LevelMap *map) {
  assert (finished_);

  for (int pos=1; pos<moveIndex_; pos++) {
    int x = moves_[pos]&0x7f;
    int y = (moves_[pos]>>8)&0x7f;
    bool push = (moves_[pos]&0x80)==0x80;
    bool ret;

    if (push) ret = map->push (x, y);
    else ret = map->move (x, y);

    if (!ret) return false;
  }

  return true;
}

bool
Move::undo (LevelMap *map) {
  assert (finished_);

  for (int pos=moveIndex_-2; pos>=0; --pos) {
    int x = moves_[pos]&0x7f;
    int y = (moves_[pos]>>8)&0x7f;
    bool push = (moves_[pos+1]&0x80)==0x80;
    bool ret;

    if (push) ret = map->unpush (x, y);
    else ret = map->unmove (x, y);

    if (!ret) return false;
  }

  return true;
}
