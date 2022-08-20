/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <unistd.h>

#include <cstdlib>
#include <cctype>
#include <cassert>
#include <cstdio>
#include <cstring>

#include <KConfig>

#include <zlib.h>

#include "LevelMap.h"
#include "LevelCollection.h"

#define BUFSIZE (128*1024)

const QString &
LevelMap::collectionName() {
  return collection_->name();
}

LevelMap::LevelMap () : collection_(nullptr), totalMoves_(0), totalPushes_(0),
			goodLevel_(false) {
}

LevelMap::~LevelMap () {
}

void
LevelMap::changeCollection (LevelCollection *_collection)
{
  collection_ = _collection;
  goodLevel_ = collection_->loadLevel(this);
  totalMoves_ = totalPushes_ = 0;
}

int
LevelMap::level () const {
  if (collection_ == nullptr) return 0;
  return collection_->level();
}

void
LevelMap::level (int _level) {
  assert(collection_ != nullptr);

  collection_->level(_level);
  goodLevel_ = collection_->loadLevel(this);

  totalMoves_ = totalPushes_ = 0;
}

int
LevelMap::noOfLevels () const {
  assert(collection_ != nullptr);
  return collection_->noOfLevels();
}

int
LevelMap::completedLevels () const{
  assert(collection_ != nullptr);
  return collection_->completedLevels();
}

int
LevelMap::distance (int x1, int y1, int x2, int y2) {
  int d;

  if (x2 > x1) d = x2-x1;
  else d = x1-x2;

  if (y2 > y1) d += y2-y1;
  else d += y1-y2;

  return d;
}

bool
LevelMap::step (int _x, int _y) {
  int oldX=xpos_, oldY=ypos_;

  bool success = Map::step (_x, _y);

  totalMoves_ += distance (oldX, oldY, xpos_, ypos_);

  return success;
}

bool
LevelMap::push (int _x, int _y) {
  int oldX=xpos_, oldY=ypos_;

  bool success = Map::push (_x, _y);

  int d = distance (oldX, oldY, xpos_, ypos_);
  totalMoves_ += d;
  totalPushes_ += d;

  if (completed ()) collection_->levelCompleted();

  return success;
}

bool
LevelMap::unstep (int _x, int _y) {
  int oldX=xpos_, oldY=ypos_;

  bool success = Map::unstep (_x, _y);

  totalMoves_ -= distance (oldX, oldY, xpos_, ypos_);

  return success;
}

bool
LevelMap::unpush (int _x, int _y) {
  int oldX=xpos_, oldY=ypos_;

  bool success = Map::unpush (_x, _y);

  int d = distance (oldX, oldY, xpos_, ypos_);
  totalMoves_ -= d;
  totalPushes_ -= d;

  return success;
}

#if 0
void
LevelMap::random (void) {
  printf ("start!\n");

  minX_ = 0;
  minY_ = 0;
  maxX_ = MAX_X;
  maxY_ = MAX_Y;
  totalMoves_ = totalPushes_ = 0;
  clearMap ();

  xpos_ = 13;
  ypos_ = 9;

  KRandomSequence random(0);

  for (int i=0; i<200; i++) {
    map (xpos_, ypos_, FLOOR);

    switch (random.getLong(4)) {
    case 0:
      if (ypos_ > 1) ypos_--; else i--;
      break;

    case 1:
      if (ypos_ < MAX_Y-1) ypos_++; else i--;
      break;

    case 2:
      if (xpos_ > 1) xpos_--; else i--;
      break;

    case 3:
      if (xpos_ < MAX_X-1) xpos_++; else i--;
      break;
    }
  }

  for (int y=1; y<MAX_Y; y++) {
    for (int x=1; x<MAX_X; x++) {
      if (map (x, y) & FLOOR) {
	if (!(map (x, y-1) & FLOOR)) map (x, y-1, WALL);
	if (!(map (x, y+1) & FLOOR)) map (x, y+1, WALL);
	if (!(map (x-1, y) & FLOOR)) map (x-1, y, WALL);
	if (!(map (x+1, y) & FLOOR)) map (x+1, y, WALL);
      }
    }
  }

  printf ("klar!\n");
  printMap ();
}
#endif
