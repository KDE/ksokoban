// Pov-ray image source for ksokoban
// created 1998-08-20 by Anders Widell  <d95-awi@nada.kth.se>

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
}
