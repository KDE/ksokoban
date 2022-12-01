/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "LevelMap.h"

#include "LevelCollection.h"

#include <KConfig>

#include <unistd.h>

#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

const QString &LevelMap::collectionName() const
{
    return collection_->name();
}

LevelMap::LevelMap() = default;

LevelMap::~LevelMap() = default;

void LevelMap::changeCollection(LevelCollection *_collection)
{
    collection_ = _collection;
    goodLevel_ = collection_->loadLevel(&map_);
    totalMoves_ = totalPushes_ = 0;
}

int LevelMap::level() const
{
    if (collection_ == nullptr)
        return 0;
    return collection_->level();
}

void LevelMap::level(int _level)
{
    assert(collection_ != nullptr);

    collection_->level(_level);
    goodLevel_ = collection_->loadLevel(&map_);

    totalMoves_ = totalPushes_ = 0;
}

int LevelMap::noOfLevels() const
{
    assert(collection_ != nullptr);
    return collection_->noOfLevels();
}

int LevelMap::completedLevels() const
{
    assert(collection_ != nullptr);
    return collection_->completedLevels();
}

int LevelMap::distance(int x1, int y1, int x2, int y2)
{
    int d;

    if (x2 > x1)
        d = x2 - x1;
    else
        d = x1 - x2;

    if (y2 > y1)
        d += y2 - y1;
    else
        d += y1 - y2;

    return d;
}

bool LevelMap::step(int _x, int _y)
{
    const int oldX = map_.xpos();
    const int oldY = map_.ypos();

    bool success = map_.step(_x, _y);

    const int d = distance(oldX, oldY, map_.xpos(), map_.ypos());
    totalMoves_ += d;

    return success;
}

bool LevelMap::push(int _x, int _y)
{
    const int oldX = map_.xpos();
    const int oldY = map_.ypos();

    bool success = map_.push(_x, _y);

    const int d = distance(oldX, oldY, map_.xpos(), map_.ypos());
    totalMoves_ += d;
    totalPushes_ += d;

    if (map_.completed())
        collection_->levelCompleted();

    return success;
}

bool LevelMap::unstep(int _x, int _y)
{
    const int oldX = map_.xpos();
    const int oldY = map_.ypos();

    bool success = map_.unstep(_x, _y);

    const int d = distance(oldX, oldY, map_.xpos(), map_.ypos());
    totalMoves_ -= d;

    return success;
}

bool LevelMap::unpush(int _x, int _y)
{
    const int oldX = map_.xpos();
    const int oldY = map_.ypos();

    bool success = map_.unpush(_x, _y);

    const int d = distance(oldX, oldY, map_.xpos(), map_.ypos());
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
