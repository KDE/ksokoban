/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MOVE_H
#define MOVE_H

#include "Map.h"

#include <QString>

#include <cassert>

class LevelMap;

/**
 * Holds information about a move
 *
 * The move can consist of several atomic steps and pushes. An atomic
 * step/push is a step/push along a straight line. The reason why these are
 * grouped together in a Move object is that they belong to the same logical
 * move in the player's point of view. An undo/redo will undo/redo all the
 * atomic moves in one step.
 *
 * @short   Maintains game movement move
 * @author  Anders Widell <d95-awi@nada.kth.se>
 * @version 0.1
 * @see     History
 */
class Move
{
    friend class MoveSequence;

public:
    Move(int _startX, int _startY);
    ~Move();

public:
    /**
     * Add an atomic move.
     * NOTE: either (x != (previous x)) or (y != (previous y))
     * must be true (but not both).
     *
     * @see LevelMap#move
     *
     * @param x  x position of destination
     * @param y  y position of destination
     */
    void step(int _x, int _y)
    {
#ifndef NDEBUG
        assert(!finished_);
        assert(_x >= 0 && _x <= Map::MAX_X && _y >= 0 && _y <= Map::MAX_Y);
        assert(moveIndex_ < 400);
        assert((_x != lastX_ && _y == lastY_) || (_x == lastX_ && _y != lastY_));
        lastX_ = _x;
        lastY_ = _y;
#endif

        moves_[moveIndex_++] = _x | (_y << 8);
    }

    /**
     * Same as move above, but used when an object is pushed.
     *
     * @see LevelMap#push
     */
    void push(int _x, int _y)
    {
#ifndef NDEBUG
        assert(!finished_);
        assert(_x >= 0 && _x <= Map::MAX_X && _y >= 0 && _y <= Map::MAX_Y);
        assert(moveIndex_ < 400);
        assert((_x != lastX_ && _y == lastY_) || (_x == lastX_ && _y != lastY_));
        lastX_ = _x;
        lastY_ = _y;
#endif

        moves_[moveIndex_++] = _x | (_y << 8) | 0x80;
    }

    void finish();

    int startX() const
    {
        return moves_[0] & 0x7f;
    }
    int startY() const
    {
        return (moves_[0] >> 8) & 0x7f;
    }
    int finalX() const
    {
        return moves_[moveIndex_ - 1] & 0x7f;
    }
    int finalY() const
    {
        return (moves_[moveIndex_ - 1] >> 8) & 0x7f;
    }

    void save(QString &_str) const;
    const char *load(const char *_str);
    bool redo(LevelMap *map);
    bool undo(LevelMap *map);

private:
    unsigned short *moves_;
    int moveIndex_ = 1;
    bool finished_ = false;

#ifndef NDEBUG
    int lastX_, lastY_;
#endif
};

#endif /* MOVE_H */
