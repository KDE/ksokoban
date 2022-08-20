/*
    SPDX-FileCopyrightText: 1998 Anders Widell <awl@hem.passagen.se>

    SPDX-License-Identifier: GPL-2.0-or-later
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
