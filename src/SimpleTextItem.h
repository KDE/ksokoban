/*
    SPDX-FileCopyrightText: 2023 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SIMPLETEXTITEM_H
#define SIMPLETEXTITEM_H

#include <QGraphicsSimpleTextItem>
#include <QGraphicsLayoutItem>

class SimpleTextItem : public QGraphicsLayoutItem, public QGraphicsSimpleTextItem
{
public:
    explicit SimpleTextItem(QGraphicsItem *parent = nullptr);
    explicit SimpleTextItem(const QString &text, QGraphicsItem *parent = nullptr);

public: // QGraphicsLayoutItem API
    void setGeometry(const QRectF &geom) override;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const override;
};

#endif
