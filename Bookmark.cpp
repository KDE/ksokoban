/*
 *  ksokoban - a Sokoban game for KDE
 *  Copyright (C) 1998  Anders Widell  <awl@hem.passagen.se>
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

#include "Bookmark.h"
#include "History.h"
#include "LevelMap.h"

#include <qfile.h>

#include <kapplication.h>

#include <kglobal.h>
#include <kstandarddirs.h>

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <assert.h>

void
Bookmark::fileName(QString &p) {
  p = KGlobal::dirs()->saveLocation("appdata");

  QString n;
  n.setNum(number_);
  p += "/bookmark" + n;
}

Bookmark::Bookmark(int _num) :
  number_(_num), collection_(-1), level_(-1), moves_(0), data_("") {

  QString p;
  fileName(p);

  FILE *file = fopen(p.latin1(), "r");
  if (file == NULL) return;

  char buf[4096];
  buf[0] = '\0';
  fgets (buf, 4096, file);
  if (sscanf(buf, "%d %d %d", &collection_, &level_, &moves_) != 3) {
    collection_ = level_ = -1;
    data_ = "";
    fclose(file);
    return;
  }

  data_ = "";
  int len;
  while (!feof(file)) {
    len = fread(buf, 1, 4095, file);
    if (ferror(file)) break;
    buf[len] = '\0';
    data_ += buf;
  }
  fclose(file);        

  data_ = data_.stripWhiteSpace();
}



void
Bookmark::set(int _collection, int _level, int _moves, History *_h) {
  assert(_collection >= 0);
  if (_collection < 0) return;

  collection_ = _collection;
  level_ = _level;
  moves_ = _moves;

  data_ = "";
  _h->save(data_);
  
  QString p;
  fileName(p);
  FILE *file = fopen(QFile::encodeName(p), "w");
  if (file == NULL) return;
  fprintf(file, "%d %d %d\n", collection_, level_, moves_);
  fprintf(file, "%s\n", data_.latin1());
  fclose(file);
}

bool
Bookmark::goTo(LevelMap *_map, History *_h) {
  return _h->load(_map, data_.latin1()) != 0;
}


