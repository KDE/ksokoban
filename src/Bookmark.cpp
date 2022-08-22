/*
    SPDX-FileCopyrightText: 1998 Anders Widell <awl@hem.passagen.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "Bookmark.h"

#include "History.h"
#include "LevelMap.h"

#include <QDir>
#include <QFile>
#include <QStandardPaths>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cassert>
#include <cstdio>

void Bookmark::fileName(QString &p) const
{
    p = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/bookmark%1").arg(number_);
}

Bookmark::Bookmark(int _num)
    : number_(_num)
{
    QString p;
    fileName(p);
    FILE *file = fopen(QFile::encodeName(p).constData(), "r");
    if (file == nullptr)
        return;

    char buf[4096];
    buf[0] = '\0';
    fgets(buf, 4096, file);
    if (sscanf(buf, "%d %d %d", &collection_, &level_, &moves_) != 3) {
        collection_ = level_ = -1;
        data_ = QString();
        fclose(file);
        return;
    }

    data_ = QString();
    int len;
    while (!feof(file)) {
        len = fread(buf, 1, 4095, file);
        if (ferror(file))
            break;
        buf[len] = '\0';
        data_ += QString::fromLatin1(buf);
    }
    fclose(file);

    data_ = data_.trimmed();
}

void Bookmark::set(int _collection, int _level, int _moves, History *_h)
{
    assert(_collection >= 0);
    if (_collection < 0)
        return;

    collection_ = _collection;
    level_ = _level;
    moves_ = _moves;

    data_ = QString();
    _h->save(data_);

    // ensure folder exists
    QDir bookmarksDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if (!bookmarksDir.mkpath(QStringLiteral("."))) {
        return;
    }
    QString p;
    fileName(p);
    FILE *file = fopen(QFile::encodeName(p).constData(), "w");
    if (file == nullptr)
        return;
    fprintf(file, "%d %d %d\n", collection_, level_, moves_);
    fprintf(file, "%s\n", data_.toLatin1().constData());
    fclose(file);
}

bool Bookmark::goTo(LevelMap *_map, History *_h)
{
    return _h->load(_map, data_.toLatin1().constData()) != nullptr;
}
