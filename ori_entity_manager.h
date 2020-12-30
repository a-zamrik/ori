#ifndef ORI_ORI_ENTITY_MANAGER_H
#define ORI_ORI_ENTITY_MANAGER_H

#include "text_box.h"
#include "file_explorer.h"
#include <string>

class OriEntityManager {

  public:
    static void init (TextBox &, FileExplorer &);
    static FileExplorer & get_file_explorer ();
    static TextBox & get_text_box ();
    static void load_new_text (const std::string &);

  private:
    static inline FileExplorer * file_explorer;
    static inline TextBox * text_box;


};

#endif
