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

#ifndef INTERNALCOLLECTIONS_H
#define INTERNALCOLLECTIONS_H

#include <assert.h>

#include "Array.H"
#include "LevelCollection.H"

class InternalCollections {
public:
  InternalCollections();
  ~InternalCollections();

  int collections();
  LevelCollection *operator[](int n);

private:
  static int configCollection2Real(int collection);
  static int realCollection2Config(int collection);

  Array<LevelCollection *> collections_;
  char *data_;

};

#endif  /* INTERNALCOLLECTIONS_H */
