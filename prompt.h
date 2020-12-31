#ifndef ORI_PROMPT_H
#define ORI_PROMPT_H

#include "menu.h"

class Prompt : public Menu {
  
  using Menu::Menu;
  
  public:

    void render ();
    unsigned command_left ();
    unsigned command_right ();
    unsigned command_enter ();
};

#endif
