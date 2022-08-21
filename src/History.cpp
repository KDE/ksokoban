/*
    SPDX-FileCopyrightText: 1998 Anders Widell <awl@hem.passagen.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "History.h"

#include "LevelMap.h"
#include "Move.h"
#include "MoveSequence.h"

#include <QList>

History::History()
{
    // past_.setAutoDelete(true);
    // future_.setAutoDelete(true);
}
History::~History()
{
    for (QList<Move *>::iterator it = past_.begin(); it != past_.end(); it++)
        delete *it;
    for (QList<Move *>::iterator it = future_.begin(); it != future_.end(); it++)
        delete *it;
}

void History::add(Move *_m)
{
    future_.clear();
    past_.append(_m);
}

void History::clear()
{
    past_.clear();
    future_.clear();
}

void History::save(QString &_str)
{
    for (QList<Move *>::Iterator iterator = past_.begin(); iterator != past_.end(); ++iterator) {
        (*iterator)->save(_str);
    }
    _str += QLatin1Char('-');

    for (QList<Move *>::Iterator iterator = future_.begin(); iterator != future_.end(); ++iterator) {
        (*iterator)->save(_str);
    }
}

const char *History::load(LevelMap *map, const char *_str)
{
    Move *m;
    int x = map->xpos();
    int y = map->ypos();

    clear();
    while (*_str != '\0' && *_str != '-') {
        m = new Move(x, y);
        _str = m->load(_str);
        if (_str == nullptr)
            return nullptr;
        x = m->finalX();
        y = m->finalY();
        past_.append(m);
        if (!m->redo(map)) {
            // printf("redo failed: %s\n", _str);
            // abort();
            return nullptr;
        }
    }
    if (*_str != '-')
        return nullptr;

    _str++;
    while (*_str != '\0') {
        m = new Move(x, y);
        _str = m->load(_str);
        if (_str == nullptr)
            return nullptr;
        x = m->finalX();
        y = m->finalY();
        future_.append(m);
    }

    return _str;
}

bool History::redo(LevelMap *map)
{
    if (future_.isEmpty())
        return false;

    Move *m = future_.takeAt(0);
    past_.append(m);
    return m->redo(map);
}

MoveSequence *History::deferRedo(LevelMap *map)
{
    if (future_.isEmpty())
        return nullptr;

    Move *m = future_.takeAt(0);
    past_.append(m);
    return new MoveSequence(m, map);
}

bool History::undo(LevelMap *map)
{
    if (past_.isEmpty())
        return false;

    Move *m = past_.takeAt(past_.count() - 1);
    future_.insert(0, m);
    return m->undo(map);
}

MoveSequence *History::deferUndo(LevelMap *map)
{
    if (past_.isEmpty())
        return nullptr;

    Move *m = past_.takeAt(past_.count() - 1);
    future_.insert(0, m);
    return new MoveSequence(m, map, true);
}
