/*
    SPDX-FileCopyrightText: 1998 Anders Widell <awl@hem.passagen.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef HISTORY_H
#define HISTORY_H

#include "Move.h"

#include <QList>
#include <QString>

class MoveSequence;

/**
 * Maintains movement history
 *
 * @short   Maintains game movement history
 * @author  Anders Widell <d95-awi@nada.kth.se>
 * @version 0.1
 * @see     PlayField
 */
class History
{
public:
    History();
    ~History();

public:
    /**
     * Add a move to the history. Deletes all currently undone moves.
     */
    void add(Move *_m);
    /**
     * Clear the history and delete all Move objects stored in it.
     */
    void clear();

    void save(QString &_str) const;
    const char *load(LevelMap *map, const char *_str);
    bool redo(LevelMap *map);
    MoveSequence *deferRedo(LevelMap *map);
    bool undo(LevelMap *map);
    MoveSequence *deferUndo(LevelMap *map);

private:
    QList<Move *> past_;
    QList<Move *> future_;
};

#endif /* HISTORY_H */
