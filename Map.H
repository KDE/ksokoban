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

#ifndef MAP_H
#define MAP_H

#include <assert.h>

#define MAX_X 26
#define MAX_Y 19

#define WALL   1
#define GOAL   2
#define OBJECT 4
#define FLOOR  8

class Map {
  friend class MapDelta;
public:
  //static const int MAX_X=26;
  //static const int MAX_Y=19;

  bool completed () { return objectsLeft_ <= 0; }

  bool move   (int _x, int _y);
  bool push   (int _x, int _y);
  bool unmove (int _x, int _y);
  bool unpush (int _x, int _y);


  static bool badCoords (int _x, int _y) {
    return _x<0 || _y<0 || _x>MAX_X || _y>MAX_Y;
  }

  static bool badDelta (int _xd, int _yd) {
    return (_xd!=0 && _yd!=0) || (_xd==0 && _yd==0);
  }

  int xpos () { return xpos_; }
  int ypos () { return ypos_; }

  bool empty  (int x, int y) {
    assert (x>=0 && x<=MAX_X && y>=0 && y<=MAX_Y);
    return (currentMap_[y+1][x+1] & (WALL|OBJECT)) == 0;
  }
  bool wall   (int x, int y) {
    assert (x>=0 && x<=MAX_X && y>=0 && y<=MAX_Y);
    return (currentMap_[y+1][x+1] & WALL) != 0;
  }
  bool goal   (int x, int y) {
    assert (x>=0 && x<=MAX_X && y>=0 && y<=MAX_Y);
    return (currentMap_[y+1][x+1] & GOAL) != 0;
  }
  bool object (int x, int y) {
    assert (x>=0 && x<=MAX_X && y>=0 && y<=MAX_Y);
    return (currentMap_[y+1][x+1] & OBJECT) != 0;
  }
  bool floor (int x, int y) {
    assert (x>=0 && x<=MAX_X && y>=0 && y<=MAX_Y);
    return (currentMap_[y+1][x+1] & FLOOR) != 0;
  }

  bool wallUp    (int x, int y) {
    assert (x>=0 && x<=MAX_X && y>=0 && y<=MAX_Y);
    return (currentMap_[y  ][x+1] & WALL) != 0;
  }
  bool wallDown  (int x, int y) {
    assert (x>=0 && x<=MAX_X && y>=0 && y<=MAX_Y);
    return (currentMap_[y+2][x+1] & WALL) != 0;
  }
  bool wallLeft  (int x, int y) {
    assert (x>=0 && x<=MAX_X && y>=0 && y<=MAX_Y);
    return (currentMap_[y+1][x  ] & WALL) != 0;
  }
  bool wallRight (int x, int y) {
    assert (x>=0 && x<=MAX_X && y>=0 && y<=MAX_Y);
    return (currentMap_[y+1][x+2] & WALL) != 0;
  }

protected:
  //static const int wall_  =1;
  //static const int goal_  =2;
  //static const int object_=4;
  //static const int floor_ =8;

  int map (int x, int y) {
    assert (x>=0 && x<=MAX_X && y>=0 && y<=MAX_Y);
    return currentMap_[y+1][x+1];
  }
  void map (int x, int y, int val);

  void setMap (int x, int y, int bits);
  void clearMap (int x, int y, int bits);
  void clearMap ();
  void fillFloor (int x, int y);
  int objectsLeft () { return objectsLeft_; }

  int xpos_;
  int ypos_;

private:
  int    currentMap_[MAX_Y+3][MAX_X+3];
  int    objectsLeft_;
};

#endif  /* MAP_H */
