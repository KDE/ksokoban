/*
 *  ksokoban - a Sokoban game by KDE
 *  Copyright (C) 1998  Anders Widell  <awl@hem.passagen.se>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <QLabel>
#include <QFont>
#include <QApplication>
#include <QWidgetList>
#include <QString>
#include <QFontDatabase>

#include "ModalLabel.h"

ModalLabel::ModalLabel(const QString &text, QWidget *parent, Qt::WindowFlags f)
  : QLabel(text, parent, f) {
  QFont font(QFontDatabase::systemFont(QFontDatabase::GeneralFont).family(), 24, QFont::Bold);
  QFontMetrics fontMet(font);
  setAutoFillBackground(true);
  QString currentLine;
  QRect bounds;
  int lineLen, width=0, height=0;

  for (int linePos=0; linePos < text.length(); linePos += lineLen+1) {

    lineLen = text.indexOf('\n', linePos);
    if (lineLen < 0) lineLen = text.length() - linePos;
    else lineLen -= linePos;

    currentLine = text.mid(linePos, lineLen);
    bounds = fontMet.boundingRect(currentLine);

    if (bounds.width() > width) width = bounds.width();
    height += bounds.height();
  }

  width += 32;
  height += 32;

  if (width < 300) width = 300;
  if (height < 75) height = 75;

  setAlignment (Qt::AlignCenter);
  setFrameStyle (QFrame::Panel | QFrame::Raised);
  setLineWidth (4);
  setFont (font);
  move (parent->width ()/2 - width/2, parent->height ()/2 - height/2);
  resize (width, height);
  show ();

  QWidgetList list = QApplication::allWidgets();
  for(QWidgetList::Iterator it=list.begin(); it!=list.end(); it++) {
	(*it)->installEventFilter (this);
  }
  
  completed_ = false;
  startTimer (1000);
}

void
ModalLabel::timerEvent (QTimerEvent *) {
  completed_ = true;
}

bool
ModalLabel::eventFilter (QObject *, QEvent *e) {
  switch (e->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
      //case QEvent::Accel:
      //case QEvent::DragEnter:
    case QEvent::DragMove:
    case QEvent::DragLeave:
    case QEvent::Drop:
      //case QEvent::DragResponse:

      //kdDebug << "Ate event" << endl;
    return true;
  default:
    return false;
  }
}

void
ModalLabel::message (const QString &text, QWidget *parent) {
  QApplication *app = qApp;
  ModalLabel cl (text, parent);

  while (!cl.completed_) app->processEvents ();
}
