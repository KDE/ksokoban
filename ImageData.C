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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ImageData.H"
#include "RandomGenerator.H"

#include <assert.h>
#include <qpainter.h>
#include <qpixmap.h>

ImageData::ImageData() : indexSize_(0), size_(0), halfSize_(0) {
  rng_ = new RandomGenerator();
}

ImageData::~ImageData() {
  delete rng_;
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
    upperLargeIndex_[i] = rng_->next(LARGE_STONES);
    lowerLargeIndex_[i] = rng_->next(LARGE_STONES);
    leftSmallIndex_[i] = rng_->next(SMALL_STONES);
    rightSmallIndex_[i] = rng_->next(SMALL_STONES);
  }

  indexSize_ = size;
}

const QPixmap &
ImageData::upperLarge(int index) {
  assert(index >= 0);
  if (indexSize_ <= index) expandIndex(index);
  return largeStone_xpm_[upperLargeIndex_[index]];
}

const QPixmap &
ImageData::lowerLarge(int index) {
  assert(index >= 0);
  if (indexSize_ <= index) expandIndex(index);
  return largeStone_xpm_[lowerLargeIndex_[index]];
}

const QPixmap &
ImageData::leftSmall(int index) {
  assert(index >= 0);
  if (indexSize_ <= index) expandIndex(index);
  return smallStone_xpm_[leftSmallIndex_[index]];
}

const QPixmap &
ImageData::rightSmall(int index) {
  assert(index >= 0);
  if (indexSize_ <= index) expandIndex(index);
  return smallStone_xpm_[rightSmallIndex_[index]];
}

int
ImageData::resize(int size) {
  assert(size > 0);
  size &= ~1u;
  if (size == size_) return size;

  size_ = size;
  halfSize_ = size/2;

  for (int i=0; i<SMALL_STONES; i++) {
#if QT_VERSION < 200
    smallStone_xpm_[i].convertFromImage(images_[i].smoothScale(halfSize_, halfSize_), ColorOnly|DiffuseDither|DiffuseAlphaDither|AvoidDither);
#else
    smallStone_xpm_[i].convertFromImage(images_[i].smoothScale(halfSize_, halfSize_), QPixmap::ColorOnly|QPixmap::DiffuseDither|QPixmap::DiffuseAlphaDither|QPixmap::AvoidDither);
#endif
  }

  for (int i=0; i<LARGE_STONES; i++) {
#if QT_VERSION < 200
    largeStone_xpm_[i].convertFromImage(images_[SMALL_STONES+i].smoothScale(size_, halfSize_) , ColorOnly|DiffuseDither|DiffuseAlphaDither|AvoidDither);
#else
    largeStone_xpm_[i].convertFromImage(images_[SMALL_STONES+i].smoothScale(size_, halfSize_) , QPixmap::ColorOnly|QPixmap::DiffuseDither|QPixmap::DiffuseAlphaDither|QPixmap::AvoidDither);
#endif
  }

  // I don't use DiffuseDither for the objects on the "floor" since
  // it gives spurious dots on the floor around them
  for (int i=0; i<OTHER_IMAGES; i++) {
#if QT_VERSION < 200
    otherPixmaps_[i].convertFromImage(images_[SMALL_STONES+LARGE_STONES+i].smoothScale(size_, size_), ColorOnly|OrderedDither|OrderedAlphaDither|AvoidDither);
#else
    otherPixmaps_[i].convertFromImage(images_[SMALL_STONES+LARGE_STONES+i].smoothScale(size_, size_), QPixmap::ColorOnly|QPixmap::OrderedDither|QPixmap::OrderedAlphaDither|QPixmap::AvoidDither);
#endif
  }

  return size_;
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
  p.drawPixmap(x, y, otherPixmaps_[0]);
}

void
ImageData::man(QPainter &p, int x, int y) {
  p.drawPixmap(x, y, otherPixmaps_[1]);
}

void
ImageData::object(QPainter &p, int x, int y) {
  p.drawPixmap(x, y, otherPixmaps_[2]);
}

void
ImageData::saveman(QPainter &p, int x, int y) {
  p.drawPixmap(x, y, otherPixmaps_[3]);
}

void
ImageData::treasure(QPainter &p, int x, int y) {
  p.drawPixmap(x, y, otherPixmaps_[4]);
}
