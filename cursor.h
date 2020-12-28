#ifndef ORI_CURSOR_H
#define ORI_CURSOR_H

#include <list>
#include "line.h"
#include "menu.h"

struct cursor {
  unsigned row;
  unsigned col;
  std::list<Line>::iterator line;
  std::list<Menu::MenuEntry>::iterator entry;
};

#endif
