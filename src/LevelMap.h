/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef LEVELMAP_H
#define LEVELMAP_H

#include "Map.h"

#include <QString>

#include <cassert>

class LevelCollection;

//#define EXTERNAL_LEVEL 100

class LevelMap : public Map
{
public:
    LevelMap();
    ~LevelMap();

    LevelCollection *collection() const
    {
        return collection_;
    }
    const QString &collectionName();
    void changeCollection(LevelCollection *_collection);
    int totalMoves() const
    {
        return totalMoves_;
    }
    int totalPushes() const
    {
        return totalPushes_;
    }
    void level(int _level);
    int level() const;
    int noOfLevels() const;
    int completedLevels() const;
    bool goodLevel() const
    {
        return goodLevel_;
    }

    bool step(int _x, int _y);
    bool push(int _x, int _y);
    bool unstep(int _x, int _y);
    bool unpush(int _x, int _y);

    // void random();

protected:
    LevelCollection *collection_;

private:
    static int distance(int x1, int y1, int x2, int y2);

private:
    int totalMoves_;
    int totalPushes_;
    bool goodLevel_;
};

#endif /* LEVELMAP_H */
