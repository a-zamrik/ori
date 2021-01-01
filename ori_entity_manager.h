#ifndef ORI_ORI_ENTITY_MANAGER_H
#define ORI_ORI_ENTITY_MANAGER_H

#include "text_box.h"
#include "file_explorer.h"
#include <string>
#include "prompt.h"
#include <sys/ioctl.h>

class OriEntityManager {

  public:
    static void init (TextBox &, FileExplorer &);
    static FileExplorer & get_file_explorer ();
    static TextBox & get_text_box ();
    static void load_new_text (const std::string &);
    
    /* getes window size and sets up stdin to not be buffered or echo */
    static const struct winsize initialize_window ();
    static const struct winsize get_view_port ();

    static void clean_up ();

  private:
    static inline FileExplorer * file_explorer;
    static inline TextBox * text_box;
    static inline struct winsize view_port;

};


/* loads a simple prompt with a single entry that says "OK" */
Prompt * load_ok_prompt (const std::string &);
Prompt * load_unsaved_changes_prompt (const std::string &title);

#endif
