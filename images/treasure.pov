// Pov-ray image source for ksokoban
// created 1998-08-20 by Anders Widell  <d95-awi@nada.kth.se>

#include "shapes.inc"
#include "shapes2.inc"
#include "colors.inc"
#include "finish.inc"
#include "glass.inc"

#include "goal.pov"

object {
  intersection {
    plane {-z, 0.3 rotate < 30,    0, 0>}
    plane {-z, 0.3 rotate < 30,   60, 0>}
    plane {-z, 0.3 rotate < 30,  120, 0>} 
    plane {-z, 0.3 rotate < 30,  180, 0>}
    plane {-z, 0.3 rotate < 30,  240, 0>}
    plane {-z, 0.3 rotate < 30,  300, 0>}

    plane {-z, 0.3 rotate <-50,    0, 0>}
    plane {-z, 0.3 rotate <-50,   60, 0>}
    plane {-z, 0.3 rotate <-50,  120, 0>}
    plane {-z, 0.3 rotate <-50,  180, 0>}
    plane {-z, 0.3 rotate <-50,  240, 0>}
    plane {-z, 0.3 rotate <-50,  300, 0>}
  }

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
  pigment { color rgbf <1, 0.1, 0.2, 0.8>}

  translate <0, -0.1, 0>
}
