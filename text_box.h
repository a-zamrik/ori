#ifndef ORI_TEXT_BOX_H
#define ORI_TEXT_BOX_H

#include "line.h"
#include "ori_entity.h"
#include "keybinding.h"
#include "cursor.h"
#include <list>
#include <assert.h>
#include <fstream>
#include <string>


class TextBox : public OriEntity {

  private:
    /* define the top left corner that TextBox is located */
    unsigned text_col_offset;
    unsigned text_row_offset;

    /* where the text box in its entirity is pinned at */
    unsigned row_anchor;
    unsigned col_anchor;

    /* define the length and width of TextBox view port */
    unsigned width;
    unsigned length;

    /* Lines filled with text in TextBox */
    std::list<Line> lines;

    /* which line to start off in view port; used in rendering */
    unsigned scroll_offset;

    /* enable line numbers if true */
    bool numbered_lines = 0;

    /* rows to jump when using page up or down */
    unsigned jump_dist;

    std::list<Line*> marked_lines;

    /* file associated with this TextBox */
    std::string file_name;

    /* Methods */
    bool load_file (std::string const&);
    bool write_file ();
    void command_new_line (struct cursor &);
    void command_backspace (struct cursor &);
    bool command_down (struct cursor &);
    bool command_up (struct cursor &);
    void clear_marked_lines ();

  public:
    TextBox (unsigned, unsigned, unsigned, unsigned);
    TextBox (unsigned _col_offset, unsigned _row_offset, unsigned _width,
             unsigned _length, const std::string &);
    void mount_cursor (struct cursor &);
    struct cursor & unmount_cursor (struct cursor &);
    void add_line (std::string);
    void remove_line (std::list<Line>::iterator);
    std::list<Line>::iterator begin ();
    std::list<Line>::iterator end ();
    void insert_line (std::list<Line>::iterator, Line);
    void enable_line_number (struct cursor &);
    void disable_line_number (struct cursor &);
    void do_command (struct cursor &, unsigned, char);
    void render (); 
};

#endif
