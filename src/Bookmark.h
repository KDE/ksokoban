/*
    SPDX-FileCopyrightText: 1998 Anders Widell <awl@hem.passagen.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <QString>

class History;
class LevelMap;

class Bookmark
{
public:
    explicit Bookmark(int _num);

public:
    int collection() const
    {
        return collection_;
    }
    int level() const
    {
        return level_;
    }
    int moves() const
    {
        return moves_;
    }
    // int pushes() { return pushes_; }

    void set(int _collection, int _level, int _moves, History *_h);
    bool goTo(LevelMap *_map, History *_h);

private:
    void fileName(QString &p) const;

private:
    int number_;
    int collection_;
    int level_;
    int moves_;
    // int     pushes_;
    QString data_;
};

#endif /* BOOKMARK_H */
