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

#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <qimage.h>
#include <qpixmap.h>

#include <krandomsequence.h>

#include "Array.H"

class QPainter;

#define SMALL_STONES 4
#define LARGE_STONES 6
#define OTHER_IMAGES 5
#define NO_OF_IMAGES (SMALL_STONES + LARGE_STONES + OTHER_IMAGES)

class
ImageData {
public:
  virtual ~ImageData();

  int resize(int size);
  int size() { return size_; }

  void wall(QPainter &p, int x, int y, int index, bool left, bool right);
  void floor(QPainter &p, int x, int y);
  void goal(QPainter &p, int x, int y);
  void man(QPainter &p, int x, int y);
  void object(QPainter &p, int x, int y);
  void saveman(QPainter &p, int x, int y);
  void treasure(QPainter &p, int x, int y);

  const QPixmap &background() { return background_; }

protected:
  ImageData();

  void expandIndex(int size);

  const QPixmap &upperLarge(int index);
  const QPixmap &lowerLarge(int index);
  const QPixmap &leftSmall(int index);
  const QPixmap &rightSmall(int index);

  QImage images_[NO_OF_IMAGES];

  QPixmap smallStone_xpm_[SMALL_STONES];
  QPixmap largeStone_xpm_[LARGE_STONES];
  QPixmap otherPixmaps_[OTHER_IMAGES];
  QPixmap background_;
  
  int indexSize_;
  Array<char> upperLargeIndex_;
  Array<char> lowerLargeIndex_;
  Array<char> leftSmallIndex_;
  Array<char> rightSmallIndex_;

  int size_, halfSize_;
  KRandomSequence random;
};

#endif  /* IMAGEDATA_H */
