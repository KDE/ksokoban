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

  int level () { return collectionCurrentLevel_[collection ()]; }
  void level (int _level);
  int collection () { return collection_; }
  int completedLevels () { return collectionMaxLevel_[collection ()]; }
  int noOfLevels () { return collectionSize_[collection ()]; }
  int noOfCollections () { return noOfCollections_; }
  const char *collectionName (int i) { assert (i>=0 && i <noOfCollections_); return collectionNames_[i]; }
  void changeCollection (int _collection);
  int totalMoves () { return totalMoves_; }
  int totalPushes () { return totalPushes_; }

  bool move   (int _x, int _y);
  bool push   (int _x, int _y);
  bool unmove (int _x, int _y);
  bool unpush (int _x, int _y);

  void   printMap ();
  //void random ();

protected:
  int collection_;


private:
  int maxX_, maxY_, minX_, minY_;

  int    datasize;
  char  *data;
  char **levelIndex_;
  char **collectionNames_;
  int   *collectionIndex_;
  int   *collectionSize_;
  int   *collectionCurrentLevel_;
  int   *collectionMaxLevel_;
  int    totalLevels_;
  int    noOfCollections_;

  int    totalMoves_;
  int    totalPushes_;

  static int configCollection2Real (int collection);
  static int realCollection2Config (int collection);
  static int distance (int x1, int y1, int x2, int y2);
};

#endif  /* LEVELMAP_H */
