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

#include "Array.H"

#include <new.h>
#include <stdlib.h>


template <class T>
Array<T>::Array(int size) : data_(0), size_(0) {
  assert(size >= 0);

  resize(size);
}

template <class T>
Array<T>::Array(const Array &init) : data_(0), size_(0) {
  add(init.data_, init.size_);
}


template <class T>
Array<T>::~Array() {
  resize(0);
}


template <class T>
void
Array<T>::add(const T *items, int num) {
  assert(num >= 0);
  assert(items != 0);

  if (num <= 0) return;

  data_ = (T *) realloc(data_, (size_+num) * sizeof (T));
  if (data_ == 0) abort();

  for (int i=0; i<num; i++) {
    new (data_ + size_ + i) T(items[i]);
  }

  size_ += num;
}


template <class T>
void
Array<T>::resize(int size) {
  assert(size >= 0);

  for (int i=size; i<size_; i++) {
    data_[i].~T();
  }

  if (size == 0) {
    free(data_);
    data_ = 0;
    size_ = 0;
  } else {
    data_ = (T *) realloc(data_, size * sizeof (T));
    if (data_ == 0) abort();

    if (size > size_) {
      for (int i=size_; i<size; i++) {
	new (data_ + i) T;
      }
    }
  }

  size_ = size;
}

template <class T>
Array<T> &
Array<T>::operator =(const Array &init) {
  if (&init != this) {
    resize(init.size_);

    for (int i=0; i<size_; i++) {
      data_[i] = init.data_[i];
    }
  }

  return *this;
}


template Array<char>;
template Array<char *>;

#include "LevelCollection.H"
template Array<LevelCollection *>;
