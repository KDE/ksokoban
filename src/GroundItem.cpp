/*
    SPDX-FileCopyrightText: 2022 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "GroundItem.h"

#include <KGameRenderer>

#include <QApplication>
#include <QStyleOptionGraphicsItem>
#include <QPainter>

// hardcoded for now, featch from theme
static constexpr int SMALL_STONES = 4;
static constexpr int LARGE_STONES = 6;

GroundItem::GroundItem(const Map *map, KGameRenderer *renderer, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_renderer(renderer)
    , m_map(map)
{
    m_stoneIndex.setStoneCount(LARGE_STONES, SMALL_STONES);
}

QRectF GroundItem::boundingRect() const
{
    int cols = m_map->width();
    int rows = m_map->height();

    const int width = m_squareSize * cols;
    const int height = m_squareSize * rows;

    return QRectF(0, 0, width, height);
}

void GroundItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    if (m_squareSize <= 0)
        return;

    const QRectF &rect = option->exposedRect;

    int minx = squareX(rect.x());
    int miny = squareY(rect.y());
    int maxx = squareX(rect.x() + rect.width() - 1);
    int maxy = squareY(rect.y() + rect.height() - 1);

    if (minx < 0)
        minx = 0;
    if (miny < 0)
        miny = 0;
    if (maxx >= m_map->width())
        maxx = m_map->width() - 1;
    if (maxy >= m_map->height())
        maxy = m_map->height() - 1;

    for (int y = miny; y <= maxy; y++) {
        for (int x = minx; x <= maxx; x++) {
            paintSquare(x, y, painter);
        }
    }
}

QPixmap GroundItem::stonePixmap(int stoneIndex) const
{
    const QString spriteName = QStringLiteral("stone_%1").arg(stoneIndex);

    return m_renderer->spritePixmap(spriteName, QSize(m_squareSize, m_halfSquareSize));
}

QPixmap GroundItem::halfStonePixmap(int stoneIndex) const
{
    const QString spriteName = QStringLiteral("halfstone_%1").arg(stoneIndex);

    return m_renderer->spritePixmap(spriteName, QSize(m_halfSquareSize, m_halfSquareSize));
}

void GroundItem::paintWall(int x, int y, QPainter *painter)
{
    const int pixelX = squareToX(x);
    const int pixelY = squareToY(y);
    const int stoneIndex = x + y * (Map::MAX_X + 1);

    const qreal dpr = qApp->devicePixelRatio();
    const int deviceSize_ = m_squareSize * dpr;
    const int halfdeviceSize_ = deviceSize_ / 2;

    if (m_map->wallLeft(x, y)) {
        painter->drawPixmap(pixelX, pixelY, stonePixmap(m_stoneIndex.upperLarge(stoneIndex - 1)),
                            halfdeviceSize_, 0, -1, -1);
    } else {
        painter->drawPixmap(pixelX, pixelY, halfStonePixmap(m_stoneIndex.leftSmall(stoneIndex)));
    }

    if (m_map->wallRight(x, y)) {
        painter->drawPixmap(pixelX + m_halfSquareSize, pixelY, stonePixmap(m_stoneIndex.upperLarge(stoneIndex)),
                         0, 0, halfdeviceSize_, -1);
    } else {
        painter->drawPixmap(pixelX + m_halfSquareSize, pixelY, halfStonePixmap(m_stoneIndex.rightSmall(stoneIndex)));
    }

    painter->drawPixmap(pixelX, pixelY + m_halfSquareSize, stonePixmap(m_stoneIndex.lowerLarge(stoneIndex)));
}

void GroundItem::paintSquare(int x, int y, QPainter *painter)
{
    if (m_map->wall(x, y)) {
        paintWall(x, y, painter);
        return;
    }

    QString spriteName;
    if (m_map->xpos() == x && m_map->ypos() == y) {
        if (m_map->goal(x, y))
            spriteName = QStringLiteral("saveman");
        else {
            spriteName = QStringLiteral("man");
        }
    } else if (m_map->empty(x, y)) {
        if (m_map->floor(x, y)) {
            if (m_map->goal(x, y))
                spriteName = QStringLiteral("goal");
            else {
                // shortcut for now, replace with theme pixmap (or color property)
                painter->fillRect(squareToX(x), squareToY(y), m_squareSize, m_squareSize, QColor(0x67, 0x67, 0x67, 255));
                return;
            }
        }
    } else if (m_map->object(x, y)) {
        // TODO: add highlighting & other states to KGameRenderer
#if 0
        if (highlightX_ == x && highlightY_ == y) {
            if (m_map->goal(x, y))
                imageData_->brightTreasure(paint, squareToX(x), squareToY(y));
            else
                imageData_->brightObject(paint, squareToX(x), squareToY(y));
            return;
        } else
#endif
        {
            if (m_map->goal(x, y))
                spriteName = QStringLiteral("treasure");
            else
                spriteName = QStringLiteral("object");
        }
    }

    if (spriteName.isEmpty()) {
        return;
    }

    const QPixmap pixmap = m_renderer->spritePixmap(spriteName, QSize(m_squareSize, m_squareSize));
    painter->drawPixmap(squareToX(x), squareToY(y), pixmap);
}
