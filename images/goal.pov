// Pov-ray image source for ksokoban
// copyright (c) 1998-1999  Anders Widell  <awl@hem.passagen.se>

#include "floor_common.inc"

torus {
  0.35, 0.05
  rotate <90, 0, 0>
  translate <0,0,2>
  pigment { color rgb FloorColour }
}

cylinder {< 0.35,0,2>, < 10,0,2>, 0.05 pigment { color rgb FloorColour}}
cylinder {<-0.35,0,2>, <-10,0,2>, 0.05 pigment { color rgb FloorColour}}
cylinder {<0, 0.35,2>, <0, 10,2>, 0.05 pigment { color rgb FloorColour}}
cylinder {<0,-0.35,2>, <0,-10,2>, 0.05 pigment { color rgb FloorColour}}

sphere {
  2*z, 0.3

  finish {
    ambient 0.1
    diffuse 0.3
    reflection .25
    specular 1
    roughness 0.02
  }
  interior {
    ior 2.4
  }
  pigment { color rgbf <0.1, 1, 0.2, 0.8>}

  scale <1,1,0.1>
}
