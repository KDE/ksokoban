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

#include "config.h"

#include <kconfig.h>
#include <kapp.h>

#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif

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

int
LevelMap::configCollection2Real (int collection) {
  for (int i=0; i < (int) (sizeof (collection_save_id) / sizeof (int)); i++) {
    if (collection_save_id[i] == collection) return i;
  }
  return 0;
}

int
LevelMap::realCollection2Config (int collection) {
  assert (collection < (int) (sizeof (collection_save_id) / sizeof (int)));
  return collection_save_id[collection];
}

LevelMap::LevelMap () {
  KConfig *cfg=(KApplication::getKApplication ())->getConfig ();

#ifdef HAVE_ZLIB_H
  data = (char *) malloc (BUFSIZE);
  if (data == NULL) abort ();

  datasize = BUFSIZE;
  uncompress ((unsigned char *) data, (long unsigned int *) &datasize, level_data, sizeof (level_data));
  data = (char *) realloc (data, datasize);
  if (data == NULL) abort ();
#else
  datasize = sizeof (level_data);
  data = (char *) malloc (datasize);
  if (data == NULL) abort ();
  memcpy (data, level_data, datasize);
#endif

  totalLevels_ = noOfCollections_ = 0;
  bool newLevel=true;
  for (int pos=0; pos<datasize; pos++) {
    while (newLevel && pos < datasize-1 && data[pos] == '\n') pos++;
    if (newLevel && isalpha (data[pos])) {
      noOfCollections_++;
    }
    //if (data[pos] == '\n' && data[pos-1] == '\n') data[pos] = '\0';
    if (data[pos] == '\0') {
      totalLevels_++;
      newLevel = true;
    } else {
      newLevel = false;
    }
  }
  //printf ("totalLevels_ = %d noOfCollections_ = %d\n", totalLevels_, noOfCollections_);

  levelIndex_ = new char *[totalLevels_];
  collectionNames_   = new char *[noOfCollections_];
  collectionIndex_   = new int[noOfCollections_];
  collectionSize_    = new int[noOfCollections_];
  collectionCurrentLevel_    = new int[noOfCollections_];
  collectionMaxLevel_    = new int[noOfCollections_];

  char *pos=data;
  int collection=0, collectionSize=0;
  for (int lev=0; lev<totalLevels_; lev++) {
    while (*pos == '\n') pos++;
    if (isalpha (*pos)) {
      collectionNames_[collection] = pos;
      collectionIndex_[collection] = lev;
      if (collection) {
	collectionSize_[collection-1] = collectionSize;
	//printf ("collectionSize[%d]=%d\n", collection-1, collectionSize);
      }
      collectionSize = 0;
      collection++;
      while (*pos != '\n') pos++;
      *pos++ = '\0';
    }
    levelIndex_[lev] = pos;
    while (*pos != '\0') pos++;
    pos++;
    collectionSize++;
  }
  if (collection) {
    collectionSize_[collection-1] = collectionSize;
    //printf ("collectionSize[%d]=%d\n", collection-1, collectionSize);
  }
  
  for (int i=0; i<noOfCollections_; i++) {
    char buf[256];
    unsigned long x;

    assert (i < (int) (sizeof (collection_save_id) / sizeof (int)));
    sprintf (buf, "level%d", realCollection2Config (i));
    collectionCurrentLevel_[i] = cfg->readNumEntry (buf, 0);

    sprintf (buf, "status%d", realCollection2Config (i));

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

    collectionMaxLevel_[i] = x>>16 & 0x3ff;
    //printf (":::%d\n", collectionMaxLevel_[3]);
    if (((x>>26) & 0x3ful) != (unsigned long) realCollection2Config (i)) collectionMaxLevel_[i] = 0;
    if ((x & 0xfffful) != (unsigned long) getuid ()) collectionMaxLevel_[i] = 0;
    if (collectionMaxLevel_[i] >= collectionSize_[i]) collectionMaxLevel_[i] = 0;
    if (collectionCurrentLevel_[i] > collectionMaxLevel_[i]) collectionCurrentLevel_[i] = collectionMaxLevel_[i];
    if (!cfg->hasKey (buf)) collectionMaxLevel_[i] = 0;
  }

  changeCollection (configCollection2Real (cfg->readNumEntry ("collection", 0)));
}

LevelMap::~LevelMap () {
  KConfig *cfg=(KApplication::getKApplication ())->getConfig ();
  char buf[256];

  for (int i=0; i<noOfCollections_; i++) {
    sprintf (buf, "level%d", realCollection2Config (i));
    cfg->writeEntry (buf, collectionCurrentLevel_[i], true, false, false);
  }
  cfg->writeEntry ("collection", realCollection2Config (collection ()));
  

  delete [] collectionCurrentLevel_;
  delete [] collectionMaxLevel_;
  delete [] collectionSize_;
  delete [] collectionIndex_;
  delete [] collectionNames_;
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
LevelMap::changeCollection (int _collection)
{
  if (_collection < 0 || _collection >= noOfCollections_) _collection = 0;
  //if (collection_ == _collection) return;
  collection_ = _collection;
  level (level ());
}

void
LevelMap::level (int _level) {
  int goalsLeft=0;
  minX_ = MAX_X;
  minY_ = MAX_Y;
  maxX_ = maxY_ = 0;
  totalMoves_ = totalPushes_ = 0;

  //printf ("collection_: %d, collectionMaxLevel_: %d\n", collection_, collectionMaxLevel_[collection_]);
  //printf ("level(%d): completedLevels=%d, noOfLevels=%d\n", _level, completedLevels (), noOfLevels ());

  if (_level >= completedLevels ()) _level = completedLevels ();
  if (_level >= noOfLevels ()) _level = noOfLevels ()-1;
  if (_level < 0) _level = 0;
  collectionCurrentLevel_[collection ()] = _level;

  //printf ("level: %d\n", _level);

  clearMap ();

  int x=0, y=0;
  for (char *pos=levelIndex_[_level+collectionIndex_[collection ()]]; *pos; pos++) {
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

  fillFloor (xpos_, ypos_);

  //emit levelChange ();
}


int
LevelMap::distance (int x1, int y1, int x2, int y2) {
  int d;

  if (x2 > x1) d = x2-x1;
  else d = x1-x2;

  if (y2 > y1) d += y2-y1;
  else d += y1-y2;

  return d;
}

bool
LevelMap::step (int _x, int _y) {
  int oldX=xpos_, oldY=ypos_;

  bool success = Map::step (_x, _y);

  totalMoves_ += distance (oldX, oldY, xpos_, ypos_);

  return success;
}

bool
LevelMap::push (int _x, int _y) {
  int oldX=xpos_, oldY=ypos_;

  bool success = Map::push (_x, _y);

  int d = distance (oldX, oldY, xpos_, ypos_);
  totalMoves_ += d;
  totalPushes_ += d;

  if (completed () && collectionMaxLevel_[collection ()] <= level () && collectionSize_[collection ()] > level ()+1) {
    char buf[256];
    unsigned long x=(((unsigned long) getuid ()) & 0xfffful);
    x |= ((unsigned long) realCollection2Config (collection ()))<<26;
    x |= ((unsigned long) (level ()+1))<<16;

    collectionMaxLevel_[collection ()] = level ()+1;

    x = forward (x, 0x608aef3ful, 0x0dul, 0xfb00ef3bul);
    x = forward (x, 0x8bb02c07ul, 0x12ul, 0x2a37dd29ul);
    x = forward (x, 0x709e62a3ul, 0x17ul, 0x23607603ul);
    x = forward (x, 0x207d488bul, 0x0bul, 0xc31fd579ul);
    x = forward (x, 0x61ebc208ul, 0x16ul, 0xbcffadadul);
    x = forward (x, 0xf6c75e2cul, 0x02ul, 0xa2baa00ful);
    x = forward (x, 0xd4d657b4ul, 0x1ful, 0x7e129575ul);
    x = forward (x, 0x80ff0b94ul, 0x0eul, 0x92fc153dul);

    sprintf (buf, "status%d", realCollection2Config (collection ()));

    KConfig *cfg=(KApplication::getKApplication ())->getConfig ();
    cfg->writeEntry (buf, x, true, false, false);
    cfg->sync ();
  }

  return success;
}

bool
LevelMap::unstep (int _x, int _y) {
  int oldX=xpos_, oldY=ypos_;

  bool success = Map::unstep (_x, _y);

  totalMoves_ -= distance (oldX, oldY, xpos_, ypos_);

  return success;
}

bool
LevelMap::unpush (int _x, int _y) {
  int oldX=xpos_, oldY=ypos_;

  bool success = Map::unpush (_x, _y);

  int d = distance (oldX, oldY, xpos_, ypos_);
  totalMoves_ -= d;
  totalPushes_ -= d;

  return success;
}

#if 0
void
LevelMap::random (void) {
  printf ("start!\n");

  minX_ = 0;
  minY_ = 0;
  maxX_ = MAX_X;
  maxY_ = MAX_Y;
  totalMoves_ = totalPushes_ = 0;
  clearMap ();
  
  xpos_ = 13;
  ypos_ = 9;

  for (int i=0; i<200; i++) {
    map (xpos_, ypos_, FLOOR);

    switch (rand () & 3) {
    case 0:
      if (ypos_ > 1) ypos_--; else i--;
      break;

    case 1:
      if (ypos_ < MAX_Y-1) ypos_++; else i--;
      break;

    case 2:
      if (xpos_ > 1) xpos_--; else i--;
      break;

    case 3:
      if (xpos_ < MAX_X-1) xpos_++; else i--;
      break;
    }
  }

  for (int y=1; y<MAX_Y; y++) {
    for (int x=1; x<MAX_X; x++) {
      if (map (x, y) & FLOOR) {
	if (!(map (x, y-1) & FLOOR)) map (x, y-1, WALL);
	if (!(map (x, y+1) & FLOOR)) map (x, y+1, WALL);
	if (!(map (x-1, y) & FLOOR)) map (x-1, y, WALL);
	if (!(map (x+1, y) & FLOOR)) map (x+1, y, WALL);
      }
    }
  }

  printf ("klar!\n");
  printMap ();
}
#endif
