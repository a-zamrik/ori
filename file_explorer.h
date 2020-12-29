#ifndef ORI_FILE_EXPLORER_H
#define ORI_FILE_EXPLORER_H

#include "menu.h"

class FileExplorer : public Menu {

  using Menu::Menu;

  public:

   void load_explorer (const std::string &);

}; 

#endif
