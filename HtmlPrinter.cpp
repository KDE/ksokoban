#include <stdio.h>
#include <stdlib.h>
#include "HtmlPrinter.h"

void
HtmlPrinter::wall (bool up, bool down, bool left, bool right)
{
  switch ( (up!=0)           |
	   ((down!=0)  << 1) |
	   ((left!=0)  << 2) |
	   ((right!=0) << 3)) {
  case 0:
  case 1:
  case 2:
  case 3:
    image ("vertiwall");
    break;
  case 4:
  case 5:
  case 6:
  case 7:
    image ("eastwall");
    break;
  case 8:
  case 9:
  case 10:
  case 11:
    image ("westwall");
    break;
  case 12:
  case 13:
  case 14:
  case 15:
    image ("horizwall");
    break;

  default:
    abort ();
  }
}

void
HtmlPrinter::image (const char *name) {
  printf ("<td><img src=%s.gif width=40 height=40><br clear=all>\n", name);
}

void
HtmlPrinter::empty () {
  printf ("<td>\n");
}

void
HtmlPrinter::printSquare (LevelMap *lm, int x, int y) {
  if (lm->xpos () == x && lm->ypos () == y) {
    image (lm->goal (x, y) ? "saveman" : "man");
    return;
  }
  if (lm->empty (x, y)) {
    if (lm->floor (x, y)) {
      image (lm->goal (x, y) ? "goal" : "floor");
    } else {
      empty ();
    }
    return;
  }
  if (lm->wall (x, y)) {
    wall (lm->wallUp (x, y),
	  lm->wallDown (x, y),
	  lm->wallLeft (x, y),
	  lm->wallRight (x, y));
    return;
  }
  if (lm->object (x, y)) {
    image (lm->goal (x, y) ? "treasure" : "object");
    return;
  }
}

void
HtmlPrinter::printHtml (LevelMap *lm) {
  printf ("\
<html>\n\
<head>\n\
<title>ksokoban level</title>\n\
</head>\n\
<body background=background.gif>\n\
");
  printf ("<table border=0 cellspacing=0 cellpadding=0>\n");
  for (int y=0; y<lm->height(); y++) {
    printf ("<tr>\n");
    for (int x=0; x<lm->width(); x++) {
      printSquare (lm, x, y);
    }
  }
  printf ("\
</table>\n\
</body>\n\
</html>\n\
");
}
