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

#include "HiRes.H"

#include "images/40/data.c"

/*     0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15 
 *         x       x       x       x       x       x       x       x 
 *     *   *   *   *  x*  x*  x*  x*   *x  *x  *x  *x x*x x*x x*x x*x  
 *             x   x           x   x           x   x           x   x 
 */

const unsigned char *const
imageData_40[NO_OF_IMAGES] = {
  vertiwall_data_40, /*  wall_data, */
  vertiwall_data_40, /*  southwall_data, */
  vertiwall_data_40, /*  northwall_data, */
  vertiwall_data_40, /*  vertiwall_data, */
  eastwall_data_40,  /*  eastwall_data, */
  eastwall_data_40,  /*  lrcornerwall_data, */
  eastwall_data_40,  /*  urcornerwall_data, */
  eastwall_data_40,  /*  east_twall_data, */
  westwall_data_40,  /*  westwall_data, */
  westwall_data_40,  /*  llcornerwall_data, */
  westwall_data_40,  /*  ulcornerwall_data, */
  westwall_data_40,  /*  west_twall_data, */
  horizwall_data_40, /*  horizwall_data, */
  horizwall_data_40, /*  south_twall_data, */
  horizwall_data_40, /*  north_twall_data, */
  horizwall_data_40, /*  centerwall_data, */
  floor_data_40,
  goal_data_40,
  man_data_40,
  object_data_40,
  saveman_data_40,
  treasure_data_40
};

const unsigned
imageSize_40[NO_OF_IMAGES] = {
  sizeof vertiwall_data_40, /*  wall_data */
  sizeof vertiwall_data_40, /*  southwall_data */
  sizeof vertiwall_data_40, /*  northwall_data */
  sizeof vertiwall_data_40, /*  vertiwall_data */
  sizeof eastwall_data_40,  /*  eastwall_data */
  sizeof eastwall_data_40,  /*  lrcornerwall_data */
  sizeof eastwall_data_40,  /*  urcornerwall_data */
  sizeof eastwall_data_40,  /*  east_twall_data */
  sizeof westwall_data_40,  /*  westwall_data */
  sizeof westwall_data_40,  /*  llcornerwall_data */
  sizeof westwall_data_40,  /*  ulcornerwall_data */
  sizeof westwall_data_40,  /*  west_twall_data */
  sizeof horizwall_data_40, /*  horizwall_data */
  sizeof horizwall_data_40, /*  south_twall_data */
  sizeof horizwall_data_40, /*  north_twall_data */
  sizeof horizwall_data_40, /*  centerwall_data */
  sizeof floor_data_40,
  sizeof goal_data_40,
  sizeof man_data_40,
  sizeof object_data_40,
  sizeof saveman_data_40,
  sizeof treasure_data_40
};

HiRes::HiRes () {
  loadImages (imageData_40, imageSize_40);
}
