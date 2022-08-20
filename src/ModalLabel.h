/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MODALLABEL_H
#define MODALLABEL_H

#include <QLabel>

class ModalLabel : public QLabel {
  Q_OBJECT
public:
  static void message (const QString &text, QWidget *parent);

  void timerEvent (QTimerEvent *) override;
  bool eventFilter (QObject *, QEvent *) override;
  bool completed_;

protected:
  ModalLabel (const QString &text, QWidget *parent, Qt::WindowFlags f={});

};

#endif  /* MODALLABEL_H */
