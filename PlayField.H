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

#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include <qwidget.h>

#include "ImageData.H"
#include "LevelMap.H"
class MapDelta;
class MoveSequence;
class Move;
#include "PathFinder.H"

class History;

class PlayField : public QWidget {
  Q_OBJECT
public:
  PlayField (QWidget *parent=0, const char *name=0, WFlags f=0);
  // PlayField (QWidget *parent, const char *name, WFlags f);
  ~PlayField ();


  int resolution () { return resolution_; }
  void resolution (int res);
  int noOfSets () { return levelMap_->noOfSets (); }
  const char *setName (int i) { return levelMap_->setName (i); }

  void setSize ();
  void level (int _l) { levelMap_->level (_l); }
  int set () { return levelMap_->set (); }
  void levelChange ();
  void emitAll ();

public slots:
  void nextLevel ();
  void previousLevel ();
  void undo ();
  void redo ();
  void restartLevel ();
  void changeSet (int set);

signals:
  void levelChanged (const char *text);
  void collectionChanged (const char *text);
  void movesChanged (const char *text);
  void pushesChanged (const char *text);

protected:
  ImageData *imageData_;
  LevelMap  *levelMap_;
  History   *history_;
  int        lastLevel_;
  MoveSequence  *moveStep_;
  MapDelta  *mapDelta_;
  bool       moveInProgress_;
  PathFinder pathFinder_;

  void paintSquare (int x, int y);
  void paintDelta ();
  void paintEvent (QPaintEvent *);
  void keyPressEvent (QKeyEvent *);
  void focusInEvent (QFocusEvent *);
  void focusOutEvent (QFocusEvent *);
  void mousePressEvent (QMouseEvent *);
  void move (int _x, int _y);
  void push (int _x, int _y);
  virtual void timerEvent (QTimerEvent *);

private:
  int width_, height_, xOffs_, yOffs_, maxX_, maxY_, minX_, minY_;
  int resolution_;

  x2pixel (int x) { return width_*x+xOffs_; }
  y2pixel (int y) { return height_*y+yOffs_; }

  pixel2x (int x) { return (x-xOffs_)/width_; }
  pixel2y (int y) { return (y-yOffs_)/height_; }

  void drawImage (int x, int y, const QPixmap *image) {
    bitBlt (this, x2pixel (x), y2pixel (y),
	    image, 0, 0, imageData_->width (), imageData_->height (),
	    CopyROP, false);
  }
  void setOffset ();
  void startMoving (Move *m);
  void startMoving (MoveSequence *ms);
  void stopMoving ();

  void emitMoves (bool force);

};

#endif  /* PLAYFIELD_H */
