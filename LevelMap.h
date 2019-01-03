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

#ifndef LEVELMAP_H
#define LEVELMAP_H

#include <assert.h>
#include <QString>

#include "Map.h"
class LevelCollection;

//#define EXTERNAL_LEVEL 100

class LevelMap : public Map {
public:
  LevelMap();
  ~LevelMap();

  LevelCollection *collection() const { return collection_; }
  const QString &collectionName();
  void changeCollection(LevelCollection *_collection);
  int totalMoves() const { return totalMoves_; }
  int totalPushes() const { return totalPushes_; }
  void level(int _level);
  int level() const;
  int noOfLevels() const;
  int completedLevels() const;
  bool goodLevel() const { return goodLevel_; }

  bool step(int _x, int _y);
  bool push(int _x, int _y);
  bool unstep(int _x, int _y);
  bool unpush(int _x, int _y);

  //void random();

protected:
  LevelCollection *collection_;


private:
  int    totalMoves_;
  int    totalPushes_;
  bool   goodLevel_;

  static int distance(int x1, int y1, int x2, int y2);
};

#endif  /* LEVELMAP_H */
