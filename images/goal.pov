// Pov-ray image source for ksokoban
// created 1998-08-20 by Anders Widell  <d95-awi@nada.kth.se>

#include "colors.inc"
#include "woods.inc"

camera {
  orthographic
  location <0, 0, -50>
  look_at  <0, 0, 0>
  right    <1, 0, 0>
  up       <0, 1, 0>
}

light_source { <-5000, 10000, -10000> color rgb <1, 1, 1> }
light_source { <-5000, 10000, -10000> color rgb <1, 1, 1> }

plane { -z, -2
  pigment { color rgb <1/2, 1/2, 1/2> }
    //texture { T_Wood35 rotate <90,0,0> }
    //pigment { checker color rgb <1/2, 1/2, 1/2> color rgb <0,0,1> scale 1/2 }
}

torus {
  0.35, 0.05
  rotate <90, 0, 0>
  translate <0,0,2>
  pigment { color rgb <1/2, 1/2, 1/2> }
}

cylinder {< 0.35,0,2>, < 10,0,2>, 0.05 pigment { color rgb <1/2,1/2,1/2>}}
cylinder {<-0.35,0,2>, <-10,0,2>, 0.05 pigment { color rgb <1/2,1/2,1/2>}}
cylinder {<0, 0.35,2>, <0, 10,2>, 0.05 pigment { color rgb <1/2,1/2,1/2>}}
cylinder {<0,-0.35,2>, <0,-10,2>, 0.05 pigment { color rgb <1/2,1/2,1/2>}}

sphere {
 2*z, 0.3

  texture {
    finish {
      ambient 0.1
      diffuse 0.3
      reflection .25
      refraction 1
      ior 2.4
      specular 1
      roughness 0.02
    }
  }
  pigment { color rgbf <0.1, 1, 0.2, 0.8>}

  scale <1,1,0.1>
}
