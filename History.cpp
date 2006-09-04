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

#include "History.h"
#include "Move.h"
#include "MoveSequence.h"
#include "LevelMap.h"

History::History() {
}


void
History::add(Move *_m) {
  qDeleteAll(future_);
  future_.clear();
  past_.append(_m);
}

void
History::clear() {
  qDeleteAll(past_);
  qDeleteAll(future_);
  past_.clear();
  future_.clear();
}

void
History::save(QString &_str) {
  foreach( Move *m, past_ )
    m->save(_str);

  _str += '-';

  foreach( Move *m, future_ )
    m->save(_str);
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
    if (_str == 0) { delete m; return 0; }
    x = m->finalX();
    y = m->finalY();
    past_.append(m);
    if (!m->redo(map)) {
      //printf("redo failed: %s\n", _str);
      //abort();
      delete m;
      return 0;
    }
  }
  if (*_str != '-') { delete m; return 0; }

  _str++;
  while (*_str != '\0') {
    m = new Move(x, y);
    _str = m->load(_str);
    if (!_str) { 
      delete m;
      return 0;
    }
    x = m->finalX();
    y = m->finalY();
    future_.append(m);
  }

  return _str;
}

bool
History::redo(LevelMap *map) {
  if (future_.isEmpty()) return false;

  Move *m=future_.takeFirst();
  past_.append(m);
  return m->redo(map);
}

MoveSequence *
History::deferRedo(LevelMap *map) {
  if (future_.isEmpty()) return 0;

  Move *m=future_.takeFirst();
  past_.append(m);
  return new MoveSequence(m, map);
}

bool
History::undo(LevelMap *map) {
  if (past_.isEmpty()) return false;

  Move *m = past_.takeLast();
  future_.prepend(m);
  return m->undo(map);
}

MoveSequence *
History::deferUndo(LevelMap *map) {
  if (past_.isEmpty()) return 0;

  Move *m = past_.takeLast();
  future_.prepend(m);
  return new MoveSequence(m, map, true);
}
