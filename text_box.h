#ifndef ORI_TEXT_BOX_H
#define ORI_TEXT_BOX_H

#include "line.h"
#include <list>

struct cursor {
  unsigned row;
  unsigned col;
  std::list<Line>::iterator line;
};

class TextBox {

  private:
    /* define the top left corner that TextBox is located */
    unsigned col_offset;
    unsigned row_offset;

    /* define the length and width of TextBox view port */
    unsigned width;
    unsigned length;

    /* Lines filled with text in TextBox */
    std::list<Line> lines;

    /* which line to start off in view port; used in rendering */
    unsigned scroll_offset;

  public:
    TextBox (unsigned _col_offset, unsigned _row_offset, unsigned _width, unsigned _length) {
      this->col_offset = _col_offset; 
      this->row_offset = _row_offset;
      this->width = _width;
      this->length = _length;
      this->scroll_offset = 0;
    }

    unsigned get_width () {
      return this->width;
    }

    unsigned get_length () {
      return this->length;
    }

    void inc_scroll_offset () {
      this->scroll_offset++;
    }

    void dec_scroll_offset () {
      this->scroll_offset--;
    }

    unsigned get_scroll_offset () {
      return this->scroll_offset;
    }

    void set_scroll_offset (unsigned offset) {
      this->scroll_offset = offset;
    }

    void add_line (std::string str) {
      lines.push_back (Line (str));
    }

    void remove_line (std::list<Line>::iterator it) {
      lines.erase (it);
    }

    std::list<Line>::iterator begin () {
      return lines.begin ();
    }

    std::list<Line>::iterator end () {
      return lines.end ();
    }

    unsigned get_col_offset () {
      return this->col_offset;
    }

    unsigned get_row_offset () {
      return this->row_offset;
    }

    void insert_line (std::list<Line>::iterator it, Line line) {
      lines.insert (it, line);
    }

    unsigned num_lines () {
      return lines.size ();
    }


    void command_new_line (struct cursor &cursor) {
      /* TODO: make new line and move over contents if needed */

      this->insert_line (++cursor.line, 
          Line (cursor.line->substr (cursor.col - this->col_offset - 1, cursor.line->length ())));
      cursor.line--;

      /* Clip off/delete string from previous line to "move" it */
      cursor.line--;
      cursor.line->clip (cursor.col - this->col_offset - 1);
      cursor.line++;

      cursor.col = this->col_offset + 1;
      cursor.row++;


    }

    void command_backspace (struct cursor &cursor) {
      /* backspace at start of line; append line to line above */
      if (cursor.col == this->col_offset + 1) {

        /* must have atleast one line */
        if (cursor.line == this->begin ())
          return;

        

        std::list<Line>::iterator to_be_removed = cursor.line;
        cursor.line--;

        /* update cursor */
        cursor.col = this->col_offset + cursor.line->length () + 1;
        
        if (cursor.row == this->row_offset) {
          this->scroll_offset--;
        } else {
          cursor.row--;
        }

        /* modify lines */
        cursor.line->append (to_be_removed);
        this->remove_line (to_be_removed);
        return;
      }


      if (cursor.line->length () > 0) {
        cursor.col--;
        cursor.line->delete_char (cursor.col - this->col_offset - 1);
      } else {
        /* must have atleast one line on display */
        if (this->num_lines () > 1) {
          this->remove_line (cursor.line++);
          if (cursor.row > 2)
            cursor.row--;
        }
      }
    }

    void render (unsigned view_port_width) {
      /* Move to lines that are in view port */
      std::list<Line>::iterator line = this->begin ();
      int j;
      for (j = 0; j < this->scroll_offset; j++)
        line++;

      /* move cursor to start of text box */
      unsigned curr_row = this->row_offset;  // curr_row relative to view_port
      printf ("\033[%u;%uH", this->row_offset, this->col_offset);

      /* mount colors */
      printf ("\033[48;2;175;246;199m\033[48;2;40;40;0m");


      /* print lines with text within bounds */
      for (int i = j; i < this->scroll_offset + this->row_offset + this->length - 1 && line != this->end (); i++) {
        printf ("\033[%u;0H", curr_row);
        printf("%2d%s%s%*s",
            i + 1,
            "| ",
            line->get_str (),
            view_port_width - line->length () - this->col_offset,
            "[ ]");
        line++;
        curr_row++;
      }

      /* print empty lines within bounds if room */
      for (; curr_row <= this->row_offset + this->length; curr_row++) {
        printf("\033[%u;0H%2c%s%*s",
            curr_row,
            '*',
            "| ",
            view_port_width - 4,
            "[ ]");
      }

      /* reset console to default colors */
      printf("\033[0m"); 



    }

};

#endif
