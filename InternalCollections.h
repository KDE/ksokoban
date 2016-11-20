/*
 *  ksokoban - a Sokoban game by KDE
 *  Copyright (C) 1998-2000  Anders Widell  <awl@hem.passagen.se>
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

#ifndef INTERNALCOLLECTIONS_H
#define INTERNALCOLLECTIONS_H

#include <assert.h>
#include <QString>
#include <QVector>

#include "LevelCollection.h"

class InternalCollections {
public:
  InternalCollections();
  ~InternalCollections();

  static int toInternalId(int _id) {
    if (_id < 10 || _id > 14) return 1000;
    return _id - 10;
  }

  int collections();
  LevelCollection *operator[](int n);

private:
  void add(LevelCollection* c);

  static int configCollection2Real(int collection);
  static int realCollection2Config(int collection);
  static QString collectionName(int _level);

  QVector<LevelCollection*> collections_;
  char *data_;

};

#endif  /* INTERNALCOLLECTIONS_H */
