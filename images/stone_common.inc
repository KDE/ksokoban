// Pov-ray image source for ksokoban
// copyright (c) 1998-1999  Anders Widell  <awl@hem.passagen.se>

#include "colors.inc"
#include "stones.inc"

#declare StoneTexture = T_Stone8

background { color rgb <0, 0, 0> }

light_source {
  <-1000000/3, 1000000/3, -1000000> color rgb <0.625, 0.625, 0.625>
}
light_source {
  <-1000000/3, 1000000/3, -1000000> color rgb <0.625, 0.625, 0.625>
}

#declare Stone = object {
  superellipsoid {
    <1/3, 1/3>

    scale <1/2, 1/4, 1/4>
  }
}

#declare HalfStone = object {
  superellipsoid {
    <1/2, 1/3>

    scale <1/4, 1/4, 1/4>
  }
}
