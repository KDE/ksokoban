/*
 *  ksokoban - a Sokoban game for KDE
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

#include "ImageData.h"

#include <assert.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qcolor.h>

ImageData::ImageData() : indexSize_(0), size_(0), halfSize_(0) {
  random.setSeed(0);
}

ImageData::~ImageData() {
}

void
ImageData::expandIndex(int size) {
  size++;
  assert(size < 2500);

  upperLargeIndex_.resize(size);
  lowerLargeIndex_.resize(size);
  leftSmallIndex_.resize(size);
  rightSmallIndex_.resize(size);

  for (int i=indexSize_; i<size; i++) {
    upperLargeIndex_[i] = random.getLong(LARGE_STONES);
    lowerLargeIndex_[i] = random.getLong(LARGE_STONES);
    leftSmallIndex_[i] = random.getLong(SMALL_STONES);
    rightSmallIndex_[i] = random.getLong(SMALL_STONES);
  }

  indexSize_ = size;
}

const QPixmap &
ImageData::upperLarge(int index) {
  assert(index >= 0);
  if (indexSize_ <= index) expandIndex(index);
  return largeStone_xpm_[(unsigned char)upperLargeIndex_[index]];
}

const QPixmap &
ImageData::lowerLarge(int index) {
  assert(index >= 0);
  if (indexSize_ <= index) expandIndex(index);
  return largeStone_xpm_[(unsigned char)lowerLargeIndex_[index]];
}

const QPixmap &
ImageData::leftSmall(int index) {
  assert(index >= 0);
  if (indexSize_ <= index) expandIndex(index);
  return smallStone_xpm_[(unsigned char)leftSmallIndex_[index]];
}

const QPixmap &
ImageData::rightSmall(int index) {
  assert(index >= 0);
  if (indexSize_ <= index) expandIndex(index);
  return smallStone_xpm_[(unsigned char)rightSmallIndex_[index]];
}

int
ImageData::resize(int size) {
  assert(size > 0);
  size &= ~1u;
  if (size == size_) return size;

  size_ = size;
  halfSize_ = size/2;

  for (int i=0; i<SMALL_STONES; i++) {
    image2pixmap(images_[i].smoothScale(halfSize_, halfSize_), smallStone_xpm_[i]);
//     smallStone_xpm_[i].convertFromImage(images_[i].smoothScale(halfSize_, halfSize_), QPixmap::ColorOnly|QPixmap::DiffuseDither|QPixmap::DiffuseAlphaDither|QPixmap::AvoidDither);
  }

  for (int i=0; i<LARGE_STONES; i++) {
    image2pixmap(images_[SMALL_STONES+i].smoothScale(size_, halfSize_), largeStone_xpm_[i]);
//     largeStone_xpm_[i].convertFromImage(images_[SMALL_STONES+i].smoothScale(size_, halfSize_) , QPixmap::ColorOnly|QPixmap::DiffuseDither|QPixmap::DiffuseAlphaDither|QPixmap::AvoidDither);
  }

  objectImg_ = images_[SMALL_STONES+LARGE_STONES].smoothScale(size_, size_);

  // Use copy() because if the size is not changed, smoothScale is not
  // really a copy
  // Use {[Geometry] height=753 width=781} to test

  if (objectImg_.width() == size_) objectImg_ = objectImg_.copy();

  image2pixmap(objectImg_, otherPixmaps_[0], false);
  brighten(objectImg_);
  image2pixmap(objectImg_, brightObject_, false);

  QImage img = images_[SMALL_STONES+LARGE_STONES+1].smoothScale(size_, size_);
  if (img.width() == size_) img = img.copy();

  image2pixmap(img, otherPixmaps_[1], false);
  brighten(img);
  image2pixmap(img, brightTreasure_, false);

  for (int i=2; i<OTHER_IMAGES; i++) {
    image2pixmap(images_[SMALL_STONES+LARGE_STONES+i].smoothScale(size_, size_), otherPixmaps_[i]);
//     otherPixmaps_[i].convertFromImage(images_[SMALL_STONES+LARGE_STONES+i].smoothScale(size_, size_), QPixmap::ColorOnly|QPixmap::OrderedDither|QPixmap::OrderedAlphaDither|QPixmap::AvoidDither);
  }

  return size_;
}

// Don't use DiffuseDither for the objects on the "floor" since
// it gives spurious dots on the floor around them

void
ImageData::image2pixmap(QImage img, QPixmap& xpm, bool diffuse) {
  xpm.convertFromImage(img,
		       (diffuse ?
			(QPixmap::DiffuseDither|QPixmap::DiffuseAlphaDither) :
			(QPixmap::OrderedDither|QPixmap::OrderedAlphaDither))|
		       QPixmap::ColorOnly|QPixmap::AvoidDither);
}

void
ImageData::brighten(QImage& img) {
  assert(img.depth() == 32);

  for (int y=0; y<img.height(); y++) {
    for (int x=0; x<img.width(); x++) {
      QRgb rgb = img.pixel(x, y);
      int r = qRed(rgb);
      int g = qGreen(rgb);
      int b = qBlue(rgb);

      if (r > g && r > b) {
       // only modify redish pixels

       QColor col(r, g, b);
       QColor lcol = col.light(130);

       img.setPixel(x, y, lcol.rgb());
      }
    }
  }
}

void
ImageData::wall(QPainter &p, int x, int y, int index, bool left, bool right) {
  if (left) p.drawPixmap(x, y, upperLarge(index-1), halfSize_);
  else p.drawPixmap(x, y, leftSmall(index));

  if (right) p.drawPixmap(x+halfSize_, y, upperLarge(index), 0, 0, halfSize_);
  else p.drawPixmap(x+halfSize_, y, rightSmall(index));

  p.drawPixmap(x, y+halfSize_, lowerLarge(index));
}

void
ImageData::floor(QPainter &p, int x, int y) {
  p.eraseRect(x, y, size_, size_);
}

void
ImageData::goal(QPainter &p, int x, int y) {
  p.drawPixmap(x, y, otherPixmaps_[2]);
}

void
ImageData::man(QPainter &p, int x, int y) {
  p.drawPixmap(x, y, otherPixmaps_[3]);
}

void
ImageData::object(QPainter &p, int x, int y) {
  p.drawPixmap(x, y, otherPixmaps_[0]);
}

void
ImageData::saveman(QPainter &p, int x, int y) {
  p.drawPixmap(x, y, otherPixmaps_[4]);
}

void
ImageData::treasure(QPainter &p, int x, int y) {
  p.drawPixmap(x, y, otherPixmaps_[1]);
}

void
ImageData::brightObject(QPainter &p, int x, int y) {
  p.drawPixmap(x, y, brightObject_);
}

void
ImageData::brightTreasure(QPainter &p, int x, int y) {
  p.drawPixmap(x, y, brightTreasure_);
}
