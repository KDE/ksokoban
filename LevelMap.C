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

#include <kconfig.h>
#include <kapp.h>

#include <unistd.h>
#include <zlib.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>

#include "LevelMap.H"

#include "levels/data.c"

#define BUFSIZE (128*1024)

static inline unsigned long
forward(unsigned long a, unsigned long b, unsigned long c, unsigned long d)
{
  unsigned long x=(a^b)&0xfffffffful;
  return (((x<<c)|(x>>((32ul-c)&31ul)))*d)&0xfffffffful;
}

static inline unsigned long
backward(unsigned long a, unsigned long b, unsigned long c, unsigned long d)
{
  unsigned long x=(a*b)&0xfffffffful;
  return (((x<<c)|(x>>((32ul-c)&31ul)))^d)&0xfffffffful;
}

static const int collection_save_id[] = {
  0, 1, 3, 5, 9, 6, 7, 8, 2, 4
};

LevelMap::LevelMap () {
  KConfig *cfg=(KApplication::getKApplication ())->getConfig ();

  data = (char *) malloc (BUFSIZE);
  if (data == NULL) abort ();

  datasize = BUFSIZE;
  uncompress ((unsigned char *) data, (long unsigned int *) &datasize, level_data, sizeof (level_data));
  data = (char *) realloc (data, datasize);
  if (data == NULL) abort ();

  totalLevels_ = noOfSets_ = 0;
  bool newLevel=true;
  for (int pos=0; pos<datasize; pos++) {
    while (newLevel && pos < datasize-1 && data[pos] == '\n') pos++;
    if (newLevel && isalpha (data[pos])) {
      noOfSets_++;
    }
    //if (data[pos] == '\n' && data[pos-1] == '\n') data[pos] = '\0';
    if (data[pos] == '\0') {
      totalLevels_++;
      newLevel = true;
    } else {
      newLevel = false;
    }
  }
  //printf ("totalLevels_ = %d noOfSets_ = %d\n", totalLevels_, noOfSets_);

  levelIndex_ = new char *[totalLevels_];
  setNames_   = new char *[noOfSets_];
  setIndex_   = new int[noOfSets_];
  setSize_    = new int[noOfSets_];
  setCurrentLevel_    = new int[noOfSets_];
  setMaxLevel_    = new int[noOfSets_];

  char *pos=data;
  int set=0, setSize=0;
  for (int lev=0; lev<totalLevels_; lev++) {
    while (*pos == '\n') pos++;
    if (isalpha (*pos)) {
      setNames_[set] = pos;
      setIndex_[set] = lev;
      if (set) {
	setSize_[set-1] = setSize;
	//printf ("setSize[%d]=%d\n", set-1, setSize);
      }
      setSize = 0;
      set++;
      while (*pos != '\n') pos++;
      *pos++ = '\0';
    }
    levelIndex_[lev] = pos;
    while (*pos != '\0') pos++;
    pos++;
    setSize++;
  }
  if (set) {
    setSize_[set-1] = setSize;
    //printf ("setSize[%d]=%d\n", set-1, setSize);
  }
  
  for (int i=0; i<noOfSets_; i++) {
    char buf[256];
    unsigned long x;

    sprintf (buf, "level%d", collection_save_id[i]);
    setCurrentLevel_[i] = cfg->readNumEntry (buf, 0);

    sprintf (buf, "status%d", collection_save_id[i]);

    x = cfg->readUnsignedLongNumEntry (buf, 0);

    x = backward (x, 0xc1136a15ul, 0x12ul, 0x80ff0b94ul);
    x = backward (x, 0xd38fd2ddul, 0x01ul, 0xd4d657b4ul);
    x = backward (x, 0x59004eeful, 0x1eul, 0xf6c75e2cul);
    x = backward (x, 0x366c3e25ul, 0x0aul, 0x61ebc208ul);
    x = backward (x, 0x20a784c9ul, 0x15ul, 0x207d488bul);
    x = backward (x, 0xc02864abul, 0x09ul, 0x709e62a3ul);
    x = backward (x, 0xe2a60f19ul, 0x0eul, 0x8bb02c07ul);
    x = backward (x, 0x3b0e11f3ul, 0x13ul, 0x608aef3ful);

    //printf ("%d: %d %d %d\n", i, (x>>26) & 0x3ful, x>>16 & 0x3ff, x & 0xfffful);

    setMaxLevel_[i] = x>>16 & 0x3ff;
    //printf (":::%d\n", setMaxLevel_[3]);
    if (((x>>26) & 0x3ful) != (unsigned long) i) setMaxLevel_[i] = 0;
    if ((x & 0xfffful) != (unsigned long) getuid ()) setMaxLevel_[i] = 0;
    if (setMaxLevel_[i] >= setSize_[i]) setMaxLevel_[i] = 0;
    if (setCurrentLevel_[i] > setMaxLevel_[i]) setCurrentLevel_[i] = setMaxLevel_[i];
    if (!cfg->hasKey (buf)) setMaxLevel_[i] = 0;
  }

  changeSet (cfg->readNumEntry ("collection", 0));
}

LevelMap::~LevelMap () {
  KConfig *cfg=(KApplication::getKApplication ())->getConfig ();
  char buf[256];

  for (int i=0; i<noOfSets_; i++) {
    sprintf (buf, "level%d", collection_save_id[i]);
    cfg->writeEntry (buf, setCurrentLevel_[i], true, false, false);
  }
  cfg->writeEntry ("collection", set ());
  

  delete [] setCurrentLevel_;
  delete [] setMaxLevel_;
  delete [] setSize_;
  delete [] setIndex_;
  delete [] setNames_;
  delete [] levelIndex_;
  free (data);
}

void
LevelMap::printMap (void) {
  for (int y=minY_; y<=maxY_; y++) {
    for (int x=minX_; x<=maxX_; x++) {
      switch (map (x, y) & ~FLOOR) {
      case WALL:
	printf ("#");
	break;
      case GOAL:
	printf ("%c", x==xpos_ && y==ypos_ ? '+' : '.');
	break;
      case OBJECT:
	printf ("$");
	break;
      case OBJECT|GOAL:
	printf ("*");
	break;
      case 0:
	printf ("%c", x==xpos_ && y==ypos_ ? '@' : ' ');
	break;
      default:
	printf ("<%X>", map(x,y)&FLOOR);
	  break;
      }
    }
    printf ("\n");
  }
}




void
LevelMap::changeSet (int _set)
{
  if (_set < 0 || _set >= noOfSets_) _set = 0;
  set_ = _set;
  level (level ());
}

void
LevelMap::level (int _level) {
  int goalsLeft=0;
  minX_ = MAX_X;
  minY_ = MAX_Y;
  maxX_ = maxY_ = 0;

  //printf ("set_: %d, setMaxLevel_: %d\n", set_, setMaxLevel_[set_]);
  //printf ("level(%d): completedLevels=%d, noOfLevels=%d\n", _level, completedLevels (), noOfLevels ());

  if (_level >= completedLevels ()) _level = completedLevels ();
  if (_level >= noOfLevels ()) _level = noOfLevels ()-1;
  if (_level < 0) _level = 0;
  setCurrentLevel_[set ()] = _level;

  //printf ("level: %d\n", _level);

  clearMap ();

  int x=0, y=0;
  for (char *pos=levelIndex_[_level+setIndex_[set ()]]; *pos; pos++) {
    switch (*pos) {
    case '\n':
    case '\t':
    case ' ':
      break;
    default:
      if (badCoords (x, y)) {
	fprintf (stderr, "bad character coordinates: %d,%d\n", x, y);
	abort ();
      }
      if (x < minX_) minX_ = x;
      if (y < minY_) minY_ = y;
      if (x > maxX_) maxX_ = x;
      if (y > maxY_) maxY_ = y;
      break;
    }

    switch (*pos) {
    case '\n':
      y++;
      x = 0;
      break;

    case ' ':
      x++;
      break;

    case '\t':
      x = (x+8) & ~7;
      break;

    case '@':
      xpos_ = x;
      ypos_ = y;
      x++;
      break;

    case '$':
      map (x, y, OBJECT);
      x++;
      break;

    case '.':
      map (x, y, GOAL);
      goalsLeft++;
      x++;
      break;

    case '#':
      map (x, y, WALL);
      x++;
      break;

    case '+':
      xpos_ = x;
      ypos_ = y;
      map (x, y, GOAL);
      goalsLeft++;
      x++;
      break;

    case '*':
      map (x, y, OBJECT|GOAL);
      x++;
      break;

    default:
      fprintf (stderr, "Illegal level char: %c\n", *pos);
      abort ();
      break;
    }
  }

  assert (objectsLeft () == goalsLeft);
  assert (!badCoords (xpos_, ypos_));

  assert (empty (xpos_, ypos_));
  assert (!completed ());
  //printf ("objects: %d\n", objectsLeft_);

  fillFloor (xpos_, ypos_);

  //printf ("maxX: %d, minX: %d\n", maxX_, minX_);
  //printf ("maxY: %d, minY: %d\n", maxY_, minY_);

  //emit levelChange ();

#if 0
  printMap ();
  move (11, 11);
  printMap ();
  move (6, 11);
  printMap ();
  push (4, 11);
  printMap ();
  unpush (17, 11);
  printMap ();
#endif
}

bool
LevelMap::move (int _x, int _y) {
  assert (!badCoords (xpos_, ypos_));
  assert (empty (xpos_, ypos_));

  int xd=0, yd=0;
  if (_x < xpos_) xd = -1;
  if (_x > xpos_) xd =  1;
  if (_y < ypos_) yd = -1;
  if (_y > ypos_) yd =  1;
  if (badDelta (xd, yd) || badCoords (_x, _y)) return false;

  int x=xpos_, y=ypos_;
  do {
    x += xd;
    y += yd;
    if (!empty (x, y)) return false;
  } while (!(x==_x && y==_y));

  xpos_ = _x;
  ypos_ = _y;

  return true;
}

bool
LevelMap::push (int _x, int _y) {
  assert (!badCoords (xpos_, ypos_));
  assert (empty (xpos_, ypos_));

  int xd=0, yd=0;
  if (_x < xpos_) xd = -1;
  if (_x > xpos_) xd =  1;
  if (_y < ypos_) yd = -1;
  if (_y > ypos_) yd =  1;
  if (badDelta (xd, yd) || badCoords (_x+xd, _y+yd)) return false;

  int x=xpos_+xd, y=ypos_+yd;
  if (!object (x, y)) return false;
  if (!empty (_x+xd, _y+yd)) return false;

  while (!(x==_x && y==_y)) {
    x += xd;
    y += yd;
    if (!empty (x, y)) return false;
  }

  clearMap (xpos_+xd, ypos_+yd, OBJECT);
  setMap (_x+xd, _y+yd, OBJECT);
  xpos_ = _x;
  ypos_ = _y;

  if (completed () && setMaxLevel_[set ()] <= level () && setSize_[set ()] > level ()+1) {
    char buf[256];
    unsigned long x=(((unsigned long) getuid ()) & 0xfffful);
    x |= ((unsigned long) set ())<<26;
    x |= ((unsigned long) (level ()+1))<<16;

    setMaxLevel_[set ()] = level ()+1;

    x = forward (x, 0x608aef3ful, 0x0dul, 0xfb00ef3bul);
    x = forward (x, 0x8bb02c07ul, 0x12ul, 0x2a37dd29ul);
    x = forward (x, 0x709e62a3ul, 0x17ul, 0x23607603ul);
    x = forward (x, 0x207d488bul, 0x0bul, 0xc31fd579ul);
    x = forward (x, 0x61ebc208ul, 0x16ul, 0xbcffadadul);
    x = forward (x, 0xf6c75e2cul, 0x02ul, 0xa2baa00ful);
    x = forward (x, 0xd4d657b4ul, 0x1ful, 0x7e129575ul);
    x = forward (x, 0x80ff0b94ul, 0x0eul, 0x92fc153dul);

    sprintf (buf, "status%d", collection_save_id[set ()]);

    (KApplication::getKApplication ())->getConfig ()->writeEntry (buf, x, true, false, false);
    (KApplication::getKApplication ())->getConfig ()->sync ();
  }

  return true;
}

bool
LevelMap::unmove (int _x, int _y) {
  return move (_x, _y);
}

bool
LevelMap::unpush (int _x, int _y) {
  assert (!badCoords (xpos_, ypos_));
  assert (empty (xpos_, ypos_));

  int xd=0, yd=0;
  if (_x < xpos_) xd = -1;
  if (_x > xpos_) xd =  1;
  if (_y < ypos_) yd = -1;
  if (_y > ypos_) yd =  1;
  if (badDelta (xd, yd) || badCoords (_x+xd, _y+yd)) return false;

  int x=xpos_, y=ypos_;
  if (!object (x-xd, y-yd)) return false;

  do {
    x += xd;
    y += yd;
    if (!empty (x, y)) return false;
  } while (!(x==_x && y==_y));

  clearMap (xpos_-xd, ypos_-yd, OBJECT);
  setMap (_x-xd, _y-yd, OBJECT);
  xpos_ = _x;
  ypos_ = _y;

  return true;
}

