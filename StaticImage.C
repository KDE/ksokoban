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

#include "config.h"

#ifdef HAVE_LIBZ
#include <zlib.h>
#endif

#include "StaticImage.H"

#include "images/data.c"

#define BUFSIZE (128*1024)

void
StaticImage::loadImage (int num, const unsigned char *data, int size) {
#ifdef HAVE_LIBZ
  uLongf bufLen;
  unsigned char *buf = new unsigned char[BUFSIZE];

  bufLen = BUFSIZE;
  uncompress (buf, &bufLen, data, size);

  images_[num] = new QPixmap;
  images_[num]->loadFromData (buf, bufLen);

  delete [] buf;
#else
  images_[num] = new QPixmap;
  images_[num]->loadFromData (data, size);
#endif
}

StaticImage::StaticImage () {
  for (int i=0; i<NO_OF_IMAGES; i++) {
    images_[i] = 0;
  }
  loadImage (NO_OF_IMAGES, (const uchar *) background_data,
	     (uint) sizeof (background_data));
}

StaticImage::~StaticImage () {
  for (int i=0; i<NO_OF_IMAGES+EXTRA_IMAGES; i++) {
    if (images_[i] == 0) continue;
    delete images_[i];
    for (int j=i+1; j<NO_OF_IMAGES+EXTRA_IMAGES; j++) {
      if (images_[i] == images_[j]) {
	images_[j] = 0;
      }
    }
  }
}



void
StaticImage::loadImages (const unsigned char *const data[NO_OF_IMAGES], 
			 const unsigned size_[NO_OF_IMAGES]) {
  width_ = height_ = 1;

  for (int i=0; i<NO_OF_IMAGES; i++) {
    if (images_[i] != 0) continue;
    loadImage (i, (const uchar *) data[i], (uint) size_[i]);
    if (images_[i]->width () > width_) width_ = images_[i]->width ();
    if (images_[i]->height () > height_) height_ = images_[i]->height ();

    for (int j=i+1; j<NO_OF_IMAGES; j++) {
      if (data[i] == data[j]) {
	images_[j] = images_[i];
      }
    }
  }

  for (unsigned i=0; i<NO_OF_IMAGES; i++)
    if (images_[i]->width () != width_ || images_[i]->height () != height_)
      images_[i]->resize (width_, height_);
}
