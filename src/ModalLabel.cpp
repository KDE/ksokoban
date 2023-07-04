/*
    SPDX-FileCopyrightText: 1998 Anders Widell <awl@hem.passagen.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "ModalLabel.h"

#include <QApplication>
#include <QFont>
#include <QFontDatabase>
#include <QLabel>
#include <QString>
#include <QWidgetList>

ModalLabel::ModalLabel(const QString &text, QWidget *parent)
    : QLabel(text, parent)
{
    QFont font(QFontDatabase::systemFont(QFontDatabase::GeneralFont).family(), 24, QFont::Bold);
    QFontMetrics fontMet(font);
    setAutoFillBackground(true);
    QString currentLine;
    QRect bounds;
    int lineLen, width = 0, height = 0;

    for (int linePos = 0; linePos < text.length(); linePos += lineLen + 1) {
        lineLen = text.indexOf(QLatin1Char('\n'), linePos);
        if (lineLen < 0)
            lineLen = text.length() - linePos;
        else
            lineLen -= linePos;

        currentLine = text.mid(linePos, lineLen);
        bounds = fontMet.boundingRect(currentLine);

        if (bounds.width() > width)
            width = bounds.width();
        height += bounds.height();
    }

    width += 32;
    height += 32;

    if (width < 300)
        width = 300;
    if (height < 75)
        height = 75;

    setAlignment(Qt::AlignCenter);
    setFrameStyle(QFrame::Panel | QFrame::Raised);
    setLineWidth(4);
    setFont(font);
    move(parent->width() / 2 - width / 2, parent->height() / 2 - height / 2);
    resize(width, height);
    show();

    const QWidgetList widgets = QApplication::allWidgets();
    for (QWidget *widget : widgets) {
        widget->installEventFilter(this);
    }

    startTimer(1000);
}

void ModalLabel::timerEvent(QTimerEvent *)
{
    completed_ = true;
}

bool ModalLabel::eventFilter(QObject *, QEvent *e)
{
    switch (e->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        // case QEvent::Accel:
        // case QEvent::DragEnter:
    case QEvent::DragMove:
    case QEvent::DragLeave:
    case QEvent::Drop:
        // case QEvent::DragResponse:

        // kdDebug << "Ate event" << endl;
        return true;
    default:
        return false;
    }
}

void ModalLabel::message(const QString &text, QWidget *parent)
{
    QApplication *app = qApp;
    ModalLabel cl(text, parent);

    while (!cl.completed_)
        app->processEvents();
}

#include "moc_ModalLabel.cpp"
