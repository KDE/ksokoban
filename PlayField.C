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

#include <stdio.h>
#include <unistd.h>

#include <qwidget.h>
#include <qpixmap.h>
#include <qkeycode.h>
#include <assert.h>
#include <kconfig.h>
#include <kapp.h>

#include "PlayField.H"
#include "ModalLabel.H"
#include "LowRes.H"
#include "MedRes.H"
#include "HiRes.H"
#include "LevelMap.H"
#include "Move.H"
#include "History.H"
#include "PathFinder.H"
#include "MapDelta.H"
#include "MoveSequence.H"

#include "HtmlPrinter.H"
#include "Bookmark.H"

#include "PlayField.moc.C"

PlayField::PlayField (QWidget *parent, const char *name, WFlags f)
  : QWidget (parent, name, f) {
  xOffs_ = yOffs_ = 0;
  lastLevel_ = -1;

  KConfig *cfg = (KApplication::getKApplication ())->getConfig ();
  resolution_ = cfg->readNumEntry ("resolution", 1);
  switch (resolution_) {
  case 0:
    imageData_ = new LowRes;
    break;
  case 2:
    imageData_ = new HiRes;
    break;
  default:
    resolution_ = 1;
    imageData_ = new MedRes;
    break;
  }

  animDelay_ = cfg->readNumEntry ("animDelay", 2);
  if (animDelay_ < 0 || animDelay_ > 3) animDelay_ = 2;

  width_  = imageData_->width ();
  height_ = imageData_->height ();

  history_   = new History;

  setFocus ();

  setBackgroundPixmap (*(imageData_->background ()));

  levelMap_  = new LevelMap;
  mapDelta_ = new MapDelta (levelMap_);
  mapDelta_->end ();
  moveInProgress_ = false;
  moveSequence_ = 0;

  levelChange ();
  repaint (false);
}

PlayField::~PlayField () {
  KConfig *cfg = (KApplication::getKApplication ())->getConfig ();
  cfg->writeEntry ("animDelay", animDelay_, true, false, false);
  cfg->writeEntry ("resolution", resolution_, true, false, false);

  delete history_;
  delete levelMap_;
  delete imageData_;
}

void
PlayField::emitAll () {
  static char levelText[20];

  emit collectionChanged (levelMap_->collectionName (collection ()));
  QString str = i18n("Level:");
  sprintf (levelText, (const char *) (str + " %d"), levelMap_->level ()+1);
  emit levelChanged (levelText);
  emitMoves (true);
}

void
PlayField::emitMoves (bool force) {
  static int lastMoves=-1, lastPushes=-1;
  int moves=levelMap_->totalMoves ();
  int pushes=levelMap_->totalPushes ();

  if (lastMoves != moves || force) {
    static char moveBuf[20];
    QString str = i18n("Moves:");
    sprintf (moveBuf, (const char *) (str +" %d"), moves);
    emit movesChanged (moveBuf);
    lastMoves = moves;
  }

  if (lastPushes != pushes || force) {
    static char pushBuf[20];
    QString str = i18n("Pushes:");
    sprintf (pushBuf, (const char *) (str + " %d"), pushes);
    emit pushesChanged (pushBuf);
    lastPushes = pushes;
  }
}

void
PlayField::levelChange () {
  stopMoving ();
  history_->clear ();
  maxX_ = levelMap_->maxX ();
  maxY_ = levelMap_->maxY ();
  minX_ = levelMap_->minX ();
  minY_ = levelMap_->minY ();
  setOffset ();

  if (x2pixel (minX_) > 0)
    erase (0, y2pixel (minY_), x2pixel (minX_), (maxY_+1-minY_)*height_);
  if (y2pixel (minY_) > 0)
    erase (0, 0, (MAX_X+1)*width_, y2pixel (minY_));

  if (x2pixel (maxX_+1) < (MAX_X+1)*width_)
    erase (x2pixel (maxX_+1), y2pixel (minY_),
	   (MAX_X+1)*width_ - x2pixel (maxX_+1), (maxY_+1-minY_)*height_);
  if (y2pixel (maxY_+1) < (MAX_Y+1)*height_)
    erase (0, y2pixel (maxY_+1), (MAX_X+1)*width_, (MAX_Y+1)*height_ - y2pixel (maxY_+1));

  emitAll ();
}

void
PlayField::paintSquare (int x, int y) {
  if (levelMap_->xpos () == x && levelMap_->ypos () == y) {
    drawImage (x, y, levelMap_->goal (x, y) ?
	       imageData_->saveman () :
	       imageData_->man ());
    return;
  }
  if (levelMap_->empty (x, y)) {
    if (levelMap_->floor (x, y)) {
      drawImage (x, y, levelMap_->goal (x, y) ?
		 imageData_->goal () :
		 imageData_->floor ());
    } else {
      erase (x2pixel (x), y2pixel (y), 
	     width_, height_);
    }
    return;
  }
  if (levelMap_->wall (x, y)) {
    drawImage (x, y,
	       imageData_->wall (levelMap_->wallUp (x, y),
				 levelMap_->wallDown (x, y),
				 levelMap_->wallLeft (x, y),
				 levelMap_->wallRight (x, y)));
    return;
  }
  if (levelMap_->object (x, y)) {
    drawImage (x, y, levelMap_->goal (x, y) ?
	       imageData_->treasure () :
	       imageData_->object ());
    return;
  }
}

void
PlayField::paintDelta () {
  for (int y=levelMap_->minY(); y<=levelMap_->maxY(); y++) {
    for (int x=levelMap_->minX(); x<=levelMap_->maxX(); x++) {
      if (mapDelta_->hasChanged (x, y)) paintSquare (x, y);
    }
  }
}



void
PlayField::paintEvent (QPaintEvent *) {
  for (int y=levelMap_->minY(); y<=levelMap_->maxY(); y++) {
    for (int x=levelMap_->minX(); x<=levelMap_->maxX(); x++) {
      paintSquare (x, y);
    }
  }
}

void
PlayField::stopMoving () {
  killTimers ();
  delete moveSequence_;
  moveSequence_ = 0;
  moveInProgress_ = false;
  emitMoves (false);
}


void
PlayField::startMoving (Move *m) {
  startMoving (new MoveSequence (m, levelMap_));
}

void
PlayField::startMoving (MoveSequence *ms) {
  static const int delay[4] = {0, 15, 35, 60};

  assert (moveSequence_ == 0 && !moveInProgress_);
  moveSequence_ = ms;
  moveInProgress_ = true;
  if (animDelay_) startTimer (delay[animDelay_]);
  timerEvent (0);
}

void
PlayField::timerEvent (QTimerEvent *) {
  assert (moveInProgress_);
  if (moveSequence_ == 0) {
    killTimers ();
    moveInProgress_ = false;
    return;
  }

  bool more=false;

  mapDelta_->start ();
  if (animDelay_) more = moveSequence_->next ();
  else {
    while (moveSequence_->next ()) if (levelMap_->completed ()) break;
    more = true;   // FIXME: clean this up
    stopMoving ();
  }
  mapDelta_->end ();

  if (more) {
    paintDelta ();
    if (levelMap_->completed ()) {
      stopMoving ();
      ModalLabel::message (i18n("Level completed"), this);
      nextLevel ();
      return;
    }
  } else stopMoving ();
}

void
PlayField::setOffset () {
  xOffs_ = (MAX_X-maxX_-minX_)*width_/2;
  yOffs_ = (MAX_Y-maxY_-minY_)*height_/2;
}

void
PlayField::step (int _x, int _y) {
  if (moveInProgress_) return;
  int oldX=levelMap_->xpos ();
  int oldY=levelMap_->ypos ();
  int x=oldX, y=oldY;
  
  int dx=0, dy=0;
  if (_x>oldX) dx=1;
  if (_x<oldX) dx=-1;
  if (_y>oldY) dy=1;
  if (_y<oldY) dy=-1;
  
  while (!(x==_x && y==_y) && levelMap_->step (x+dx, y+dy)) {
    x += dx;
    y += dy;
  }

  if (x!=oldX || y!=oldY) {
    Move *m = new Move (oldX, oldY);
    m->step (x, y);
    m->finish ();
    history_->add (m);
    m->undo (levelMap_);

    startMoving (m);
    
  }
}

void
PlayField::push (int _x, int _y) {
  if (moveInProgress_) return;
  int oldX=levelMap_->xpos ();
  int oldY=levelMap_->ypos ();
  int x=oldX, y=oldY;
  
  int dx=0, dy=0;
  if (_x>oldX) dx=1;
  if (_x<oldX) dx=-1;
  if (_y>oldY) dy=1;
  if (_y<oldY) dy=-1;
  
  while (!(x==_x && y==_y) && levelMap_->step (x+dx, y+dy)) {
    x += dx;
    y += dy;
  }
  int objX=x, objY=y;
  while (!(x==_x && y==_y) && levelMap_->push (x+dx, y+dy)) {
    x += dx;
    y += dy;
  }

  if (x!=oldX || y!=oldY) {
    Move *m = new Move (oldX, oldY);

    if (objX!=oldX || objY!=oldY) m->step (objX, objY);

    if (objX!=x || objY!=y) {
      m->push (x, y);
      
      objX += dx;
      objY += dy;
    }
    m->finish ();
    history_->add (m);

    m->undo (levelMap_);

    startMoving (m);
  }
}

void
PlayField::keyPressEvent (QKeyEvent * e) {
  int x=levelMap_->xpos ();
  int y=levelMap_->ypos ();

  switch (e->key ()) {
  case Key_Up:
    if (e->state () & ControlButton) step (x, 0);
    else if (e->state () & ShiftButton) push (x, 0);
    else push (x, y-1);
    break;
  case Key_Down:
    if (e->state () & ControlButton) step (x, MAX_Y);
    else if (e->state () & ShiftButton) push (x, MAX_Y);
    else push (x, y+1);
    break;
  case Key_Left:
    if (e->state () & ControlButton) step (0, y);
    else if (e->state () & ShiftButton) push (0, y);
    else push (x-1, y);
    break;
  case Key_Right:
    if (e->state () & ControlButton) step (MAX_X, y);
    else if (e->state () & ShiftButton) push (MAX_X, y);
    else push (x+1, y);
    break;

  case Key_Q:
    KApplication::getKApplication ()->quit ();
    break;

#if 0
  case Key_X:
    levelMap_->random ();
    levelChange ();
    repaint (false);
    break;

  case Key_R:
    level (levelMap_->level ());
    return;
    break;
  case Key_N:
    nextLevel ();
    return;
    break;
  case Key_P:
    previousLevel ();
    return;
    break;
  case Key_U:
    undo ();
    return;
    break;
  case Key_I:
    history_->redo (levelMap_);
    repaint (false);
    return;
    break;

  case Key_S:
    {
      QString buf;
      history_->save (buf);
      printf ("%s\n", (char *) buf);
    }
    return;
    break;

  case Key_L:
    stopMoving ();
    history_->clear ();
    level (levelMap_->level ());
    {
      char buf[4096]="r1*D1*D1*r1*@r1*D1*";
      //scanf ("%s", buf);
      history_->load (levelMap_, buf);
    }
    emitMoves (true);
    repaint (false);
    return;
    break;
#endif


  case Key_Print:
    HtmlPrinter::printHtml (levelMap_);
    break;

  default:
    e->ignore ();
    return;
    break;
  }
}

void
PlayField::mousePressEvent (QMouseEvent *e) {
  if (moveInProgress_) return;
  Move *m;
  int x=pixel2x (e->x ());
  int y=pixel2y (e->y ());

  switch (e->button ()) {
  case LeftButton:
    m = pathFinder_.search (levelMap_, x, y);
    if (m != 0) {
      history_->add (m);

      startMoving (m);
    }
    break;
  case MidButton:
    push (x, y);
    break;
  case RightButton:
    undo ();
    return;
    break;

  default:
    return;
  }
}

void
PlayField::focusInEvent (QFocusEvent *) {
  //printf ("PlayField::focusInEvent\n");
}

void
PlayField::focusOutEvent (QFocusEvent *) {
  //printf ("PlayField::focusOutEvent\n");
}

void
PlayField::setSize () {
  setFixedSize (width_*(MAX_X+1), height_*(MAX_Y+1));
}

void
PlayField::nextLevel () {
  if (levelMap_->level ()+1 >= levelMap_->noOfLevels ()) {
    ModalLabel::message (i18n("\
This is the last level in\n\
the current collection."), this);
    return;
  }
  if (levelMap_->level () >= levelMap_->completedLevels ()) {
    ModalLabel::message (i18n("\
You have not completed\n\
this level yet."), this);
    return;
  }

  level (levelMap_->level ()+1);
  levelChange ();
  repaint (false);
}

void
PlayField::previousLevel () {
  if (levelMap_->level () <= 0) {
    ModalLabel::message (i18n("\
This is the first level in\n\
the current collection."), this);
    return;
  }
  level (levelMap_->level ()-1);
  levelChange ();
  repaint (false);
}

void
PlayField::undo () {
  if (moveInProgress_) return;
  startMoving (history_->deferUndo (levelMap_));
}

void
PlayField::redo () {
  if (moveInProgress_) return;
  startMoving (history_->deferRedo (levelMap_));
}

void
PlayField::restartLevel () {
  stopMoving ();
  history_->clear ();
  level (levelMap_->level ());
  emitMoves (true);
  repaint (false);
}

void
PlayField::resolution (int res) {
  assert (res >= 0 && res <= 2);
  if (resolution_ == res) return;

  resolution_ = res;
  delete imageData_;
  switch (resolution_) {
  case 0:
    imageData_ = new LowRes;
    break;
  case 2:
    imageData_ = new HiRes;
    break;
  default:
    resolution_ = 1;
    imageData_ = new MedRes;
    break;
  }
  width_  = imageData_->width ();
  height_ = imageData_->height ();
  setOffset ();
  setSize ();
}

void
PlayField::changeCollection (int collection)
{
  if (levelMap_->collection () == collection) return;
  levelMap_->changeCollection (collection);
  levelChange ();
  repaint (false);
}

void
PlayField::changeAnim (int num)
{
  assert (num >= 0 && num <= 3);

  animDelay_ = num;
}

void
PlayField::setBookmark (Bookmark *bm) {
  bm->set (collection (), levelMap_->level (), levelMap_->totalMoves (), history_);
}

void
PlayField::goToBookmark (Bookmark *bm) {
  levelMap_->changeCollection (bm->collection ());
  level (bm->level ());
  levelChange ();
  if (!bm->goTo (levelMap_, history_)) fprintf (stderr, "Warning: bad bookmark\n");
  emitAll ();
  repaint (false);
}

