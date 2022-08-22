/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MAP_H
#define MAP_H

#include <cassert>

#define MAX_X 49
#define MAX_Y 49

#define WALL 1
#define GOAL 2
#define OBJECT 4
#define FLOOR 8

class Map
{
    friend class MapDelta;
    friend class LevelCollection;

public:
    Map();

public:
    bool completed() const
    {
        return objectsLeft_ <= 0;
    }

    bool step(int _x, int _y);
    bool push(int _x, int _y);
    bool unstep(int _x, int _y);
    bool unpush(int _x, int _y);

    static bool badCoords(int _x, int _y)
    {
        return _x < 0 || _y < 0 || _x > MAX_X || _y > MAX_Y;
    }

    static bool badDelta(int _xd, int _yd)
    {
        return (_xd != 0 && _yd != 0) || (_xd == 0 && _yd == 0);
    }

    int xpos() const
    {
        return xpos_;
    }
    int ypos() const
    {
        return ypos_;
    }

    bool empty(int x, int y)
    {
        assert(x >= 0 && x <= MAX_X && y >= 0 && y <= MAX_Y);
        return (currentMap_[y + 1][x + 1] & (WALL | OBJECT)) == 0;
    }
    bool wall(int x, int y)
    {
        assert(x >= 0 && x <= MAX_X && y >= 0 && y <= MAX_Y);
        return (currentMap_[y + 1][x + 1] & WALL) != 0;
    }
    bool goal(int x, int y)
    {
        assert(x >= 0 && x <= MAX_X && y >= 0 && y <= MAX_Y);
        return (currentMap_[y + 1][x + 1] & GOAL) != 0;
    }
    bool object(int x, int y)
    {
        assert(x >= 0 && x <= MAX_X && y >= 0 && y <= MAX_Y);
        return (currentMap_[y + 1][x + 1] & OBJECT) != 0;
    }
    bool floor(int x, int y)
    {
        assert(x >= 0 && x <= MAX_X && y >= 0 && y <= MAX_Y);
        return (currentMap_[y + 1][x + 1] & FLOOR) != 0;
    }

    bool wallUp(int x, int y)
    {
        assert(x >= 0 && x <= MAX_X && y >= 0 && y <= MAX_Y);
        return (currentMap_[y][x + 1] & WALL) != 0;
    }
    bool wallDown(int x, int y)
    {
        assert(x >= 0 && x <= MAX_X && y >= 0 && y <= MAX_Y);
        return (currentMap_[y + 2][x + 1] & WALL) != 0;
    }
    bool wallLeft(int x, int y)
    {
        assert(x >= 0 && x <= MAX_X && y >= 0 && y <= MAX_Y);
        return (currentMap_[y + 1][x] & WALL) != 0;
    }
    bool wallRight(int x, int y)
    {
        assert(x >= 0 && x <= MAX_X && y >= 0 && y <= MAX_Y);
        return (currentMap_[y + 1][x + 2] & WALL) != 0;
    }

    void printMap();

    int width() const
    {
        return width_;
    }
    int height() const
    {
        return height_;
    }

protected:
    int map(int x, int y) const
    {
        assert(x >= 0 && x <= MAX_X && y >= 0 && y <= MAX_Y);
        return currentMap_[y + 1][x + 1];
    }
    void map(int x, int y, int val);

    void setMap(int x, int y, int bits);
    void clearMap(int x, int y, int bits);
    void clearMap();
    bool fillFloor(int x, int y);
    int objectsLeft() const
    {
        return objectsLeft_;
    }

protected:
    int xpos_;
    int ypos_;

private:
    char currentMap_[MAX_Y + 3][MAX_X + 3];
    int width_;
    int height_;
    int objectsLeft_;
};

#endif /* MAP_H */
