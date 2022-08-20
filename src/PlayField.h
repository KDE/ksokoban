/*
 *  ksokoban - a Sokoban game by KDE
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include <QWidget>
#include <QString>
#include <QFont>
#include <QFontMetrics>
#include <QPixmap>
#include <QImage>
#include <QBrush>

#include "ImageData.h"
#include "LevelMap.h"
class MapDelta;
class MoveSequence;
class Move;
#include "PathFinder.h"

class History;
class Bookmark;
class LevelCollection;
class QPainter;
class QCursor;

class PlayField : public QWidget {
  Q_OBJECT
public:
  PlayField(QWidget *parent);
  ~PlayField () override;

  bool canMoveNow();
  int animDelay() { return animDelay_; }

  void setSize(int w, int h);
  void level(int _l) { levelMap_->level(_l); }
  LevelCollection  *collection() const { return levelMap_->collection(); }
  void setBookmark(Bookmark *bm);
  void goToBookmark(Bookmark *bm);

  int level() const;
  const QString &collectionName();
  int totalMoves() const;
  int totalPushes() const;

  void updateCollectionXpm();
  void updateTextXpm();
  void updateLevelXpm();
  void updateStepsXpm();
  void updatePushesXpm();

public slots:
  void nextLevel();
  void previousLevel();
  void undo();
  void redo();
  void restartLevel();
  void changeCollection(LevelCollection *collection);
  void changeAnim(int num);

protected:
  ImageData *imageData_;
  LevelMap  *levelMap_;
  History   *history_;
  int        lastLevel_;
  MoveSequence  *moveSequence_;
  MapDelta  *mapDelta_;
  bool       moveInProgress_;
  bool       dragInProgress_;
  PathFinder pathFinder_;
  int        animDelay_;
  const QCursor* cursor_;

  void levelChange ();
  void paintSquare (int x, int y, QPainter &paint);
  void paintDelta ();
  void paintEvent (QPaintEvent *e) override;
  void paintPainterClip(QPainter& paint, int x, int y, int w, int h);
  void paintPainter(QPainter& paint, const QRect& rect);
  void resizeEvent (QResizeEvent *e) override;
  void mouseMoveEvent(QMouseEvent* e) override;
  void keyPressEvent (QKeyEvent *) override;
  void focusInEvent (QFocusEvent *) override;
  void focusOutEvent (QFocusEvent *) override;
  void mousePressEvent (QMouseEvent *) override;
  void mouseReleaseEvent(QMouseEvent*) override;
  void leaveEvent(QEvent*) override;
  void wheelEvent (QWheelEvent *) override;
  void step (int _x, int _y);
  void push (int _x, int _y);
  void timerEvent (QTimerEvent *) override;
  void stopDrag();
  void dragObject(int xpixel, int ypixel);
  void highlight();
  void changeCursor(const QCursor* c);

private:
  int size_, xOffs_, yOffs_;
  int highlightX_, highlightY_;
  int dragX_, dragY_;
  int lastMouseXPos_, lastMouseYPos_;
  int mousePosX_, mousePosY_;
  int wheelDelta_;
  int debug_counter;
  
  QList<int> timers;
  void killTimers();
  QCursor sizeAllCursor;
  QCursor crossCursor; 
  int x2pixel (int x) const { return size_*x+xOffs_; }
  int y2pixel (int y) const { return size_*y+yOffs_; }

  int pixel2x (int x) const { return (x-xOffs_)/size_; }
  int pixel2y (int y) const { return (y-yOffs_)/size_; }

  void startMoving (Move *m);
  void startMoving (MoveSequence *ms);
  void stopMoving ();

  QRect pnumRect_, ptxtRect_, snumRect_, stxtRect_, lnumRect_, ltxtRect_;
  QRect collRect_;

  const QString levelText_, stepsText_, pushesText_;
  QPixmap *pnumXpm_, *ptxtXpm_, *snumXpm_, *stxtXpm_, *lnumXpm_, *ltxtXpm_;
  QPixmap *collXpm_;
  QPixmap dragXpm_;
  QImage  dragImage_;
  QFont         statusFont_;
  QFontMetrics  statusMetrics_;
  QBrush  background_;
  QBrush  floor_;

};

#endif  /* PLAYFIELD_H */
