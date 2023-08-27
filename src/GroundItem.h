/*
    SPDX-FileCopyrightText: 2022 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef GROUNDITEM_H
#define GROUNDITEM_H

#include "Map.h"
#include "StoneIndex.h"

#include <QGraphicsItem>
#include <QGraphicsLayoutItem>

class KGameRenderer;
class QPixmap;

class GroundItem : public QGraphicsLayoutItem, public QGraphicsItem
{
public:
    GroundItem(const Map *map, KGameRenderer *renderer, QGraphicsItem *parent = nullptr);

public:
    int squareSize() const;

    QPoint squareFromScene(QPointF scenePos) const;
    QPointF squareToScene(QPoint square) const;

    void updateSquares();

public: // QGraphicsItem API
    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

public: // QGraphicsLayoutItem API
    void setGeometry(const QRectF &geom) override;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const override;

private:
    QPixmap stonePixmap(int stoneIndex) const;
    QPixmap halfStonePixmap(int stoneIndex) const;

    void paintWall(int x, int y, QPainter *painter);
    void paintSquare(int x, int y, QPainter *painter);

    int squareX(qreal x) const;
    int squareY(qreal y) const;

    int squareToX(int x) const;
    int squareToY(int y) const;

private:
    // TODO: look into making the item a floor 0,0-based again, with positioning externally managed
    QPoint m_contentOffset;

    int m_squareSize = 0;
    int m_halfSquareSize = 0;

    KGameRenderer * const m_renderer;
    StoneIndex m_stoneIndex;
    const Map * const m_map;
};


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

inline int GroundItem::squareToX(int x) const
{
    return m_squareSize * x;
}
inline int GroundItem::squareToY(int y) const
{
    return m_squareSize * y;
}

#endif
