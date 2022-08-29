/*
    SPDX-FileCopyrightText: 2022 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PLAYFIELDVIEW_H
#define PLAYFIELDVIEW_H

#include <QGraphicsView>

class PlayField;

class PlayFieldView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit PlayFieldView(PlayField *playField, QWidget *parent);
    ~PlayFieldView() override;

protected:
    void resizeEvent(QResizeEvent *e) override;

private:
    PlayField *const m_playField;
};

#endif
