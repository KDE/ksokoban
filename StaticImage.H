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

#ifndef STATICIMAGE_H
#define STATICIMAGE_H

#include "ImageData.H"

class
StaticImage : public ImageData {
private:
  void loadImage (int num, const unsigned char *data, int size);
protected:
  void loadImages (const unsigned char *const data[NO_OF_IMAGES], const unsigned size[NO_OF_IMAGES]);

public:
  StaticImage ();
  ~StaticImage ();

};

#endif  /* STATICIMAGE_H */
