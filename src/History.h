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

#ifndef HISTORY_H
#define HISTORY_H

#include <QList>
#include <QString>

#include "Move.h"
class MoveSequence;

/**
 * Maintains movement history
 *
 * @short   Maintains game movement history
 * @author  Anders Widell <d95-awi@nada.kth.se>
 * @version 0.1
 * @see     PlayField
 */

class History {
private:
  QList<Move*> past_;
  QList<Move*> future_;

protected:

public:
  History();
  ~History();
  /**
   * Add a move to the history. Deletes all currently undone moves.
   */
  void add(Move *_m);
  /**
   * Clear the history and delete all Move objects stored in it.
   */
  void clear();

  void save(QString &_str);
  const char *load(LevelMap *map, const char *_str);
  bool redo(LevelMap *map);
  MoveSequence *deferRedo(LevelMap *map);
  bool undo(LevelMap *map);
  MoveSequence *deferUndo(LevelMap *map);
};

#endif  /* HISTORY_H */
