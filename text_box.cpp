#include "text_box.h"
#include "prompt.h"
#include "line.h"
#include "ori_entity.h"
#include "ori_entity_manager.h"
#include "keybinding.h"
#include "ori_codes.h"
#include "cursor.h"
#include <list>
#include <string>


bool TextBox::load_file (std::string const&file_name) {

  bool unsaved_changes = false;
  for (auto& line : this->lines) {
      if (line.get_mark ()[1] == '+') {
        unsaved_changes = true;
        break;
      }
  }
  if (unsaved_changes || this->lines.size () != this->original_lines_size) {
    this->aux_prompt = load_ok_prompt ("Please save current file first");
    return false;
  }


  this->file_name = file_name;
  std::string line;
  std::ifstream working_file (file_name);

  if (working_file.is_open ()) {
    this->lines.clear ();
    this->cursor.row = this->text_row_offset;
    this->cursor.col = this->text_col_offset;
    this->scroll_offset = 0;
    while (getline (working_file, line)) {
      this->add_line (line);
    }
    this->curr_line = this->lines.begin ();
    working_file.close ();
    
    /* TODO: this causes screen to flash */
    this->render ();
    this->original_lines_size = this->lines.size ();
    return true;
  }
  return false;
}

bool TextBox::write_file () {
  std::list<Line>::iterator it = lines.begin ();
  std::ofstream working_file (file_name);

  assert (working_file.is_open ());

  /* TODO: add error handling */
  for (it = lines.begin (); it != lines.end (); it++) {
    working_file << it->get_str_obj () << std::endl;
  }

  working_file.close ();
  for (auto& line : this->lines) {
    line.set_mark (' ');
  }

  this->original_lines_size = this->lines.size ();

  assert (this->aux_prompt == NULL);
  std::string prompt_title = std::to_string (lines.size ());
  prompt_title += " lines written to " + this->file_name;
  this->aux_prompt = load_ok_prompt (prompt_title);
}

unsigned TextBox::command_enter () {
  /* TODO: make new line and move over contents if needed */

  this->insert_line (++this->curr_line, 
      Line (this->curr_line->substr (this->cursor.col - this->text_col_offset, this->curr_line->length ())));
  this->curr_line--;

  /* Clip off/delete string from previous line to "move" it */
  this->curr_line--;
  this->curr_line->clip (this->cursor.col - this->text_col_offset);
  this->curr_line++;

  if (this->cursor.col != this->text_col_offset || !this->curr_line->length ())
    this->curr_line->set_mark ('+');
  this->cursor.col = this->text_col_offset;
  this->cursor.row++;
}

unsigned TextBox::command_backspace () {
  /* backspace at start of line; append line to line above */
  if (this->cursor.col == this->text_col_offset) {

    /* must have atleast one line */
    if (this->curr_line == this->begin ())
      return 0;



    std::list<Line>::iterator to_be_removed = this->curr_line;
    this->curr_line--;

    /* update cursor */
    this->cursor.col = this->text_col_offset + this->curr_line->length ();

    /* move cursor up if deleting at start of text; scroll if needed */
    if (this->cursor.row == this->text_row_offset) {
      this->scroll_offset--;
    } else {
      this->cursor.row--;
    }

    /* modify lines */
    if (to_be_removed->length()) {
      this->curr_line->append (to_be_removed);
    }
    this->remove_line (to_be_removed);
    return 0;
  }


  /* deleteing just char or entire line */
  if (this->curr_line->length () > 0) {
    this->cursor.col--;
    this->curr_line->delete_char (this->cursor.col - this->text_col_offset);
  } else {
    /* must have atleast one line on display */
    if (this->lines.size () > 1) {
      this->remove_line (this->curr_line++);
      if (this->cursor.row > this->text_row_offset)
        this->cursor.row--;
    }
  }
}

unsigned TextBox::command_down () {
  /* TODO: should be able to scroll down to a full page of empty lines */
  if (++this->curr_line != this->end ()) {
    this->cursor.row++;
  } else {
    this->curr_line--;
  }
}

unsigned TextBox::command_up () {
  if (this->curr_line != this->begin ()) {
    this->cursor.row--;
    this->curr_line--;
  }
}

void TextBox::clear_marked_lines () {
  if (marked_lines.size () > 0) {
    std::list<Line*>::iterator it;
    for (it = marked_lines.begin (); it != marked_lines.end ();) {
      (*it)->set_mark (' ');
      marked_lines.erase (it++);
    }
  }

}

/* _COL_OFFSET and _ROW_OFFSET is the top left start of where the text
 * will be located.
 * _WIDTH and _LENGTH stores the length and width of text box.
 */
TextBox::TextBox (unsigned _col_offset, unsigned _row_offset, unsigned _width, unsigned _length) {
  /* stdout does not use 0 based indexing. Using zero based indexing
   * will break rendering */
  assert (_col_offset > 0);
  assert (_row_offset > 0);

  this->text_col_offset = _col_offset;
  this->text_row_offset = _row_offset;
  this->cursor.col = this->text_col_offset;
  this->cursor.row = this->text_row_offset;
  this->row_anchor = _row_offset;
  this->col_anchor = _col_offset;
  this->width = _width;
  this->length = _length;
  this->scroll_offset = 0;
  this->jump_dist = _length / 2;
  this->curr_line = this->lines.begin ();

  OriEntity::set_text_color (175, 246, 199);
  OriEntity::set_background_color (40, 40, 0);
}

TextBox::TextBox (unsigned _col_offset, unsigned _row_offset, unsigned _width,
    unsigned _length, const std::string &file_name) {
  *this = TextBox (_col_offset, _row_offset, _width, _length);
  this->load_file (file_name);
  this->curr_line = this->lines.begin ();
}

/* Legacy */
void TextBox::mount_cursor () {
}

/* Legacy */
struct cursor & TextBox::unmount_cursor () {
  return this->cursor;
}

void TextBox::add_line (std::string str) {
  lines.push_back (Line (str));
}

void TextBox::remove_line (std::list<Line>::iterator it) {
  lines.erase (it);
}

std::list<Line>::iterator TextBox::begin () {
  return lines.begin ();
}

std::list<Line>::iterator TextBox::end () {
  return lines.end ();
}

void TextBox::insert_line (std::list<Line>::iterator it, Line line) {
  lines.insert (it, line);
}

void TextBox::enable_line_number () {
  if (this->numbered_lines)
    return;

  this->numbered_lines = true;
  this->text_col_offset += 5;
  this->cursor.col += 5;
}

void TextBox::disable_line_number () {
  if (!this->numbered_lines)
    return;

  this->numbered_lines = false;
  this->text_col_offset -= 5;
  this->cursor.col -= 5;
}

void TextBox::command_scroll_down () {
  if (this->curr_line != --this->lines.end ()) {
    this->scroll_offset++;
    if (cursor.row == this->text_row_offset) {
      this->curr_line++;
    } else {
      cursor.row--;
    }
  }
}

void TextBox::command_scroll_up () {
  if (this->curr_line != this->lines.begin () && this->scroll_offset) {
    this->scroll_offset--;
    if (cursor.row == this->text_row_offset + this->length) {
      this->curr_line--;
    } else {
      cursor.row++;
    }
  }

}

unsigned TextBox::do_command (unsigned command, char c) {

  if (this->aux_prompt) {
    if (this->aux_prompt->do_command (command, c) == ORI_DESTROY) {
      delete this->aux_prompt;
      this->aux_prompt = NULL;
    }
    return ORI_NO_OP;
  }

  switch (command) {
    case UP:
      this->command_up ();
      break;
    case DOWN:
      this->command_down ();
      break;
    case LEFT:
      if (cursor.col > this->text_col_offset)
        this->cursor.col--;
      break;

    case RIGHT:
      if (this->cursor.col < this->curr_line->length() + this->text_col_offset)
        this->cursor.col++;
      break;

    case PGUP:
      this->command_scroll_up ();
      break;

    case PGDOWN:
      this->command_scroll_down ();
      break;

    case ENTER:
      this->command_enter ();
      break;
    case BACKSPACE:
      this->command_backspace ();
      break;
    case CTRL_L:
      if (this->numbered_lines)
        this->disable_line_number ();
      else
        this->enable_line_number ();
      break;


    case CTRL_W:
      this->write_file ();
      break;
    default: // command = TEXT
      this->marked_lines.push_back (&(*this->curr_line));
      this->curr_line->insert_char (c, this->cursor.col - this->text_col_offset);
      this->cursor.col++;
  }

  /* push back cursor to end of line when moving up or down into
   * no mans land */
  if (this->cursor.col - this->text_col_offset > this->curr_line->length()) {
    this->cursor.col = this->text_col_offset + this->curr_line->length();
  }

  /* if cursor needs to scroll text box down */
  if (this->cursor.row > this->text_row_offset + this->length) {
    this->scroll_offset++;
    this->cursor.row--;
  }

  if (this->cursor.row < this->text_row_offset) {
    this->scroll_offset--;
    this->cursor.row++;
  }
  
  return 0;
}


void TextBox::render () {
  
  if (this->aux_prompt) {
    this->aux_prompt->render ();
    return;
  }

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
  printf ("%s%s", this->text_color.c_str (), this->background_color.c_str ());


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
  for (int i = j; i <= this->scroll_offset + this->length && line != this->end (); i++) {
    printf ("\033[%u;%uH", curr_row, this->text_col_offset);
    printf("%s%*s",
        line->get_str (),
        this->width - line->length () - (this->text_col_offset - this->col_anchor),
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
          this->width - (this->text_col_offset - this->col_anchor) + 1,
          " ~ ");
    }
  } else {
    for (; curr_row <= this->text_row_offset + this->length; curr_row++) {
      printf ("\033[%u;%uH", curr_row, this->text_col_offset);
      printf ("%*s",
          this->width - (this->text_col_offset - this->col_anchor),
          " ~ ");
    }
  }

  /* reset console to default colors */
  printf ("\033[0m"); 

}


