/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef QUEUE_H
#define QUEUE_H

#include <cassert>

template<class Type, int SizePow2>
class Queue
{
private:
    Type *queue_;
    long head_, tail_;

public:
    void clear()
    {
        head_ = tail_ = 0;
    }
    bool empty()
    {
        return head_ == tail_;
    }
    bool full()
    {
        return ((tail_ + 1) & ((1l << SizePow2) - 1)) == head_;
    }

    Queue()
    {
        queue_ = new Type[1l << SizePow2];
        clear();
    }
    ~Queue()
    {
        delete[] queue_;
    }

    void enqueue(Type _x)
    {
        assert(!full());
        queue_[tail_] = _x;
        tail_ = (tail_ + 1) & ((1l << SizePow2) - 1);
    }

    Type dequeue()
    {
        assert(!empty());
        Type x = queue_[head_];
        head_ = (head_ + 1) & ((1l << SizePow2) - 1);
        return x;
    }
};

#endif /* QUEUE_H */
