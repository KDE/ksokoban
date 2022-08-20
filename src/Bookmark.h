/*
 *  ksokoban - a Sokoban game by KDE
 *  Copyright (C) 1998  Anders Widell  <awl@hem.passagen.se>
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

#ifndef BOOKMARK_H
#define BOOKMARK_H

class History;
class LevelMap;

#include <QString>

class Bookmark {
public:
  Bookmark(int _num);

  int collection() const { return collection_; }
  int level() const { return level_; }
  int moves() const { return moves_; }
  //int pushes() { return pushes_; }

  void set(int _collection, int _level, int _moves, History *_h);
  bool goTo(LevelMap *_map, History *_h);

private:
  void fileName(QString &p);

  int     number_;
  int     collection_;
  int     level_;
  int     moves_;
  //int     pushes_;
  QString data_;
};

#endif  /* BOOKMARK_H */
