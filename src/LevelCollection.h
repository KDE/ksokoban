/*
 *  ksokoban - a Sokoban game by KDE
 *  Copyright (C) 1998,1999  Anders Widell  <d95-awi@nada.kth.se>
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

#ifndef LEVELCOLLECTION_H
#define LEVELCOLLECTION_H

#include <QString>
#include <QByteArray>
#include <QVector>

class Map;

class LevelCollection {
public:
  LevelCollection(const char *_def, int _len, const QString &_name, int _id=-1);
  LevelCollection(const QString &_path, const QString &_name, int _id=-1);
  ~LevelCollection();

  const QString &name() const { return name_; }
  int id() const { return id_; }
  int level() const { return level_; }
  void level(int _level);
  void levelCompleted();
  int completedLevels() const { return completedLevels_; }
  int noOfLevels() const { return noOfLevels_; }
  bool loadLevel(Map *_map);

protected:
  void indexTextCollection();
  void loadPrefs();


private:
  void addLevel(const char* _level);
  void addData(const char* _data, unsigned _len);
  void addSeparator();

  QVector<const char*> index_;
  QByteArray    data_;
  //int    dataLen_;

  int      level_;
  int      completedLevels_;
  int      noOfLevels_;
  QString  name_;
  QString  path_;
  int      id_;
};

#endif  /* LEVELCOLLECTION_H */
