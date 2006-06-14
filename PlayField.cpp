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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <QWidget>
#include <QWheelEvent>
#include <QFocusEvent>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QTimerEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <kconfig.h>
#include <kapplication.h>
#include <klocale.h>
#include <qpainter.h>
#include <kmessagebox.h>
#include <kglobalsettings.h>
#include <QAbstractEventDispatcher>

#include "PlayField.h"
#include "ModalLabel.h"
#include "LevelMap.h"
#include "Move.h"
#include "History.h"
#include "PathFinder.h"
#include "MapDelta.h"
#include "MoveSequence.h"
#include "StaticImage.h"
#include "HtmlPrinter.h"
#include "Bookmark.h"
#include "LevelCollection.h"

#include "PlayField.moc"

PlayField::PlayField(QWidget *parent, Qt::WFlags f)
  : QWidget(parent, f|Qt::WResizeNoErase), imageData_(0), lastLevel_(-1),
    moveSequence_(0), moveInProgress_(false), dragInProgress_(false),
    xOffs_(0), yOffs_(0),
    wheelDelta_(0),
    levelText_(i18n("Level:")), stepsText_(i18n("Steps:")),
    pushesText_(i18n("Pushes:")),
    statusFont_(KGlobalSettings::generalFont().family(), 18, QFont::Bold), statusMetrics_(statusFont_) {

  setFocusPolicy(Qt::StrongFocus);
  setFocus();
  setMouseTracking(true);

  highlightX_ = highlightY_ = 0;

  KConfigGroup settings(KGlobal::config(), "settings");

  imageData_ = new StaticImage;

  animDelay_ = settings.readEntry("animDelay", 2);
  if (animDelay_ < 0 || animDelay_ > 3) animDelay_ = 2;

  history_ = new History;

  background_.setTexture( imageData_->background() );
  floor_ = QColor(0x66,0x66,0x66);

  levelMap_  = new LevelMap;
  mapDelta_ = new MapDelta(levelMap_);
  mapDelta_->end();

  levelChange();
}

PlayField::~PlayField() {
  KConfigGroup settings(KGlobal::config(), "settings");
  settings.writeEntry("animDelay", animDelay_, KConfigBase::Normal);

  delete mapDelta_;
  delete history_;
  delete levelMap_;
  delete imageData_;
}

void
PlayField::changeCursor(const QCursor& c) {
  if (cursor_.handle() == c.handle()) return;

  cursor_ = c;
  QWidget::setCursor(c);
}

void PlayField::unsetCursor() {
  cursor_ = Qt::BlankCursor; // just so
  QWidget::unsetCursor();
}

int
PlayField::level() const {
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
PlayField::totalMoves() const {
  if (levelMap_ == 0) return 0;
  return levelMap_->totalMoves();
}

int
PlayField::totalPushes() const{
  if (levelMap_ == 0) return 0;
  return levelMap_->totalPushes();
}

void
PlayField::levelChange() {
  stopMoving();
  stopDrag();
  history_->clear();
  setSize(width(), height());

  updateLevelXpm();
  updateStepsXpm();
  updatePushesXpm();
  highlight();
}

void
PlayField::paintSquare(int x, int y, QPainter &paint) {
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
	paint.fillRect(x2pixel(x), y2pixel(y), size_, size_, floor_);
    } else {
      paint.fillRect(x2pixel(x), y2pixel(y), size_, size_, background_);
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
    if (highlightX_ == x && highlightY_ == y) {
      if (levelMap_->goal(x, y))
	imageData_->brightTreasure(paint, x2pixel(x), y2pixel(y));
      else
	imageData_->brightObject(paint, x2pixel(x), y2pixel(y));
    } else {
      if (levelMap_->goal(x, y))
	imageData_->treasure(paint, x2pixel(x), y2pixel(y));
      else
	imageData_->object(paint, x2pixel(x), y2pixel(y));
    }
    return;
  }
}

void
PlayField::paintDelta() {

  for (int y=0; y<levelMap_->height(); y++) {
    for (int x=0; x<levelMap_->width(); x++) {
      if (mapDelta_->hasChanged(x, y)) 
          update(x2pixel(x), y2pixel(y), size_, size_);
    }
  }
}



void
PlayField::paintEvent(QPaintEvent *e) {
  QPainter paint(this);

  // the following line is a workaround for a bug in Qt 2.0.1
  // (and possibly earlier versions)
  paint.setBrushOrigin(0, 0);

  paint.setClipRegion(e->region());
  paint.setClipping(true);

  paintPainter(paint, e->rect());
}

void
PlayField::paintPainterClip(QPainter &paint, int x, int y, int w, int h) {
  QRect rect(x, y, w, h);

  paint.setClipRect(rect);
  paint.setClipping(true);
  paintPainter(paint, rect);
}

void
PlayField::paintPainter(QPainter &paint, const QRect &rect) {
  if (size_ <= 0) return;
  int minx = pixel2x(rect.x());
  int miny = pixel2y(rect.y());
  int maxx = pixel2x(rect.x()+rect.width()-1);
  int maxy = pixel2y(rect.y()+rect.height()-1);

  if (minx < 0) minx = 0;
  if (miny < 0) miny = 0;
  if (maxx >= levelMap_->width()) maxx = levelMap_->width()-1;
  if (maxy >= levelMap_->height()) maxy = levelMap_->height()-1;

  {
    int x1, x2, y1, y2;
    y1 = y2pixel(miny);
    if (y1 > rect.y()) paint.fillRect(rect.x(), rect.y(), rect.width(), y1-rect.y(), background_);

    int bot=rect.y()+rect.height();
    if (bot > height()-collRect_.height()) bot = height()-collRect_.height();

    y2 = y2pixel(maxy+1);
    if (y2 < bot) paint.fillRect(rect.x(), y2, rect.width(), bot-y2, background_);

    x1 = x2pixel(minx);
    if (x1 > rect.x()) paint.fillRect(rect.x(), y1, x1-rect.x(), y2-y1, background_);

    x2 = x2pixel(maxx+1);
    if (x2 < rect.x()+rect.width()) paint.fillRect(x2, y1, rect.x()+rect.width()-x2, y2-y1, background_);

    // paint.eraseRect
  }

  for (int y=miny; y<=maxy; y++) {
    for (int x=minx; x<=maxx; x++) {
      paintSquare(x, y, paint);
    }
  }

  if (collRect_.intersects(rect)) paint.drawPixmap(collRect_.x(), collRect_.y(), collXpm_);
  if (ltxtRect_.intersects(rect)) paint.drawPixmap(ltxtRect_.x(), ltxtRect_.y(), ltxtXpm_);
  if (lnumRect_.intersects(rect)) paint.drawPixmap(lnumRect_.x(), lnumRect_.y(), lnumXpm_);
  if (stxtRect_.intersects(rect)) paint.drawPixmap(stxtRect_.x(), stxtRect_.y(), stxtXpm_);
  if (snumRect_.intersects(rect)) paint.drawPixmap(snumRect_.x(), snumRect_.y(), snumXpm_);
  if (ptxtRect_.intersects(rect)) paint.drawPixmap(ptxtRect_.x(), ptxtRect_.y(), ptxtXpm_);
  if (pnumRect_.intersects(rect)) paint.drawPixmap(pnumRect_.x(), pnumRect_.y(), pnumXpm_);
}

void
PlayField::resizeEvent(QResizeEvent *e) {
  setSize(e->size().width(), e->size().height());
}

void
PlayField::mouseMoveEvent(QMouseEvent *e) {
  lastMouseXPos_ = e->x();
  lastMouseYPos_ = e->y();

  if (!dragInProgress_) return highlight();

  int old_x = dragX_, old_y = dragY_;

  dragX_ = lastMouseXPos_ - mousePosX_;
  dragY_ = lastMouseYPos_ - mousePosY_;

  {
    int x = pixel2x(dragX_ + size_/2);
    int y = pixel2y(dragY_ + size_/2);
    if (x >= 0 && x < levelMap_->width() &&
	y >= 0 && y < levelMap_->height() &&
	pathFinder_.canDragTo(x, y)) {
      x = x2pixel(x);
      y = y2pixel(y);

      if (dragX_ >= x - size_/4 &&
	  dragX_ <  x + size_/4 &&
	  dragY_ >= y - size_/4 &&
	  dragY_ <  y + size_/4) {
	dragX_ = x;
	dragY_ = y;
      }
    }
  }

  if (dragX_ == old_x && dragY_ == old_y) return;

  QRect rect(dragX_, dragY_, size_, size_);

  dragXpm_= QPixmap(size_, size_);

  QPainter paint;
  paint.begin(&dragXpm_);
  paint.setBackground(palette().color(backgroundRole()));
  paint.setBrushOrigin(- dragX_, - dragY_);
  paint.translate((double) (- dragX_), (double) (- dragY_));
  paintPainter(paint, rect);
  paint.end();

  dragImage_ = dragXpm_.toImage();
  for (int yy=0; yy<size_; yy++) {
    for (int xx=0; xx<size_; xx++) {
      QRgb rgb1 = imageData_->objectImg().pixel(xx, yy);
      int r1 = qRed(rgb1);
      int g1 = qGreen(rgb1);
      int b1 = qBlue(rgb1);
      if (r1 != g1 || r1 != b1 || r1 == 255) {
	QRgb rgb2 = dragImage_.pixel(xx, yy);
	int r2 = qRed(rgb2);
	int g2 = qGreen(rgb2);
	int b2 = qBlue(rgb2);
	r2 = (int) (0.75 * r1 + 0.25 * r2 + 0.5);
	g2 = (int) (0.75 * g1 + 0.25 * g2 + 0.5);
	b2 = (int) (0.75 * b1 + 0.25 * b2 + 0.5);
	dragImage_.setPixel(xx, yy, qRgb(r2, g2, b2));
      }
    }
  }

  paint.begin(this);

  // the following line is a workaround for a bug in Qt 2.0.1
  // (and possibly earlier versions)
  paint.setBrushOrigin(0, 0);

  dragXpm_ = QPixmap::fromImage(dragImage_,
			    Qt::OrderedDither|Qt::OrderedAlphaDither|
			    Qt::ColorOnly|Qt::AvoidDither);
  paint.drawPixmap(dragX_, dragY_, dragXpm_);

  {
    int dx = dragX_ - old_x;
    int dy = dragY_ - old_y;
    int y2 = old_y;
    if (dy > 0) {
      paintPainterClip(paint, old_x, old_y, size_, dy);
      // NOTE: clipping is now activated in the QPainter paint
      y2 += dy;
    } else if (dy < 0) {
      paintPainterClip(paint, old_x, old_y+size_+dy, size_, -dy);
      // NOTE: clipping is now activated in the QPainter paint
      dy = -dy;
    }
    if (dx > 0) {
      paintPainterClip(paint, old_x, y2, dx, size_-dy);
      // NOTE: clipping is now activated in the QPainter paint
    } else if (dx < 0) {
      paintPainterClip(paint, old_x+size_+dx, y2, -dx, size_-dy);
      // NOTE: clipping is now activated in the QPainter paint
    }
  }
  paint.end();
}

void
PlayField::highlight() {
  // FIXME: the line below should not be needed
  if (size_ == 0) return;

  int x=pixel2x(lastMouseXPos_);
  int y=pixel2y(lastMouseYPos_);

  if (x < 0 || y < 0 || x >= levelMap_->width() || y >= levelMap_->height())
    return;

  if (x == highlightX_ && y == highlightY_) return;

  if (pathFinder_.canDrag(x, y)) {
    QPainter paint(this);

    if (highlightX_ >= 0) {
      int x = highlightX_, y = highlightY_;
      highlightX_ = -1;
      paintSquare(x, y, paint);
    } else
      changeCursor(Qt::SizeAllCursor);

    if (levelMap_->goal(x, y))
      imageData_->brightTreasure(paint, x2pixel(x), y2pixel(y));
    else
      imageData_->brightObject(paint, x2pixel(x), y2pixel(y));
    highlightX_ = x;
    highlightY_ = y;
  } else {
    if (pathFinder_.canWalkTo(x, y)) changeCursor(Qt::CrossCursor);
    else unsetCursor();
    if (highlightX_ >= 0) {
      QPainter paint(this);

      int x = highlightX_, y = highlightY_;
      highlightX_ = -1;

      paintSquare(x, y, paint);
    }
  }
}

void
PlayField::stopMoving() {
  QAbstractEventDispatcher::instance()->unregisterTimers(this);
  delete moveSequence_;
  moveSequence_ = 0;
  moveInProgress_ = false;
  updateStepsXpm();
  updatePushesXpm();

  update(snumRect_.unite(pnumRect_));

  pathFinder_.updatePossibleMoves();
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
    QAbstractEventDispatcher::instance()->unregisterTimers(this);
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
  case Qt::Key_Up:
    if (e->state() & Qt::ControlModifier) step(x, 0);
    else if (e->state() & Qt::ShiftModifier) push(x, 0);
    else push(x, y-1);
    break;
  case Qt::Key_Down:
    if (e->state() & Qt::ControlModifier) step(x, MAX_Y);
    else if (e->state() & Qt::ShiftModifier) push(x, MAX_Y);
    else push(x, y+1);
    break;
  case Qt::Key_Left:
    if (e->state() & Qt::ControlModifier) step(0, y);
    else if (e->state() & Qt::ShiftModifier) push(0, y);
    else push(x-1, y);
    break;
  case Qt::Key_Right:
    if (e->state() & Qt::ControlModifier) step(MAX_X, y);
    else if (e->state() & Qt::ShiftModifier) push(MAX_X, y);
    else push(x+1, y);
    break;

  case Qt::Key_Q:
    KApplication::kApplication()->closeAllWindows();
    break;

  case Qt::Key_Backspace:
  case Qt::Key_Delete:
    if (e->state() & Qt::ControlModifier) redo();
    else undo();
    break;

#if 0
  case Qt::Key_X:
    levelMap_->random();
    levelChange();
    repaint(false);
    break;

  case Qt::Key_R:
    level(levelMap_->level());
    return;
    break;
  case Qt::Key_N:
    nextLevel();
    return;
    break;
  case Qt::Key_P:
    previousLevel();
    return;
    break;
  case Qt::Key_U:
    undo();
    return;
    break;
  case Qt::Key_I:
    history_->redo(levelMap_);
    repaint(false);
    return;
    break;

  case Qt::Key_S:
    {
      QString buf;
      history_->save(buf);
      printf("%s\n", (char *) buf);
    }
    return;
    break;

  case Qt::Key_L:
    stopMoving();
    history_->clear();
    level(levelMap_->level());
    {
      char buf[4096]="r1*D1*D1*r1*@r1*D1*";
      //scanf("%s", buf);
      history_->load(levelMap_, buf);
    }
    updateStepsXpm();
    updatePushesXpm();
    repaint(false);
    return;
    break;
#endif


  case Qt::Key_Print:
    HtmlPrinter::printHtml(levelMap_);
    break;

  default:
    e->ignore();
    return;
    break;
  }
}

void
PlayField::stopDrag() {
  if (!dragInProgress_) return;

  unsetCursor();

  QPainter paint(this);

  // the following line is a workaround for a bug in Qt 2.0.1
  // (and possibly earlier versions)
  paint.setBrushOrigin(0, 0);

  int x = highlightX_, y = highlightY_;
  paintSquare(x, y, paint);

  paintPainterClip(paint, dragX_, dragY_, size_, size_);
  // NOTE: clipping is now activated in the QPainter paint
  dragInProgress_ = false;

}

void
PlayField::dragObject(int xpixel, int ypixel) {
  int x=pixel2x(xpixel - mousePosX_ + size_/2);
  int y=pixel2y(ypixel - mousePosY_ + size_/2);

  if (x == highlightX_ && y == highlightY_) return;

  pathFinder_.drag(highlightX_, highlightY_, x, y);
  stopDrag();
}


void
PlayField::mousePressEvent(QMouseEvent *e) {
  if (!canMoveNow()) return;

  if (dragInProgress_) {
    if (e->button() == Qt::LeftButton) dragObject(e->x(), e->y());
    else stopDrag();
    return;
  }

  int x=pixel2x(e->x());
  int y=pixel2y(e->y());

  if (x < 0 || y < 0 || x >= levelMap_->width() || y >= levelMap_->height())
    return;

  if (e->button() == Qt::LeftButton && pathFinder_.canDrag(x, y)) {
    QPainter paint(this);
    changeCursor(Qt::SizeAllCursor);

    if (levelMap_->goal(x, y))
      imageData_->brightTreasure(paint, x2pixel(x), y2pixel(y));
    else
      imageData_->brightObject(paint, x2pixel(x), y2pixel(y));
    highlightX_ = x;
    highlightY_ = y;
    pathFinder_.updatePossibleDestinations(x, y);

    dragX_ = x2pixel(x);
    dragY_ = y2pixel(y);
    mousePosX_ = e->x() - dragX_;
    mousePosY_ = e->y() - dragY_;
    dragInProgress_ = true;
  }

  Move *m;
  switch (e->button()) {
  case Qt::LeftButton:
    m = pathFinder_.search(levelMap_, x, y);
    if (m != 0) {
      history_->add(m);

      startMoving(m);
    }
    break;
  case Qt::MidButton:
    undo();
    return;
    break;
  case Qt::RightButton:
    push(x, y);
    break;

  default:
    return;
  }
}

void
PlayField::wheelEvent(QWheelEvent *e) {
  wheelDelta_ += e->delta();

  if (wheelDelta_ >= 120) {
    wheelDelta_ %= 120;
    redo();
  } else if (wheelDelta_ <= -120) {
    wheelDelta_ = -(-wheelDelta_ % 120);
    undo();
  }
}

void
PlayField::mouseReleaseEvent(QMouseEvent *e) {
  if (dragInProgress_) dragObject(e->x(), e->y());
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
PlayField::leaveEvent(QEvent *) {
  stopDrag();
}

void
PlayField::setSize(int w, int h) {
  int sbarHeight = statusMetrics_.height();
  int sbarNumWidth = statusMetrics_.boundingRect("88888").width()+8;
  int sbarLevelWidth = statusMetrics_.boundingRect(levelText_).width()+8;
  int sbarStepsWidth = statusMetrics_.boundingRect(stepsText_).width()+8;
  int sbarPushesWidth = statusMetrics_.boundingRect(pushesText_).width()+8;

  pnumRect_.setRect(w-sbarNumWidth, h-sbarHeight, sbarNumWidth, sbarHeight);
  ptxtRect_.setRect(pnumRect_.x()-sbarPushesWidth, h-sbarHeight, sbarPushesWidth, sbarHeight);
  snumRect_.setRect(ptxtRect_.x()-sbarNumWidth, h-sbarHeight, sbarNumWidth, sbarHeight);
  stxtRect_.setRect(snumRect_.x()-sbarStepsWidth, h-sbarHeight, sbarStepsWidth, sbarHeight);
  lnumRect_.setRect(stxtRect_.x()-sbarNumWidth, h-sbarHeight, sbarNumWidth, sbarHeight);
  ltxtRect_.setRect(lnumRect_.x()-sbarLevelWidth, h-sbarHeight, sbarLevelWidth, sbarHeight);
  collRect_.setRect(0, h-sbarHeight, ltxtRect_.x(), sbarHeight);

  collXpm_ = QPixmap(collRect_.size());
  ltxtXpm_ = QPixmap(ltxtRect_.size());
  lnumXpm_ = QPixmap(lnumRect_.size());
  stxtXpm_ = QPixmap(stxtRect_.size());
  snumXpm_ = QPixmap(snumRect_.size());
  ptxtXpm_ = QPixmap(ptxtRect_.size());
  pnumXpm_ = QPixmap(pnumRect_.size());

  h -= sbarHeight;

  int cols = levelMap_->width();
  int rows = levelMap_->height();

  // FIXME: the line below should not be needed
  if (cols == 0 || rows == 0) return;

  int xsize = w / cols;
  int ysize = h / rows;

  if (xsize < 8) xsize = 8;
  if (ysize < 8) ysize = 8;

  size_ = imageData_->resize(xsize > ysize ? ysize : xsize);

  xOffs_ = (w - cols*size_) / 2;
  yOffs_ = (h - rows*size_) / 2;


  updateCollectionXpm();
  updateTextXpm();
  updateLevelXpm();
  updateStepsXpm();
  updatePushesXpm();
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
  update();
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
  update();
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
  updateStepsXpm();
  updatePushesXpm();
  update();
}

void
PlayField::changeCollection(LevelCollection *collection) {
  if (levelMap_->collection() == collection) return;
  levelMap_->changeCollection(collection);
  levelChange();
  //erase(collRect_);
  update();
}

void
PlayField::updateCollectionXpm() {
  if (collXpm_.isNull()) return;

  QPainter paint(&collXpm_);
  paint.setBrushOrigin(- collRect_.x(), - collRect_.y());
  paint.fillRect(0, 0, collRect_.width(), collRect_.height(), background_);

  paint.setFont(statusFont_);
  paint.setPen(QColor(0,255,0));
  paint.drawText(0, 0, collRect_.width(), collRect_.height(),
		 Qt::AlignLeft, collectionName());
}

void
PlayField::updateTextXpm() {
  if (ltxtXpm_.isNull()) return;

  QPainter paint;

  paint.begin(&ltxtXpm_);
  paint.setBrushOrigin(- ltxtRect_.x(), - ltxtRect_.y());
  paint.fillRect(0, 0, ltxtRect_.width(), ltxtRect_.height(), background_);
  paint.setFont(statusFont_);
  paint.setPen(QColor(128,128,128));
  paint.drawText(0, 0, ltxtRect_.width(), ltxtRect_.height(), Qt::AlignLeft, levelText_);
  paint.end();

  paint.begin(&stxtXpm_);
  paint.setBrushOrigin(- stxtRect_.x(), - stxtRect_.y());
  paint.fillRect(0, 0, stxtRect_.width(), stxtRect_.height(), background_);
  paint.setFont(statusFont_);
  paint.setPen(QColor(128,128,128));
  paint.drawText(0, 0, stxtRect_.width(), stxtRect_.height(), Qt::AlignLeft, stepsText_);
  paint.end();

  paint.begin(&ptxtXpm_);
  paint.setBrushOrigin(- ptxtRect_.x(), - ptxtRect_.y());
  paint.fillRect(0, 0, ptxtRect_.width(), ptxtRect_.height(), background_);
  paint.setFont(statusFont_);
  paint.setPen(QColor(128,128,128));
  paint.drawText(0, 0, ptxtRect_.width(), ptxtRect_.height(), Qt::AlignLeft, pushesText_);
  paint.end();
}

void
PlayField::updateLevelXpm() {
  if (lnumXpm_.isNull()) return;

  QPainter paint(&lnumXpm_);
  paint.setBrushOrigin(- lnumRect_.x(), - lnumRect_.y());
  paint.fillRect(0, 0, lnumRect_.width(), lnumRect_.height(), background_);

  QString str;
  paint.setFont(statusFont_);
  paint.setPen(QColor(255,0,0));
  paint.drawText(0, 0, lnumRect_.width(), lnumRect_.height(),
		 Qt::AlignLeft, str.sprintf("%05d", level()+1));
}

void
PlayField::updateStepsXpm() {
  if (snumXpm_.isNull()) return;

  QPainter paint(&snumXpm_);
  paint.setBrushOrigin(- snumRect_.x(), - snumRect_.y());
  paint.fillRect(0, 0, snumRect_.width(), snumRect_.height(), background_);

  QString str;
  paint.setFont(statusFont_);
  paint.setPen(QColor(255,0,0));
  paint.drawText(0, 0, snumRect_.width(), snumRect_.height(),
		 Qt::AlignLeft, str.sprintf("%05d", totalMoves()));
}

void
PlayField::updatePushesXpm() {
  if (pnumXpm_.isNull()) return;

  QPainter paint(&pnumXpm_);
  paint.setBrushOrigin(- pnumRect_.x(), - pnumRect_.y());
  paint.fillRect(0, 0, pnumRect_.width(), pnumRect_.height(), background_);

  QString str;
  paint.setFont(statusFont_);
  paint.setPen(QColor(255,0,0));
  paint.drawText(0, 0, pnumRect_.width(), pnumRect_.height(),
		 Qt::AlignLeft, str.sprintf("%05d", totalPushes()));
}


void
PlayField::setAnimationSpeed(int num)
{
  assert(num >= 0 && num <= 3);

  animDelay_ = num;
}

// FIXME: clean up bookmark stuff

// static const int bookmark_id[] = {
//   0, 1, 8, 2, 9, 3, 5, 6, 7, 4
// };

void
PlayField::setBookmark(Bookmark *bm) {
  if (!levelMap_->goodLevel()) return;

  if (collection()->id() < 0) {
    KMessageBox::sorry(this, i18n("Sorry, bookmarks for external levels\n"
				  "is not implemented yet."));
    return;
  }

  bm->set(collection()->id(), levelMap_->level(), levelMap_->totalMoves(), history_);
}

void
PlayField::goToBookmark(Bookmark *bm) {
  level(bm->level());
  levelChange();
  if (!bm->goTo(levelMap_, history_))
      kDebug()<<"Warning: bad bookmark"<<endl;
  //updateLevelXpm();
  updateStepsXpm();
  updatePushesXpm();
  update();
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
