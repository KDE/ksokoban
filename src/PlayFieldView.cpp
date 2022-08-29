/*
    SPDX-FileCopyrightText: 2022 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "PlayFieldView.h"

#include "PlayField.h"

#include <QResizeEvent>

PlayFieldView::PlayFieldView(PlayField *playField, QWidget *parent)
    : QGraphicsView(playField, parent)
    , m_playField(playField)
{
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
    setMouseTracking(true);

    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setFrameStyle(QFrame::NoFrame);
    setCacheMode(QGraphicsView::CacheBackground);

    setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing );
}

PlayFieldView::~PlayFieldView()
{
}

void PlayFieldView::resizeEvent(QResizeEvent *e)
{
    m_playField->setSize(e->size().width(), e->size().height());
}

#include "moc_PlayFieldView.cpp"
