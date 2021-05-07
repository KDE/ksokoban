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

#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "Move.h"
#include "LevelMap.h"

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

void
Move::save (QString &s) {
  static const char move1[] = "lrud";
  static const char push1[] = "LRUD";
  static const char move2[] = "wens";
  static const char push2[] = "WENS";

  assert (finished_);
  int x=startX ();
  int y=startY ();
  int pos=1;

  int x2, y2, dist=0;
  int dir=-1;
  bool push=false;
  while (pos<moveIndex_) {
    if (dir >= 0) s += push ? push1[dir] : move1[dir];

    x2 = moves_[pos]&0x7f;
    y2 = (moves_[pos]>>8)&0x7f;
    push = (moves_[pos++]&0x80)==0x80;

    if (x2<x) {
      dir = 0;
      dist = x-x2;
    } else if (x2>x) {
      dir = 1;
      dist = x2-x;
    } else if (y2<y) {
      dir = 2;
      dist = y-y2;
    } else if (y2>y) {
      dir = 3;
      dist = y2-y;
    } else {
      assert (0);
    }
    assert (dist > 0);

    if (dist > 1) {
      if (dist>=10) {
	s += '0' + (dist/10);
	dist %= 10;
      }
      s += '0' + dist;
    }

    x = x2;
    y = y2;
  }

  if (dir >= 0) s += push ? push2[dir] : move2[dir];
}

const char *
Move::load (const char *s) {
  assert (!finished_);
  int x=finalX ();
  int y=finalY ();

  int dist;
  bool last=false;
  char c;
  while ((c = *s++) != '\0') {
    dist = 1;
    if (c >= '0' && c <= '9') {
      dist = c - '0';
      c = *s++;
      if (c >= '0' && c <= '9') {
	dist = 10*dist + c - '0';
	c = *s++;
      }
    }

    switch (tolower (c)) {
    case 'w':
      last = true;
      /* Fall through */
    case 'l':
      x -= dist;
      break;
    case 'e':
      last = true;
      /* Fall through */
    case 'r':
      x += dist;
      break;
    case 'n':
      last = true;
      /* Fall through */
    case 'u':
      y -= dist;
      break;
    case 's':
      last = true;
      /* Fall through */
    case 'd':
      y += dist;
      break;

    default:
      //printf ("2><>%s\n", s);
      //abort ();
      return nullptr;
    }

    if (x<=0 || x>=MAX_X || y<=0 || y>=MAX_Y) {
      //printf ("x: %d, y:%d ><>%s\n", x, y, s);
      //abort ();

      return nullptr;
    }

    if (isupper (c)) push (x, y);
    else step (x, y);

    if (last) break;
  }
  finish ();

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
    else ret = map->step (x, y);

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
    else ret = map->unstep (x, y);

    if (!ret) return false;
  }

  return true;
}
