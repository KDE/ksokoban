// Pov-ray image source for ksokoban
// copyright (c) 1998-1999  Anders Widell  <awl@hem.passagen.se>

#include "stone_common.inc"

camera {
  orthographic
  location <0, 0, -50>
  look_at  <0, 0, 0>
  right    <1, 0, 0>
  up       <0, 1/2, 0>
}

object {
  Stone
  texture { StoneTexture translate <3,0,0> }
}
