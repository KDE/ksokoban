/*
 *  ksokoban - a Sokoban game for KDE
 *  Copyright (C) 1998  Anders Widell  <d95-awi@nada.kth.se>
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef MODALLABEL_H
#define MODALLABEL_H

#include <qlabel.h>
//Added by qt3to4:
#include <QTimerEvent>
#include <QEvent>

class ModalLabel : public QLabel {
  Q_OBJECT
public:
  static void message (const QString &text, QWidget *parent);

  void timerEvent (QTimerEvent *);
  bool eventFilter (QObject *, QEvent *);
  bool completed_;

protected:
  ModalLabel (const QString &text, QWidget *parent, const char *name=0, Qt::WFlags f=0);

};

#endif  /* MODALLABEL_H */
