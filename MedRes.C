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

#include "MedRes.H"

#include "images/32/data.c"

/*     0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15 
 *         x       x       x       x       x       x       x       x 
 *     *   *   *   *  x*  x*  x*  x*   *x  *x  *x  *x x*x x*x x*x x*x  
 *             x   x           x   x           x   x           x   x 
 */

const unsigned char *const
imageData[NO_OF_IMAGES] = {
  vertiwall_data, /*  wall_data, */
  vertiwall_data, /*  southwall_data, */
  vertiwall_data, /*  northwall_data, */
  vertiwall_data, /*  vertiwall_data, */
  eastwall_data,  /*  eastwall_data, */
  eastwall_data,  /*  lrcornerwall_data, */
  eastwall_data,  /*  urcornerwall_data, */
  eastwall_data,  /*  east_twall_data, */
  westwall_data,  /*  westwall_data, */
  westwall_data,  /*  llcornerwall_data, */
  westwall_data,  /*  ulcornerwall_data, */
  westwall_data,  /*  west_twall_data, */
  horizwall_data, /*  horizwall_data, */
  horizwall_data, /*  south_twall_data, */
  horizwall_data, /*  north_twall_data, */
  horizwall_data, /*  centerwall_data, */
  floor_data,
  goal_data,
  man_data,
  object_data,
  saveman_data,
  treasure_data
};

const unsigned
imageSize[NO_OF_IMAGES] = {
  sizeof vertiwall_data, /*  wall_data, */
  sizeof vertiwall_data, /*  southwall_data, */
  sizeof vertiwall_data, /*  northwall_data, */
  sizeof vertiwall_data, /*  vertiwall_data, */
  sizeof eastwall_data,  /*  eastwall_data, */
  sizeof eastwall_data,  /*  lrcornerwall_data, */
  sizeof eastwall_data,  /*  urcornerwall_data, */
  sizeof eastwall_data,  /*  east_twall_data, */
  sizeof westwall_data,  /*  westwall_data, */
  sizeof westwall_data,  /*  llcornerwall_data, */
  sizeof westwall_data,  /*  ulcornerwall_data, */
  sizeof westwall_data,  /*  west_twall_data, */
  sizeof horizwall_data, /*  horizwall_data, */
  sizeof horizwall_data, /*  south_twall_data, */
  sizeof horizwall_data, /*  north_twall_data, */
  sizeof horizwall_data, /*  centerwall_data, */
  sizeof floor_data,
  sizeof goal_data,
  sizeof man_data,
  sizeof object_data,
  sizeof saveman_data,
  sizeof treasure_data
};

MedRes::MedRes () {
  loadImages (imageData, imageSize);
}
