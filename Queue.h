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

#ifndef QUEUE_H
#define QUEUE_H

#include <assert.h>

template <class Type, int SizePow2>
class Queue {
private:
  Type *queue_;
  long   head_, tail_;

public:
  void clear() { head_ = tail_ = 0; }
  bool empty() { return head_ == tail_; }
  bool full() { return ((tail_ + 1) & ((1l<<SizePow2)-1)) == head_; }

  Queue() {
    queue_ = new Type[1l << SizePow2];
    clear();
  }
  ~Queue() { delete [] queue_; }

  void enqueue(Type _x) {
    assert(!full());
    queue_[tail_] = _x;
    tail_ = (tail_ + 1) & ((1l<<SizePow2)-1);
  }

  Type dequeue() {
    assert(!empty());
    Type x = queue_[head_];
    head_ = (head_ + 1) & ((1l<<SizePow2)-1);
    return x;
  }
};

#endif  /* QUEUE_H */
