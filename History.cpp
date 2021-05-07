/*
 *  ksokoban - a Sokoban game by KDE
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

#include <QList>

#include "History.h"
#include "Move.h"
#include "MoveSequence.h"
#include "LevelMap.h"

History::History() {
  //past_.setAutoDelete(true);
  //future_.setAutoDelete(true);
}
History::~History() {
	for(QList<Move*>::iterator it=past_.begin(); it!=past_.end(); it++)
		delete *it;
	for(QList<Move*>::iterator it=future_.begin(); it!=future_.end(); it++)
		delete *it;
}

void
History::add(Move *_m) {
  future_.clear();
  past_.append(_m);
}

void
History::clear() {
  past_.clear();
  future_.clear();
}

void
History::save(QString &_str) {
  

  for(QList<Move*>::Iterator iterator = past_.begin(); iterator != past_.end(); ++iterator) {
    (*iterator)->save(_str);
  }
  _str += '-';

  
  for(QList<Move*>::Iterator iterator = future_.begin(); iterator != future_.end(); ++iterator) {
    (*iterator)->save(_str);
  }
}

const char *
History::load(LevelMap *map, const char *_str) {
  Move *m;
  int x = map->xpos();
  int y = map->ypos();

  clear();
  while (*_str != '\0' && *_str != '-') {
    m = new Move(x, y);
    _str = m->load(_str);
    if (_str == nullptr) return nullptr;
    x = m->finalX();
    y = m->finalY();
    past_.append(m);
    if (!m->redo(map)) {
      //printf("redo failed: %s\n", _str);
      //abort();
      return nullptr;
    }
  }
  if (*_str != '-') return nullptr;

  _str++;
  while (*_str != '\0') {
    m = new Move(x, y);
    _str = m->load(_str);
    if (_str == nullptr) return nullptr;
    x = m->finalX();
    y = m->finalY();
    future_.append(m);
  }

  return _str;
}

bool
History::redo(LevelMap *map) {
  if (future_.isEmpty()) return false;

  Move *m=future_.takeAt(0);
  past_.append(m);
  return m->redo(map);
}

MoveSequence *
History::deferRedo(LevelMap *map) {
  if (future_.isEmpty()) return nullptr;

  Move *m=future_.takeAt(0);
  past_.append(m);
  return new MoveSequence(m, map);
}

bool
History::undo(LevelMap *map) {
  if (past_.isEmpty()) return false;

  Move *m = past_.takeAt(past_.count ()-1);
  future_.insert(0, m);
  return m->undo(map);
}

MoveSequence *
History::deferUndo(LevelMap *map) {
  if (past_.isEmpty()) return nullptr;

  Move *m = past_.takeAt(past_.count()-1);
  future_.insert(0, m);
  return new MoveSequence(m, map, true);
}
