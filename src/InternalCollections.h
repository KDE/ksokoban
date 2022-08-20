/*
    SPDX-FileCopyrightText: 1998-2000 Anders Widell <awl@hem.passagen.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef INTERNALCOLLECTIONS_H
#define INTERNALCOLLECTIONS_H

#include <cassert>
#include <QString>
#include <QVector>

#include "LevelCollection.h"

class InternalCollections {
public:
  InternalCollections();
  ~InternalCollections();

  static int toInternalId(int _id) {
    if (_id < 10 || _id > 14) return 1000;
    return _id - 10;
  }

  int collections();
  LevelCollection *operator[](int n);

private:
  void add(LevelCollection* c);

  static int configCollection2Real(int collection);
  static int realCollection2Config(int collection);
  static QString collectionName(int _level);

  QVector<LevelCollection*> collections_;
  char *data_;

};

#endif  /* INTERNALCOLLECTIONS_H */
