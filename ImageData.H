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

#include <qpixmap.h>

#define NO_OF_IMAGES 22
#define EXTRA_IMAGES 1

class
ImageData {
protected:
  //  static const unsigned NO_OF_IMAGES=22;
  QPixmap *images_[NO_OF_IMAGES+EXTRA_IMAGES];
  int width_;
  int height_;

public:
  int width () { return width_; }
  int height () { return height_; }

  /*     0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15 
   *         x       x       x       x       x       x       x       x 
   *     *   *   *   *  x*  x*  x*  x*   *x  *x  *x  *x x*x x*x x*x x*x  
   *             x   x           x   x           x   x           x   x 
   */

  const QPixmap *wall (bool up, bool down, bool left, bool right) {
    return images_[ (up!=0)          |
		    ((down!=0)  << 1) |
		    ((left!=0)  << 2) |
		    ((right!=0) << 3)];
  }

  const QPixmap *floor    () { return images_[16]; }
  const QPixmap *goal     () { return images_[17]; }
  const QPixmap *man      () { return images_[18]; }
  const QPixmap *object   () { return images_[19]; }
  const QPixmap *saveman  () { return images_[20]; }
  const QPixmap *treasure () { return images_[21]; }

  const QPixmap *background () { return images_[22]; }
		   
};

#endif  /* IMAGEDATA_H */
