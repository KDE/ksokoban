// Pov-ray image source for ksokoban
// created 1998-08-20 by Anders Widell  <d95-awi@nada.kth.se>

#include "colors.inc"
#include "stones.inc"

camera {
  orthographic
  location <0, 0, -50>
  look_at  <0, 0, 0>
  right    <1, 0, 0>
  up       <0, 1, 0>
}

//background { color rgb <10/256, 10/256, 10/256> }
background { color rgb <0, 0, 0> }

light_source {
  <-1000000/3, 1000000/3, -1000000> color rgb <0.625, 0.625, 0.625>
}
light_source {
  <-1000000/3, 1000000/3, -1000000> color rgb <0.625, 0.625, 0.625>
}

/*
light_source {
  <-100/3, 100/3, -100> White
}
*/

/*
light_source {
  <-10000/3, 10000/3, -10000>
  White
  cylinder
  radius 1
  falloff 15
  point_at <0,0,0>
}
*/

#declare Stone = object {
  superellipsoid {
    <1/3, 1/3>

    scale <1/2, 1/4, 1/4>
  }
  texture { T_Stone8 }
}

#declare HalfStone = object {
  superellipsoid {
    <1/2, 1/3>

    scale <1/4, 1/4, 1/4>
  }
  texture { T_Stone8 }
}
