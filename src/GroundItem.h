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
    void setSquareSize(int size);

    int squareSize() const;

    int squareX(qreal x) const;
    int squareY(qreal y) const;
    QPoint squareFromScene(QPointF scenePos) const;
    QPointF squareToScene(QPoint square) const;

public:
    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QPixmap stonePixmap(int stoneIndex) const;
    QPixmap halfStonePixmap(int stoneIndex) const;

    void paintWall(int x, int y, QPainter *painter);
    void paintSquare(int x, int y, QPainter *painter);

    int squareToX(int x) const;
    int squareToY(int y) const;

private:
    QRect m_boundRect;

    int m_squareSize = 0;
    int m_halfSquareSize = 0;

    KGameRenderer * const m_renderer;
    StoneIndex m_stoneIndex;
    const Map * const m_map;
};

inline void GroundItem::setSquareSize(int size)
{
    m_squareSize = size;
    m_halfSquareSize = m_squareSize / 2;
}

inline int GroundItem::squareSize() const
{
    return m_squareSize;
}

inline int GroundItem::squareX(qreal x) const
{
    return x / m_squareSize;
}
inline int GroundItem::squareY(qreal y) const
{
    return y / m_squareSize;
}

inline QPoint GroundItem::squareFromScene(QPointF scenePos) const
{
    const QPointF pos = mapFromScene(scenePos);
    return QPoint(pos.x() / m_squareSize, pos.y() / m_squareSize);
}

inline QPointF GroundItem::squareToScene(QPoint square) const
{
    return mapToScene(QPointF{static_cast<qreal>(m_squareSize * square.x()),
                              static_cast<qreal>(m_squareSize * square.y())});
}

inline int GroundItem::squareToX(int x) const
{
    return m_squareSize * x;
}
inline int GroundItem::squareToY(int y) const
{
    return m_squareSize * y;
}

#endif
