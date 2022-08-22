/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MAPDELTA_H
#define MAPDELTA_H

#include "Map.h"

#include <cassert>

class MapDelta : private Map
{
public:
    explicit MapDelta(Map *m);

public:
    void start();
    void end();

    bool hasChanged(int x, int y) const
    {
        assert(ended_);
        return map(x, y) == 1;
    }

private:
    Map *source_;
    bool ended_ = true;
};

#endif /* MAPDELTA_H */
