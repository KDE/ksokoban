/*
    SPDX-FileCopyrightText: 1998, 1999 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef LEVELCOLLECTION_H
#define LEVELCOLLECTION_H

#include <QByteArray>
#include <QString>
#include <QVector>

class Map;

class LevelCollection
{
public:
    LevelCollection(const char *_def, int _len, const QString &_name, int _id = -1);
    LevelCollection(const QString &_path, const QString &_name, int _id = -1);
    ~LevelCollection();

    const QString &name() const
    {
        return name_;
    }
    int id() const
    {
        return id_;
    }
    int level() const
    {
        return level_;
    }
    void level(int _level);
    void levelCompleted();
    int completedLevels() const
    {
        return completedLevels_;
    }
    int noOfLevels() const
    {
        return noOfLevels_;
    }
    bool loadLevel(Map *_map);

protected:
    void indexTextCollection();
    void loadPrefs();

private:
    void addLevel(const char *_level);
    void addData(const char *_data, unsigned _len);
    void addSeparator();

private:
    QVector<const char *> index_;
    QByteArray data_;
    // int    dataLen_;

    int level_;
    int completedLevels_;
    int noOfLevels_;
    QString name_;
    QString path_;
    int id_;
};

#endif /* LEVELCOLLECTION_H */
