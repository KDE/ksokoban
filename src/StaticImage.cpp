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

#include <assert.h>
#include "StaticImage.h"

#include "images/data.c"

const unsigned char *const
imageData[NO_OF_IMAGES] = {
  halfstone_1_data_,
  halfstone_2_data_,
  halfstone_3_data_,
  halfstone_4_data_,

  stone_1_data_,
  stone_2_data_,
  stone_3_data_,
  stone_4_data_,
  stone_5_data_,
  stone_6_data_,

  object_data_,
  treasure_data_,
  goal_data_,
  man_data_,
  saveman_data_,
};

const unsigned
imageSize[NO_OF_IMAGES] = {
  sizeof halfstone_1_data_,
  sizeof halfstone_2_data_,
  sizeof halfstone_3_data_,
  sizeof halfstone_4_data_,

  sizeof stone_1_data_,
  sizeof stone_2_data_,
  sizeof stone_3_data_,
  sizeof stone_4_data_,
  sizeof stone_5_data_,
  sizeof stone_6_data_,

  sizeof object_data_,
  sizeof treasure_data_,
  sizeof goal_data_,
  sizeof man_data_,
  sizeof saveman_data_,
};

StaticImage::StaticImage () {
  bool valid = background_.loadFromData((const uchar *) starfield_data_,
					(uint) sizeof (starfield_data_));

  if (!valid) {
    background_ = background_.copy(0, 0, 128, 128);
    background_.fill(Qt::black);
  }

  for (int i=0; i<NO_OF_IMAGES; i++) {
    images_[i].loadFromData((const uchar *) imageData[i], (uint) imageSize[i]);
  }

  //resize(32,32);
}

StaticImage::~StaticImage () {
}
