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
#include <assert.h>

#include <qwidget.h>
#include <qpixmap.h>
#include <qkeycode.h>
#include <kconfig.h>
#include <kapp.h>
#include <klocale.h>
#include <qpainter.h>

#include "PlayField.H"
#include "ModalLabel.H"
#include "LevelMap.H"
#include "Move.H"
#include "History.H"
#include "PathFinder.H"
#include "MapDelta.H"
#include "MoveSequence.H"
#include "StaticImage.H"
#include "HtmlPrinter.H"
#include "Bookmark.H"
#include "LevelCollection.H"

#include "PlayField.moc"

PlayField::PlayField(QWidget *parent, const char *name, WFlags f)
  : QWidget(parent, name, f|WResizeNoErase), imageData_(0), lastLevel_(-1),
    moveSequence_(0), moveInProgress_(false), xOffs_(0), yOffs_(0),
    levelText_(i18n("Level")), stepsText_(i18n("Steps")),
    pushesText_(i18n("Pushes")) {

  setFocusPolicy(QWidget::StrongFocus);
  setFocus();
  setFont(QFont("helvetica", 18, QFont::Bold));
  setBackgroundColor(QColor(0x66,0x66,0x66));

  KConfig *cfg = (KApplication::getKApplication())->getConfig();
  cfg->setGroup("settings");

  imageData_ = new StaticImage;

  animDelay_ = cfg->readNumEntry("animDelay", 2);
  if (animDelay_ < 0 || animDelay_ > 3) animDelay_ = 2;

  history_ = new History;

  setBackgroundPixmap(imageData_->background());

  levelMap_  = new LevelMap;
  mapDelta_ = new MapDelta(levelMap_);
  mapDelta_->end();

  levelChange();
  repaint(false);
}

PlayField::~PlayField() {
  KConfig *cfg = (KApplication::getKApplication())->getConfig();
  cfg->setGroup("settings");
  cfg->writeEntry("animDelay", animDelay_, true, false, false);

  delete history_;
  delete levelMap_;
  delete imageData_;
}

int
PlayField::level() {
  if (levelMap_ == 0) return 0;
  return levelMap_->level();
}

const QString &
PlayField::collectionName() {
  static QString error = "????";
  if (levelMap_ == 0) return error;
  return levelMap_->collectionName();
}

int
PlayField::totalMoves() {
  if (levelMap_ == 0) return 0;
  return levelMap_->totalMoves();
}

int 
PlayField::totalPushes() {
  if (levelMap_ == 0) return 0;
  return levelMap_->totalPushes();
}

void
PlayField::levelChange() {
  stopMoving();
  history_->clear();
  setSize(width(), height());

  //changeLevelStatus();
  //changeStepsStatus();
  //changePushesStatus();
}

void
PlayField::paintSquare(int x, int y, QPainter &paint, bool erased) {
  if (levelMap_->xpos() == x && levelMap_->ypos() == y) {
    if (levelMap_->goal(x, y))
      imageData_->saveman(paint, x2pixel(x), y2pixel(y));
    else
      imageData_->man(paint, x2pixel(x), y2pixel(y));
    return;
  }
  if (levelMap_->empty(x, y)) {
    if (levelMap_->floor(x, y)) {
      if (levelMap_->goal(x, y))
	imageData_->goal(paint, x2pixel(x), y2pixel(y));
      else
	imageData_->floor(paint, x2pixel(x), y2pixel(y));
    } else {
      if (!erased) erase(x2pixel(x), y2pixel(y), width_, height_);
    }
    return;
  }
  if (levelMap_->wall(x, y)) {
    imageData_->wall(paint, x2pixel(x), y2pixel(y), x+y*(MAX_X+1),
		     levelMap_->wallLeft(x, y),
		     levelMap_->wallRight(x, y));
    return;
  }
  if (levelMap_->object(x, y)) {
    if (levelMap_->goal(x, y))
      imageData_->treasure(paint, x2pixel(x), y2pixel(y));
    else
      imageData_->object(paint, x2pixel(x), y2pixel(y));
    return;
  }
}

void
PlayField::paintDelta() {
  QPainter paint(this);
  for (int y=0; y<levelMap_->height(); y++) {
    for (int x=0; x<levelMap_->width(); x++) {
      if (mapDelta_->hasChanged(x, y)) paintSquare(x, y, paint, false);
    }
  }
}



void
PlayField::paintEvent(QPaintEvent *e) {
  QPainter paint(this);
#if QT_VERSION < 200
  paint.setClipRect(e->rect());
#else
  paint.setClipRegion(e->region());
#endif
  paint.setClipping(true);

  int minx = pixel2x(e->rect().x());
  int miny = pixel2y(e->rect().y());
  int maxx = pixel2x(e->rect().x()+e->rect().width()-1);
  int maxy = pixel2y(e->rect().y()+e->rect().height()-1);

  if (minx < 0) minx = 0;
  if (miny < 0) miny = 0;
  if (maxx >= levelMap_->width()) maxx = levelMap_->width()-1;
  if (maxy >= levelMap_->height()) maxy = levelMap_->height()-1;

  bool erased = e->erased();
  if (!erased) {
    int x1, x2, y1, y2;
    y1 = y2pixel(miny);
    if (y1 > e->rect().y()) erase(e->rect().x(), e->rect().y(), e->rect().width(), y1-e->rect().y());
    
    y2 = y2pixel(maxy+1);
    if (y2 < e->rect().y()+e->rect().height()) erase(e->rect().x(), y2, e->rect().width(), e->rect().y()+e->rect().height()-y2);
    
    x1 = x2pixel(minx);
    if (x1 > e->rect().x()) erase(e->rect().x(), y1, x1-e->rect().x(), y2-y1);

    x2 = x2pixel(maxx+1);
    if (x2 < e->rect().x()+e->rect().width()) erase(x2, y1, e->rect().x()+e->rect().width()-x2, y2-y1);

    // paint.eraseRect
  }



  for (int y=miny; y<=maxy; y++) {
    for (int x=minx; x<=maxx; x++) {
      paintSquare(x, y, paint, erased);
    }
  }

  paint.setPen(QColor(0,255,0));
  paint.drawText(collRect_, AlignLeft, collectionName());

  paint.setPen(QColor(128,128,128));
  paint.drawText(ltxtRect_, AlignLeft, levelText_);
  paint.drawText(stxtRect_, AlignLeft, stepsText_);
  paint.drawText(ptxtRect_, AlignLeft, pushesText_);

  QString str;
  paint.setPen(QColor(255,0,0));
  paint.drawText(lnumRect_, AlignLeft, str.sprintf("%05d", level()+1));
  paint.drawText(snumRect_, AlignLeft, str.sprintf("%05d", totalMoves()));
  paint.drawText(pnumRect_, AlignLeft, str.sprintf("%05d", totalPushes()));
}

void
PlayField::resizeEvent(QResizeEvent *e) {
  setSize(e->size().width(), e->size().height());
}

void
PlayField::stopMoving() {
  killTimers();
  delete moveSequence_;
  moveSequence_ = 0;
  moveInProgress_ = false;
  changeStepsStatus();
  changePushesStatus();
}


void
PlayField::startMoving(Move *m) {
  startMoving(new MoveSequence(m, levelMap_));
}

void
PlayField::startMoving(MoveSequence *ms) {
  static const int delay[4] = {0, 15, 35, 60};

  assert(moveSequence_ == 0 && !moveInProgress_);
  moveSequence_ = ms;
  moveInProgress_ = true;
  if (animDelay_) startTimer(delay[animDelay_]);
  timerEvent(0);
}

void
PlayField::timerEvent(QTimerEvent *) {
  assert(moveInProgress_);
  if (moveSequence_ == 0) {
    killTimers();
    moveInProgress_ = false;
    return;
  }

  bool more=false;

  mapDelta_->start();
  if (animDelay_) more = moveSequence_->next();
  else {
    while (moveSequence_->next()) if (levelMap_->completed()) break;
    more = true;   // FIXME: clean this up
    stopMoving();
  }
  mapDelta_->end();

  if (more) {
    paintDelta();
    if (levelMap_->completed()) {
      stopMoving();
      ModalLabel::message(i18n("Level completed"), this);
      nextLevel();
      return;
    }
  } else stopMoving();
}

void
PlayField::step(int _x, int _y) {
  if (!canMoveNow()) return;

  int oldX=levelMap_->xpos();
  int oldY=levelMap_->ypos();
  int x=oldX, y=oldY;
  
  int dx=0, dy=0;
  if (_x>oldX) dx=1;
  if (_x<oldX) dx=-1;
  if (_y>oldY) dy=1;
  if (_y<oldY) dy=-1;
  
  while (!(x==_x && y==_y) && levelMap_->step(x+dx, y+dy)) {
    x += dx;
    y += dy;
  }

  if (x!=oldX || y!=oldY) {
    Move *m = new Move(oldX, oldY);
    m->step(x, y);
    m->finish();
    history_->add(m);
    m->undo(levelMap_);

    startMoving(m);
    
  }
}

void
PlayField::push(int _x, int _y) {
  if (!canMoveNow()) return;

  int oldX=levelMap_->xpos();
  int oldY=levelMap_->ypos();
  int x=oldX, y=oldY;
  
  int dx=0, dy=0;
  if (_x>oldX) dx=1;
  if (_x<oldX) dx=-1;
  if (_y>oldY) dy=1;
  if (_y<oldY) dy=-1;
  
  while (!(x==_x && y==_y) && levelMap_->step(x+dx, y+dy)) {
    x += dx;
    y += dy;
  }
  int objX=x, objY=y;
  while (!(x==_x && y==_y) && levelMap_->push(x+dx, y+dy)) {
    x += dx;
    y += dy;
  }

  if (x!=oldX || y!=oldY) {
    Move *m = new Move(oldX, oldY);

    if (objX!=oldX || objY!=oldY) m->step(objX, objY);

    if (objX!=x || objY!=y) {
      m->push(x, y);
      
      objX += dx;
      objY += dy;
    }
    m->finish();
    history_->add(m);

    m->undo(levelMap_);

    startMoving(m);
  }
}

void
PlayField::keyPressEvent(QKeyEvent * e) {
  int x=levelMap_->xpos();
  int y=levelMap_->ypos();

  switch (e->key()) {
  case Key_Up:
    if (e->state() & ControlButton) step(x, 0);
    else if (e->state() & ShiftButton) push(x, 0);
    else push(x, y-1);
    break;
  case Key_Down:
    if (e->state() & ControlButton) step(x, MAX_Y);
    else if (e->state() & ShiftButton) push(x, MAX_Y);
    else push(x, y+1);
    break;
  case Key_Left:
    if (e->state() & ControlButton) step(0, y);
    else if (e->state() & ShiftButton) push(0, y);
    else push(x-1, y);
    break;
  case Key_Right:
    if (e->state() & ControlButton) step(MAX_X, y);
    else if (e->state() & ShiftButton) push(MAX_X, y);
    else push(x+1, y);
    break;

  case Key_Q:
    KApplication::getKApplication()->quit();
    break;

  case Key_Backspace:
    if (e->state() & ControlButton) redo();
    else undo();
    break;

#if 0
  case Key_X:
    levelMap_->random();
    levelChange();
    repaint(false);
    break;

  case Key_R:
    level(levelMap_->level());
    return;
    break;
  case Key_N:
    nextLevel();
    return;
    break;
  case Key_P:
    previousLevel();
    return;
    break;
  case Key_U:
    undo();
    return;
    break;
  case Key_I:
    history_->redo(levelMap_);
    repaint(false);
    return;
    break;

  case Key_S:
    {
      QString buf;
      history_->save(buf);
      printf("%s\n", (char *) buf);
    }
    return;
    break;

  case Key_L:
    stopMoving();
    history_->clear();
    level(levelMap_->level());
    {
      char buf[4096]="r1*D1*D1*r1*@r1*D1*";
      //scanf("%s", buf);
      history_->load(levelMap_, buf);
    }
    changeStepsStatus();
    changePushesStatus();
    repaint(false);
    return;
    break;
#endif


  case Key_Print:
    HtmlPrinter::printHtml(levelMap_);
    break;

  default:
    e->ignore();
    return;
    break;
  }
}

void
PlayField::mousePressEvent(QMouseEvent *e) {
  if (!canMoveNow()) return;

  Move *m;
  int x=pixel2x(e->x());
  int y=pixel2y(e->y());

  switch (e->button()) {
  case LeftButton:
    m = pathFinder_.search(levelMap_, x, y);
    if (m != 0) {
      history_->add(m);

      startMoving(m);
    }
    break;
  case MidButton:
    push(x, y);
    break;
  case RightButton:
    undo();
    return;
    break;

  default:
    return;
  }
}

void
PlayField::focusInEvent(QFocusEvent *) {
  //printf("PlayField::focusInEvent\n");
}

void
PlayField::focusOutEvent(QFocusEvent *) {
  //printf("PlayField::focusOutEvent\n");
}

void
PlayField::setSize(int w, int h) {
  int sbarHeight = fontMetrics().height();
  int sbarNumWidth = fontMetrics().boundingRect("88888").width()+8;
  int sbarLevelWidth = fontMetrics().boundingRect(levelText_).width()+8;
  int sbarStepsWidth = fontMetrics().boundingRect(stepsText_).width()+8;
  int sbarPushesWidth = fontMetrics().boundingRect(pushesText_).width()+8;

  pnumRect_.setRect(w-sbarNumWidth, h-sbarHeight, sbarNumWidth, sbarHeight);
  ptxtRect_.setRect(pnumRect_.x()-sbarPushesWidth, h-sbarHeight, sbarPushesWidth, sbarHeight);
  snumRect_.setRect(ptxtRect_.x()-sbarNumWidth, h-sbarHeight, sbarNumWidth, sbarHeight);
  stxtRect_.setRect(snumRect_.x()-sbarStepsWidth, h-sbarHeight, sbarStepsWidth, sbarHeight);
  lnumRect_.setRect(stxtRect_.x()-sbarNumWidth, h-sbarHeight, sbarNumWidth, sbarHeight);
  ltxtRect_.setRect(lnumRect_.x()-sbarLevelWidth, h-sbarHeight, sbarLevelWidth, sbarHeight);
  collRect_.setRect(0, h-sbarHeight, ltxtRect_.x(), sbarHeight);

  h -= sbarHeight;

  int cols = levelMap_->width();
  int rows = levelMap_->height();

  // FIXME: the line below should not be needed
  if (cols == 0 || rows == 0) return;

  width_ = w / cols;
  height_ = h / rows;
  if (width_ < 8) width_ = 8;
  if (height_ < 8) height_ = 8;


  if (width_ > height_) width_ = height_;
  if (height_ > width_) height_ = width_;

  width_ = height_ = imageData_->resize(width_);

  xOffs_ = (w - cols*width_) / 2;
  yOffs_ = (h - rows*height_) / 2;
}

void
PlayField::nextLevel() {
  if (levelMap_->level()+1 >= levelMap_->noOfLevels()) {
    ModalLabel::message(i18n("\
This is the last level in\n\
the current collection."), this);
    return;
  }
  if (levelMap_->level() >= levelMap_->completedLevels()) {
    ModalLabel::message(i18n("\
You have not completed\n\
this level yet."), this);
    return;
  }

  level(levelMap_->level()+1);
  levelChange();
  repaint(false);
}

void
PlayField::previousLevel() {
  if (levelMap_->level() <= 0) {
    ModalLabel::message(i18n("\
This is the first level in\n\
the current collection."), this);
    return;
  }
  level(levelMap_->level()-1);
  levelChange();
  repaint(false);
}

void
PlayField::undo() {
  if (!canMoveNow()) return;

  startMoving(history_->deferUndo(levelMap_));
}

void
PlayField::redo() {
  if (!canMoveNow()) return;

  startMoving(history_->deferRedo(levelMap_));
}

void
PlayField::restartLevel() {
  stopMoving();
  history_->clear();
  level(levelMap_->level());
  changeStepsStatus();
  changePushesStatus();
  repaint(false);
}

void
PlayField::changeCollection(LevelCollection *collection) {
  if (levelMap_->collection() == collection) return;
  levelMap_->changeCollection(collection);
  levelChange();
  //erase(collRect_);
  repaint(false);
}

void
PlayField::changeLevelStatus() {
  erase(lnumRect_);

  QPainter paint(this);
  
  QString str;
  paint.setPen(QColor(255,0,0));
  paint.drawText(lnumRect_, AlignLeft, str.sprintf("%05d", level()+1));
}

void
PlayField::changeStepsStatus() {
  erase(snumRect_);

  QPainter paint(this);
  
  QString str;
  paint.setPen(QColor(255,0,0));
  paint.drawText(snumRect_, AlignLeft, str.sprintf("%05d", totalMoves()));
}

void
PlayField::changePushesStatus() {
  erase(pnumRect_);

  QPainter paint(this);
  
  QString str;
  paint.setPen(QColor(255,0,0));
  paint.drawText(pnumRect_, AlignLeft, str.sprintf("%05d", totalPushes()));
}


void
PlayField::changeAnim(int num)
{
  assert(num >= 0 && num <= 3);

  animDelay_ = num;
}

// FIXME: clean up bookmark stuff

static const int bookmark_id[] = {
  0, 1, 8, 2, 9, 3, 5, 6, 7, 4
};

void
PlayField::setBookmark(Bookmark *bm) {
  if (!levelMap_->goodLevel()) return;

  if (collection()->id() < 0) {
    ModalLabel::message(i18n("\
Sorry. Bookmarks for external levels\n\
is not implemented yet"), this);
    return;
  }

  bm->set(bookmark_id[collection()->id()], levelMap_->level(), levelMap_->totalMoves(), history_);
}

void
PlayField::goToBookmark(Bookmark *bm) {
  level(bm->level());
  levelChange();
  if (!bm->goTo(levelMap_, history_)) fprintf(stderr, "Warning: bad bookmark\n");
  //changeLevelStatus();
  //changeStepsStatus();
  //changePushesStatus();
  repaint(false);
}

bool
PlayField::canMoveNow() {
  if (moveInProgress_) return false;
  if (!levelMap_->goodLevel()) {
    ModalLabel::message(i18n("This level is broken"), this);
    return false;
  }
  return true;
}
