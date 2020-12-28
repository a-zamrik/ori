#ifndef ORI_MENU_H
#define ORI_MENU_H

#include <list>
#include <string>
#include "ori_entity.h"

class Menu : public OriEntity {

 public:
   class MenuEntry {
    private:
      unsigned width;
      unsigned height;
      std::string text;
      std::string mark = "< >";

    public:
      MenuEntry (unsigned, unsigned, const std::string &text);
      unsigned get_height ();
      unsigned get_width ();
      void draw (unsigned, unsigned);
   };

   Menu (unsigned _col_offset, unsigned _row_offset, unsigned _width, unsigned _length);
    void add_entry (MenuEntry);
    void render (void);

 private:
    /* define the top left corner that TextBox is located */
    unsigned entries_col_offset;
    unsigned entries_row_offset;

    /* where the text box in its entirity is pinned at */
    unsigned row_anchor;
    unsigned col_anchor;

    /* define the length and width of TextBox view port */
    unsigned width;
    unsigned length;

    std::string title;

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

    /* TODO: add embed */


};

#endif
