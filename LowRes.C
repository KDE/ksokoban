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

#include "LowRes.H"

#include "images/20/data.c"

/*     0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15 
 *         x       x       x       x       x       x       x       x 
 *     *   *   *   *  x*  x*  x*  x*   *x  *x  *x  *x x*x x*x x*x x*x  
 *             x   x           x   x           x   x           x   x 
 */

const unsigned char *const
imageData_20[NO_OF_IMAGES] = {
  vertiwall_data_20, /*  wall_data */
  vertiwall_data_20, /*  southwall_data */
  vertiwall_data_20, /*  northwall_data */
  vertiwall_data_20, /*  vertiwall_data */
  eastwall_data_20,  /*  eastwall_data */
  eastwall_data_20,  /*  lrcornerwall_data */
  eastwall_data_20,  /*  urcornerwall_data */
  eastwall_data_20,  /*  east_twall_data */
  westwall_data_20,  /*  westwall_data */
  westwall_data_20,  /*  llcornerwall_data */
  westwall_data_20,  /*  ulcornerwall_data */
  westwall_data_20,  /*  west_twall_data */
  horizwall_data_20, /*  horizwall_data */
  horizwall_data_20, /*  south_twall_data */
  horizwall_data_20, /*  north_twall_data */
  horizwall_data_20, /*  centerwall_data */
  floor_data_20,
  goal_data_20,
  man_data_20,
  object_data_20,
  saveman_data_20,
  treasure_data_20
};

const unsigned
imageSize_20[NO_OF_IMAGES] = {
  sizeof vertiwall_data_20, /*  wall_data */
  sizeof vertiwall_data_20, /*  southwall_data */
  sizeof vertiwall_data_20, /*  northwall_data */
  sizeof vertiwall_data_20, /*  vertiwall_data */
  sizeof eastwall_data_20,  /*  eastwall_data */
  sizeof eastwall_data_20,  /*  lrcornerwall_data */
  sizeof eastwall_data_20,  /*  urcornerwall_data */
  sizeof eastwall_data_20,  /*  east_twall_data */
  sizeof westwall_data_20,  /*  westwall_data */
  sizeof westwall_data_20,  /*  llcornerwall_data */
  sizeof westwall_data_20,  /*  ulcornerwall_data */
  sizeof westwall_data_20,  /*  west_twall_data */
  sizeof horizwall_data_20, /*  horizwall_data */
  sizeof horizwall_data_20, /*  south_twall_data */
  sizeof horizwall_data_20, /*  north_twall_data */
  sizeof horizwall_data_20, /*  centerwall_data */
  sizeof floor_data_20,
  sizeof goal_data_20,
  sizeof man_data_20,
  sizeof object_data_20,
  sizeof saveman_data_20,
  sizeof treasure_data_20
};

LowRes::LowRes () {
  loadImages (imageData_20, imageSize_20);
}
