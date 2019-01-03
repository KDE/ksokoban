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

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "Map.h"

Map::Map() : xpos_(-1), ypos_(-1), width_(0), height_(0), objectsLeft_(-1) {
}


void
Map::map (int x, int y, int val) {
  assert (x>=0 && x<=MAX_X && y>=0 && y<=MAX_Y);
  if ((map (x, y) & (OBJECT | GOAL)) == OBJECT) objectsLeft_--;
  if ((val & (OBJECT | GOAL)) == OBJECT) objectsLeft_++;
  currentMap_[y+1][x+1] = val;

  if (val != 0) {
    if (width_ <= x) width_ = x+1;
    if (height_ <= y) height_ = y+1;
  }
}

void
Map::setMap (int x, int y, int bits) {
  assert ((map (x, y) & bits) == 0);
  if (goal (x, y) && ((bits & OBJECT) == OBJECT)) objectsLeft_--;
  assert (objectsLeft_ >= 0);
  currentMap_[y+1][x+1] |= bits;

  if (bits != 0) {
    if (width_ <= x) width_ = x+1;
    if (height_ <= y) height_ = y+1;
  }
}

void
Map::clearMap (int x, int y, int bits) {
  assert ((map (x, y) & bits) == bits);
  if (goal (x, y) && ((bits & OBJECT) == OBJECT)) objectsLeft_++;
  currentMap_[y+1][x+1] &= ~bits;
}

void
Map::clearMap () {
  memset (currentMap_, 0, (MAX_Y+3)*(MAX_X+3)*sizeof (char));
  objectsLeft_ = 0;
  width_ = height_ = 0;
}

bool
Map::fillFloor (int x, int y) {
  if (badCoords (x, y)) return false;
  if ((currentMap_[y+1][x+1] & (WALL|FLOOR)) != 0) return true;

  currentMap_[y+1][x+1] |= FLOOR;
  bool a = fillFloor (x, y-1);
  bool b = fillFloor (x, y+1);
  bool c = fillFloor (x-1, y);
  bool d = fillFloor (x+1, y);

  return a && b && c && d;
}

bool
Map::step (int _x, int _y) {
  assert (!badCoords (xpos_, ypos_));
  assert (empty (xpos_, ypos_));

  int xd=0, yd=0;
  if (_x < xpos_) xd = -1;
  if (_x > xpos_) xd =  1;
  if (_y < ypos_) yd = -1;
  if (_y > ypos_) yd =  1;
  if (badDelta (xd, yd) || badCoords (_x, _y)) return false;

  int x=xpos_, y=ypos_;
  do {
    x += xd;
    y += yd;
    if (!empty (x, y)) return false;
  } while (!(x==_x && y==_y));

  xpos_ = _x;
  ypos_ = _y;

  return true;
}

bool
Map::push (int _x, int _y) {
  assert (!badCoords (xpos_, ypos_));
  assert (empty (xpos_, ypos_));

  int xd=0, yd=0;
  if (_x < xpos_) xd = -1;
  if (_x > xpos_) xd =  1;
  if (_y < ypos_) yd = -1;
  if (_y > ypos_) yd =  1;
  if (badDelta (xd, yd) || badCoords (_x+xd, _y+yd)) return false;

  int x=xpos_+xd, y=ypos_+yd;
  if (!object (x, y)) return false;
  if (!empty (_x+xd, _y+yd)) return false;

  while (!(x==_x && y==_y)) {
    x += xd;
    y += yd;
    if (!empty (x, y)) return false;
  }

  clearMap (xpos_+xd, ypos_+yd, OBJECT);
  setMap (_x+xd, _y+yd, OBJECT);

  xpos_ = _x;
  ypos_ = _y;

  return true;
}

bool
Map::unstep (int _x, int _y) {
  return Map::step (_x, _y);
}

bool
Map::unpush (int _x, int _y) {
  assert (!badCoords (xpos_, ypos_));
  assert (empty (xpos_, ypos_));

  int xd=0, yd=0;
  if (_x < xpos_) xd = -1;
  if (_x > xpos_) xd =  1;
  if (_y < ypos_) yd = -1;
  if (_y > ypos_) yd =  1;
  if (badDelta (xd, yd) || badCoords (_x+xd, _y+yd)) return false;

  int x=xpos_, y=ypos_;
  if (!object (x-xd, y-yd)) return false;

  do {
    x += xd;
    y += yd;
    if (!empty (x, y)) return false;
  } while (!(x==_x && y==_y));

  clearMap (xpos_-xd, ypos_-yd, OBJECT);
  setMap (_x-xd, _y-yd, OBJECT);

  xpos_ = _x;
  ypos_ = _y;

  return true;
}

void
Map::printMap(void) {
  for (int y=0; y<height_; y++) {
    for (int x=0; x<width_; x++) {
      switch (map (x, y) & ~FLOOR) {
      case WALL:
	printf("#");
	break;
      case GOAL:
	printf("%c", x==xpos_ && y==ypos_ ? '+' : '.');
	break;
      case OBJECT:
	printf("$");
	break;
      case OBJECT|GOAL:
	printf("*");
	break;
      case 0:
	printf("%c", x==xpos_ && y==ypos_ ? '@' : ' ');
	break;
      default:
	printf("<%X>", map(x,y)&FLOOR);
	break;
      }
    }
    printf ("\n");
  }
}

