#ifndef HTMLPRINTER_H
#define HTMLPRINTER_H

#include "LevelMap.h"

class HtmlPrinter
{
public:
    static void printHtml(const Map *lm);

private:
    static void wall(bool up, bool down, bool left, bool right);
    static void image(const char *name);
    static void empty();
    static void printSquare(const Map *lm, int x, int y);
};

#endif /* HTMLPRINTER_H */
