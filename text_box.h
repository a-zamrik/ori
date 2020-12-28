#ifndef ORI_TEXT_BOX_H
#define ORI_TEXT_BOX_H

#include "line.h"
#include "ori_entity.h"
#include "keybinding.h"
#include <list>
#include <assert.h>
#include <fstream>


struct cursor {
  unsigned row;
  unsigned col;
  std::list<Line>::iterator line;
};



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

    bool load_file (std::string const&file_name) {
      this->file_name = file_name;
      std::string line;
      std::ifstream working_file (file_name);

      if (working_file.is_open ()) {
        while (getline (working_file, line)) {
          this->add_line (line);
        }
        working_file.close ();
        return true;
      }
      return false;
    }

    bool write_file () {
      std::list<Line>::iterator it = lines.begin ();
      std::ofstream working_file (file_name);

      assert (working_file.is_open ());

      /* TODO: add error handling */
      for (it = lines.begin (); it != lines.end (); it++) {
        working_file << it->get_str_obj () << std::endl;
      }

      working_file.close ();
    }

    void command_new_line (struct cursor &cursor) {
      /* TODO: make new line and move over contents if needed */

      this->insert_line (++cursor.line, 
          Line (cursor.line->substr (cursor.col - this->text_col_offset, cursor.line->length ())));
      cursor.line--;

      /* Clip off/delete string from previous line to "move" it */
      cursor.line--;
      cursor.line->clip (cursor.col - this->text_col_offset);
      cursor.line++;

      if (cursor.col != this->text_col_offset || !cursor.line->length ())
        cursor.line->set_mark ('+');
      cursor.col = this->text_col_offset;
      cursor.row++;
    }

    void command_backspace (struct cursor &cursor) {
      /* backspace at start of line; append line to line above */
      if (cursor.col == this->text_col_offset) {

        /* must have atleast one line */
        if (cursor.line == this->begin ())
          return;



        std::list<Line>::iterator to_be_removed = cursor.line;
        cursor.line--;

        /* update cursor */
        cursor.col = this->text_col_offset + cursor.line->length ();

        /* move cursor up if deleting at start of text; scroll if needed */
        if (cursor.row == this->text_row_offset) {
          this->scroll_offset--;
        } else {
          cursor.row--;
        }

        /* modify lines */
        if (to_be_removed->length()) {
          cursor.line->append (to_be_removed);
        }
        this->remove_line (to_be_removed);
        return;
      }


      /* deleteing just char or entire line */
      if (cursor.line->length () > 0) {
        cursor.col--;
        cursor.line->delete_char (cursor.col - this->text_col_offset);
      } else {
        /* must have atleast one line on display */
        if (this->lines.size () > 1) {
          this->remove_line (cursor.line++);
          if (cursor.row > this->text_row_offset)
            cursor.row--;
        }
      }
    }

    bool command_down (struct cursor &cursor) {
      bool result = false;
      /* TODO: should be able to scroll down to a full page of empty lines */
      if (++cursor.line != this->end ()) {
        cursor.row++;
        result = true;
      } else {
        cursor.line--;
      }

      return result;
    }

    bool command_up (struct cursor &cursor) {
      if (cursor.line != this->begin ()) {
        cursor.row--;
        cursor.line--;
        return true;
      }
      return false;
    }

    void clear_marked_lines () {
      if (marked_lines.size () > 0) {
        std::list<Line*>::iterator it;
        for (it = marked_lines.begin (); it != marked_lines.end ();) {
          (*it)->set_mark (' ');
          marked_lines.erase (it++);
        }
      }

    }



  public:
    /* _COL_OFFSET and _ROW_OFFSET is the top left start of where the text
     * will be located.
     * _WIDTH and _LENGTH stores the length and width of text box.
     */
    TextBox (unsigned _col_offset, unsigned _row_offset, unsigned _width, unsigned _length) {
      /* stdout does not use 0 based indexing. Using zero based indexing
       * will break rendering */
      assert (_col_offset > 0);
      assert (_row_offset > 0);

      this->text_col_offset = _col_offset;
      this->text_row_offset = _row_offset;
      this->row_anchor = _row_offset;
      this->col_anchor = _col_offset;
      this->width = _width;
      this->length = _length;
      this->scroll_offset = 0;
      this->jump_dist = _length / 2;
    }

    TextBox (unsigned _col_offset, unsigned _row_offset, unsigned _width,
        unsigned _length, const std::string &file_name) {
      *this = TextBox (_col_offset, _row_offset, _width, _length);
      this->load_file (file_name);
    }


    void mount_cursor (struct cursor &cursor) {
      cursor.row = this->text_row_offset;
      cursor.col = this->text_col_offset;
      cursor.line = this->begin ();
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

    void insert_line (std::list<Line>::iterator it, Line line) {
      lines.insert (it, line);
    }

    void enable_line_number (struct cursor &cursor) {
      if (this->numbered_lines)
        return;

      this->numbered_lines = true;
      this->text_col_offset += 5;
      cursor.col += 5;
    }

    void disable_line_number (struct cursor &cursor) {
      if (!this->numbered_lines)
        return;

      this->numbered_lines = false;
      this->text_col_offset -= 5;
      cursor.col -= 5;
    }

    void do_command (struct cursor &cursor, unsigned command, char c) {

      switch (command) {
        case UP:
          this->command_up (cursor);
          break;
        case DOWN:
          this->command_down (cursor);
          break;
        case LEFT:
          if (cursor.col > this->text_col_offset)
            cursor.col--;
          break;

        case RIGHT:
          if (cursor.col < cursor.line->length() + this->text_col_offset)
            cursor.col++;
          break;

        case PGUP:
          for (int i = 0; i < this->jump_dist && this->command_up (cursor); i++) {
            if (cursor.row < this->text_row_offset) {
              this->scroll_offset--;
              cursor.row++;
            }
          }
          break;

        case PGDOWN:
          for (int i = 0; i < this->jump_dist && this->command_down (cursor); i++) {
            if (cursor.row > this->text_row_offset + this->length) {
              this->scroll_offset++;
              cursor.row--;
            }
          }
          break;

        case ENTER:
          this->command_new_line (cursor);
          break;
        case BACKSPACE:
          this->command_backspace (cursor);
          break;
        case CTRL_L:
          if (this->numbered_lines)
            this->disable_line_number (cursor);
          else
            this->enable_line_number (cursor);
          break;


        case CTRL_W:
          this->write_file ();
          break;
        default: // command = TEXT
          this->marked_lines.push_back (&(*cursor.line));
          cursor.line->insert_char (c, cursor.col - this->text_col_offset);
          cursor.col++;
      }

      /* push back cursor to end of line when moving up or down into
       * no mans land */
      if (cursor.col - this->text_col_offset > cursor.line->length()) {
        cursor.col = this->text_col_offset + cursor.line->length();
      }

      /* if cursor needs to scroll text box down */
      if (cursor.row > this->text_row_offset + this->length) {
        this->scroll_offset++;
        cursor.row--;
      }

      if (cursor.row < this->text_row_offset) {
        this->scroll_offset--;
        cursor.row++;
      }
    }


    void render () {
      /* Move to lines that are in view port */
      std::list<Line>::iterator line = this->begin ();
      std::list<Line>::iterator ref_line;
      int j;
      for (j = 0; j < this->scroll_offset; j++)
        line++;
      ref_line = line;

      /* move cursor to start of text box */
      unsigned curr_row = this->text_row_offset;  // curr_row relative to view_port
      printf ("\033[%u;%uH", this->text_row_offset, this->text_col_offset);

      /* mount colors */
      printf ("\033[48;2;175;246;199m\033[48;2;40;40;0m");

      /* print numbered lines */
      if (this->numbered_lines) {
        for (int k = j; k < this->scroll_offset + this->text_row_offset + this->length - 1 && line != this->end (); k++) {
          printf ("\033[%u;%uH", curr_row, this->col_anchor);
          printf ("%3d%s", k + 1, "| ");
          curr_row++;
          line++;
        }
        line = ref_line;
        curr_row = this->text_row_offset;
      }


      /* print lines with text within bounds */
      for (int i = j; i < this->scroll_offset + this->text_row_offset + this->length - 1 && line != this->end (); i++) {
        printf ("\033[%u;%uH", curr_row, this->text_col_offset);
        printf("%s%*s",
            line->get_str (),
            this->width - line->length () - this->text_col_offset,
            line->get_mark ());
        line++;
        curr_row++;
      }

      /* print empty lines */
      if (this->numbered_lines) {
        /* print empty lines within bounds if room */
        for (; curr_row <= this->text_row_offset + this->length; curr_row++) {
          printf ("\033[%u;%uH", curr_row, this->col_anchor);
          printf ("%3c%s%*s",
              '*',
              "|",
              this->width - this->text_col_offset + 1,
              " ~ ");
        }
      } else {
        for (; curr_row <= this->text_row_offset + this->length; curr_row++) {
          printf ("\033[%u;%uH", curr_row, this->text_col_offset);
          printf ("%*s",
              this->width - this->text_col_offset,
              " ~ ");
        }
      }

      /* reset console to default colors */
      printf ("\033[0m"); 

    }

};

#endif
