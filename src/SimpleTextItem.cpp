/*
    SPDX-FileCopyrightText: 2023 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "SimpleTextItem.h"

SimpleTextItem::SimpleTextItem(QGraphicsItem *parent)
    : QGraphicsSimpleTextItem(parent)
{
    setGraphicsItem(this);

}

SimpleTextItem::SimpleTextItem(const QString &text, QGraphicsItem *parent)
    : QGraphicsSimpleTextItem(text, parent)
{
    setGraphicsItem(this);
}

void SimpleTextItem::setGeometry(const QRectF &geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(geom.topLeft());
}

QSizeF SimpleTextItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    Q_UNUSED(which);
    Q_UNUSED(constraint);
    switch (which) {
    case Qt::MinimumSize:
        return QSizeF();
    default:
    // case Qt::PreferredSize
    return boundingRect().size();
    };
}
