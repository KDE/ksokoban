/*
 *  ksokoban - a Sokoban game for KDE
 *  Copyright (C) 1998,1999  Anders Widell  <awl@hem.passagen.se>
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

#ifndef ARRAY_H
#define ARRAY_H

#include <assert.h>

/**
 *  A resizable array of objects.
 *
 *  NOTES:
 *  The array's location in memory may change.
 *  Uses either T(const T &) or operator=(const T &) to copy elements.
 *  Makes sure constructors and destructors are called properly for
 *  all elements in the array.
 */

template <class T>
class Array {
public:
  Array(int size=0);
  Array(const Array &init);
  ~Array();

  void add(const T *items, int num);
  void add(const T item) { add(&item, 1); }

  T &operator [](int i) const {
    assert(i >= 0 && i < size_);
    return data_[i];
  }

  Array &operator =(const Array &init);

  int size() const { return size_; }
  void resize(int size);

private:
  T   *data_;
  int  size_;

};

#endif  /* ARRAY_H */
