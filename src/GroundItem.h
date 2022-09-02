/*
    SPDX-FileCopyrightText: 2022 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef GROUNDITEM_H
#define GROUNDITEM_H

#include "Map.h"
#include "StoneIndex.h"

#include <QGraphicsItem>

class KGameRenderer;
class QPixmap;

class GroundItem : public QGraphicsItem
{
public:
    GroundItem(const Map *map, KGameRenderer *renderer, QGraphicsItem *parent = nullptr);

public:
    void setSize(int size);

public:
    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QPixmap stonePixmap(int stoneIndex) const;
    QPixmap halfStonePixmap(int stoneIndex) const;

    void paintWall(int x, int y, QPainter *painter);
    void paintSquare(int x, int y, QPainter *painter);

    int x2pixel(int x) const;
    int y2pixel(int y) const;

    int pixel2x(int x) const;
    int pixel2y(int y) const;

private:
    QRect m_boundRect;

    int m_size = 0;
    int m_halfSize = 0;

    KGameRenderer * const m_renderer;
    StoneIndex m_stoneIndex;
    const Map * const m_map;
};

inline void GroundItem::setSize(int size)
{
    m_size = size;
    m_halfSize = m_size / 2;
}

inline int GroundItem::x2pixel(int x) const
{
    return m_size * x;
}
inline int GroundItem::y2pixel(int y) const
{
    return m_size * y;
}

inline int GroundItem::pixel2x(int x) const
{
    return x / m_size;
}
inline int GroundItem::pixel2y(int y) const
{
    return y / m_size;
}

#endif
