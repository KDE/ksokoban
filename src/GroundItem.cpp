/*
    SPDX-FileCopyrightText: 2022 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "GroundItem.h"

#include <KGameRenderer>

#include <QApplication>
#include <QStyleOptionGraphicsItem>
#include <QPainter>

#include <cmath>

// hardcoded for now, featch from theme
static constexpr int SMALL_STONES = 4;
static constexpr int LARGE_STONES = 6;

static constexpr int MIN_SQUARE_SIZE = 8;

GroundItem::GroundItem(const Map *map, KGameRenderer *renderer, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_renderer(renderer)
    , m_map(map)
{
    setGraphicsItem(this);
    m_stoneIndex.setStoneCount(LARGE_STONES, SMALL_STONES);
}

QPoint GroundItem::squareFromScene(QPointF scenePos) const
{
    const QPointF cnntentPos = mapFromScene(scenePos) - m_contentOffset;

    // using QPointF to also cover ]-1,0[
    const QPointF squarePos = cnntentPos / m_squareSize;

    if ((squarePos.x() < 0 ) || (squarePos.x() >= m_map->width()) ||
        (squarePos.y() < 0 ) || (squarePos.y() >= m_map->height())) {
        return {-1, -1};
    }

    return {static_cast<int>(squarePos.x()), static_cast<int>(squarePos.y())};
}

QPointF GroundItem::squareToScene(QPoint square) const
{
    return mapToScene(QPointF{static_cast<qreal>(m_squareSize * square.x() + m_contentOffset.x()),
                              static_cast<qreal>(m_squareSize * square.y() + m_contentOffset.y())});
}

QRectF GroundItem::boundingRect() const
{
    const int cols = m_map->width();
    const int rows = m_map->height();
    const int width = cols * m_squareSize;
    const int height = rows * m_squareSize;

    return QRectF(m_contentOffset, QSizeF(width, height));
}

void GroundItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    if (m_squareSize <= 0)
        return;

    painter->save();;
    painter->translate(m_contentOffset);

    const QRectF exposedContentRect = option->exposedRect.translated(-m_contentOffset.x(), -m_contentOffset.y());

    int minx = squareX(exposedContentRect.x());
    int miny = squareY(exposedContentRect.y());
    int maxx = squareX(exposedContentRect.x() + exposedContentRect.width() - 1);
    int maxy = squareY(exposedContentRect.y() + exposedContentRect.height() - 1);

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
    painter->restore();;
}

void GroundItem::setGeometry(const QRectF &geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    // use updated data
    const QRectF geometry = this->geometry();
    setPos(geometry.topLeft());

    updateSquares();
}

void GroundItem::updateSquares()
{
    // use updated data
    const QRectF geometry = this->geometry();

    const int cols = m_map->width();
    const int rows = m_map->height();

    // FIXME: the line below should not be needed
    if (cols == 0 || rows == 0)
        return;

    // for now render aligned to pixels
    int xsize = std::floor(geometry.width() / cols);
    int ysize = std::floor(geometry.height() / rows);

    if (xsize < MIN_SQUARE_SIZE)
        xsize = MIN_SQUARE_SIZE;
    if (ysize < MIN_SQUARE_SIZE)
        ysize = MIN_SQUARE_SIZE;

    m_squareSize = (xsize > ysize ? ysize : xsize);
    // ensure even number, for half squares to be aligned to pixels
    m_squareSize &= ~1;
    m_halfSquareSize = m_squareSize / 2;

    const int groundX = (geometry.width() - cols * m_squareSize) / 2;
    const int groundY = (geometry.height()- rows * m_squareSize) / 2;

    m_contentOffset.setX(groundX);
    m_contentOffset.setY(groundY);

    // qDebug() << "GROUNDITEM::SETGEOMETRY" << geom << "squaresize" << m_squareSize;
    update();
}

QSizeF GroundItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    Q_UNUSED(constraint);
    switch (which) {
    case Qt::MinimumSize: {
        const int cols = m_map->width();
        const int rows = m_map->height();
        const int width = MIN_SQUARE_SIZE * cols;
        const int height = MIN_SQUARE_SIZE * rows;

        return QSizeF(width, height);
    }
    default:
        return QSizeF();
    };
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
