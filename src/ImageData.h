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

#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <QImage>
#include <QPixmap>

#include <QRandomGenerator>

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
  void brightObject(QPainter &p, int x, int y);
  void brightTreasure(QPainter &p, int x, int y);

  const QPixmap &background() { return background_; }
  const QImage& objectImg() const { return objectImg_; }

protected:
  ImageData();

  void expandIndex(int size);
  void image2pixmap(const QImage &img, QPixmap& xpm, bool diffuse=true);
  void brighten(QImage& img);

  const QPixmap &upperLarge(int index);
  const QPixmap &lowerLarge(int index);
  const QPixmap &leftSmall(int index);
  const QPixmap &rightSmall(int index);

  QImage images_[NO_OF_IMAGES];

  QPixmap smallStone_xpm_[SMALL_STONES];
  QPixmap largeStone_xpm_[LARGE_STONES];
  QPixmap otherPixmaps_[OTHER_IMAGES];
  QPixmap background_, brightObject_, brightTreasure_;
  QPixmap floor_;
  QImage  objectImg_;
  
  int indexSize_;
  QByteArray upperLargeIndex_;
  QByteArray lowerLargeIndex_;
  QByteArray leftSmallIndex_;
  QByteArray rightSmallIndex_;

  int size_, halfSize_;
  QRandomGenerator random;
};

#endif  /* IMAGEDATA_H */
