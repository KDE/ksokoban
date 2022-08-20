/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MAPDELTA_H
#define MAPDELTA_H

#include <assert.h>

#include "Map.h"

class MapDelta : private Map {
public:
  MapDelta (Map *m);

  void start ();
  void end ();

  bool hasChanged (int x, int y) {
    assert (ended_);
    return map (x, y) == 1;
  }

private:
  Map      *source_;
  bool      ended_;
};

#endif  /* MAPDELTA_H */
