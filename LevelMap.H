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

#ifndef LEVELMAP_H
#define LEVELMAP_H

#include <assert.h>

#include "Map.H"

class LevelMap : public Map {
public:
  LevelMap ();
  ~LevelMap ();

  int maxX () { return maxX_; }
  int maxY () { return maxY_; }
  int minX () { return minX_; }
  int minY () { return minY_; }

  int level () { return setCurrentLevel_[set_]; }
  void level (int _level);
  int set () { return set_; }
  int completedLevels () { return setMaxLevel_[set_]; }
  int noOfLevels () { return setSize_[set_]; }
  int noOfSets () { return noOfSets_; }
  const char *setName (int i) { assert (i>=0 && i <noOfSets_); return setNames_[i]; }
  void changeSet (int _set);

  bool move   (int _x, int _y);
  bool push   (int _x, int _y);
  bool unmove (int _x, int _y);
  bool unpush (int _x, int _y);

  void   printMap ();

protected:
  int set_;


private:
  int maxX_, maxY_, minX_, minY_;

  int    datasize;
  char  *data;
  char **levelIndex_;
  char **setNames_;
  int   *setIndex_;
  int   *setSize_;
  int   *setCurrentLevel_;
  int   *setMaxLevel_;
  int    totalLevels_;
  int    noOfSets_;
};

#endif  /* LEVELMAP_H */
