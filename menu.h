#ifndef ORI_MENU_H
#define ORI_MENU_H

#include <list>
#include <string>
#include "ori_entity.h"
#include "text_box.h"

class Menu : public OriEntity {

 public:
   class MenuEntry {
    private:
      unsigned width;
      unsigned height;
      bool selected;
      std::string text;
      std::string mark = "< >";
      unsigned ori_op;  /* what command to do upon return */

    public:
      MenuEntry (unsigned, unsigned, const std::string &text);
      MenuEntry (unsigned, unsigned, const std::string &text, unsigned);
      unsigned get_height ();
      unsigned get_width ();
      void draw (unsigned, unsigned);
      void select ();
      void deselect ();
      bool is_selected ();
      const std::string & get_str ();
      void set_mark (const std::string &);
      unsigned get_ori_op ();
   };

    Menu (unsigned _col_offset, unsigned _row_offset,
          unsigned _width, unsigned _length, const std::string &_title);
    void add_entry (MenuEntry);
    virtual void render (void);
    unsigned do_command (unsigned, char);
    void mount_cursor ();
    struct cursor & unmount_cursor ();

 protected:
    /* define the top left corner that TextBox is located */
    unsigned entries_col_offset;
    unsigned entries_row_offset;

    /* where the text box in its entirity is pinned at */
    unsigned row_anchor;
    unsigned col_anchor;

    /* define the length and width of TextBox view port */
    unsigned width;
    unsigned length;

    /* Menu title */
    std::string title;

    /* Colors for entries */
    std::string default_entry_color;
    std::string selected_entry_color;

    /* Lines filled with text in TextBox */
    std::list<MenuEntry> entries;

    /* which line to start off in view port; used in rendering */
    unsigned scroll_offset;

    /* rows to jump when using page up or down */
    unsigned jump_dist;

    /* TODO: not usded .
     * defines the width and height of a menu entry */
    unsigned entries_width;
    unsigned entries_height;

    /* the currently selected entry */
    std::list<MenuEntry>::iterator curr_entry;
    unsigned curr_entry_num = 0;

    /* 
     * Methods -----------------------
     */
    virtual unsigned command_up ();
    virtual unsigned command_down ();
    virtual unsigned command_pgup ();
    virtual unsigned command_pgdown ();
    void load_aux_preview (const std::string &);

    /* serves as an embed for menus */
    TextBox* aux_preview = NULL;
};

#endif
