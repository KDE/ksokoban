/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MOVESEQUENCE_H
#define MOVESEQUENCE_H

#include <cassert>

class Move;
class LevelMap;

class MoveSequence
{
public:
    MoveSequence(Move *_move, LevelMap *_map, bool _undo = false);

public:
    bool newStep();
    bool next();

private:
    LevelMap *map_;
    Move *move_;
    int pos_;
    int x_;
    int xDest_;
    int y_;
    int yDest_;
    int xd_;
    int yd_;
    bool push_;
    bool undo_;
};

#endif /* MOVESEQUENCE_H */
