// Pov-ray image source for ksokoban
// copyright (c) 1998-1999  Anders Widell  <awl@hem.passagen.se>

#include "colors.inc"

blob {
  threshold .5

  sphere   { <0, .4, 0>, .2, 1 pigment {Flesh} }                 // head


  sphere   { <-.04, .42, -.1>, .025, -1 pigment {Flesh} }      // left eye hole
  sphere   { < .04, .42, -.1>, .025, -1 pigment {Flesh} }      // right eye hole

  //sphere   { <-.04, .42, -.09>, .025, 1 pigment {Flesh} }      // left eye
  //sphere   { < .04, .42, -.09>, .025, 1 pigment {Flesh} }      // right eye



  cylinder { <0, .415, -.1>, <0, .4, -.11>, .02, 1 pigment {Flesh} }    // nose


  cylinder { <0, .4, 0>, <0, .2, 0>, .075, 1 pigment {Flesh} }    // neck
  sphere   { <0, .4, 0>, .05, -1            pigment {Flesh} }    // head-
  sphere   { <0, .2, 0>, .05, -1            pigment {Flesh} }    // shoulder-

  cylinder { <-.2,.2,0>, <.2,.2,0>, .115, .9 pigment {White} }     // shoulder
  cylinder { <-.1,.1,0>, <.1,.1,0>, .115, .9 pigment {White} }     // stomach
  cylinder { <-.1, 0,0>, <.1, 0,0>, .115, .9 pigment {White} }     // stomach
  cylinder { <-.1,-.05,0>, <.1,-.05,0>, .115, 1 pigment {White} }     // stomach
  cylinder { <-.1,-.05,0>, <.1,-.05,0>, .115, -1 pigment {White} }     // stomach
  cylinder { <-.1,-.1,0>, <.1,-.1,0>, .115, .9 pigment {Blue} }     // waist


// Left arm

  cylinder { <-.2,.2,0>, <-.3, 0,0>, .1, 1 pigment {White  } }
  sphere   { <-.2,.2,0>, .1, -1            pigment {White  } }    // shoulder-
  cylinder { <-.3,0,0>, <-.2,-.1,-.1>, .075, 1 pigment {Flesh  } }
  sphere   { <-.3,0,0>, .075, -1            pigment {Flesh  } }   // arm-


// Right arm

  cylinder { < .2,.2,0>, < .3, 0,0>, .1, 1 pigment {White}}
  sphere   { < .2,.2,0>, .1, -1            pigment {White}}       // shoulder-
  cylinder { < .3,0,0>, < .2,-.1,-.1>, .075, 1 pigment {Flesh}}
  sphere   { < .3,0,0>, .075, -1                pigment {Flesh}}  // arm-

  // left leg
  cylinder { <-.1,-.2,0>, <-.125,-.5,-.025>, .15, 1 pigment {Blue } }

  // right leg
  cylinder { < .1,-.2,0>, < .125,-.5,-.025>, .15, 1 pigment {Blue } }


  scale 0.95
  translate 0.01*y
}
