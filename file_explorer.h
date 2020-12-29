#ifndef ORI_FILE_EXPLORER_H
#define ORI_FILE_EXPLORER_H

#include "menu.h"
#include "text_box.h"

class FileExplorer : public Menu {

  using Menu::Menu;

  public:

   void load_explorer (const std::string &);
   void load_preview ();
   void destroy_preview ();

}; 

#endif
