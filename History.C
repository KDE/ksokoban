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

#include <qlist.h>

#include "History.H"
#include "Move.H"
#include "MoveSequence.H"
#include "LevelMap.H"

History::History () {
  past_.setAutoDelete (true);
  future_.setAutoDelete (true);
}


void
History::add (Move *_m) {
  future_.clear ();
  past_.append (_m);
}

void
History::clear () {
  past_.clear ();
  future_.clear ();
}

void
History::save (QString &_str) {
  Move *m = past_.first ();

  while (m != 0) {
    m->save (_str);
    m = past_.next ();
  }
  _str += '-';

  m = future_.first ();
  while (m != 0) {
    m->save (_str);
    m = future_.next ();
  }
}

const char *
History::load (LevelMap *map, const char *_str) {
  Move *m;
  int x = map->xpos ();
  int y = map->ypos ();

  clear ();
  while (*_str != '\0' && *_str != '-') {
    m = new Move (x, y);
    _str = m->load (_str);
    if (_str == 0) return 0;
    x = m->finalX ();
    y = m->finalY ();
    past_.append (m);
    if (!m->redo (map)) {
      //printf ("redo failed: %s\n", _str);
      //abort ();
      return 0;
    }
  }
  if (*_str != '-') return 0;

  _str++;
  while (*_str != '\0') {
    m = new Move (x, y);
    _str = m->load (_str);
    if (_str == 0) return 0;
    x = m->finalX ();
    y = m->finalY ();
    future_.append (m);
  }

  return _str;
}

bool
History::redo (LevelMap *map) {
  if (future_.isEmpty ()) return false;

  Move *m=future_.take (0);
  past_.append (m);
  return m->redo (map);
}

MoveSequence *
History::deferRedo (LevelMap *map) {
  if (future_.isEmpty ()) return 0;

  Move *m=future_.take (0);
  past_.append (m);
  return new MoveSequence (m, map);
}

bool
History::undo (LevelMap *map) {
  if (past_.isEmpty ()) return false;

  Move *m = past_.take (past_.count ()-1);
  future_.insert (0, m);
  return m->undo (map);
}

MoveSequence *
History::deferUndo (LevelMap *map) {
  if (past_.isEmpty ()) return 0;

  Move *m = past_.take (past_.count ()-1);
  future_.insert (0, m);
  return new MoveSequence (m, map, true);
}
