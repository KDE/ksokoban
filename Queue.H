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

#ifndef QUEUE_H
#define QUEUE_H

#include <assert.h>

template <class Type>
class Queue {
private:
  Type *queue_;
  int   size_, head_, tail_;

public:
  void clear () { head_ = tail_ = 0; }
  bool empty () { return head_ == tail_; }
  bool full () { return (head_-tail_+size_)%size_ == 1; }

  Queue (int _capacity) {
    queue_ = new Type[size_ = _capacity+1];
    clear ();
  }
  ~Queue () { delete [] queue_; }

  void enqueue (Type _x) {
    assert (!full ());
    queue_[tail_++] = _x;
    if (tail_ >= size_) tail_ = 0;
  }

  Type dequeue () {
    assert (!empty ());
    Type x = queue_[head_++];
    if (head_ >= size_) head_ = 0;
    return x;
  }
};

#endif  /* QUEUE_H */
