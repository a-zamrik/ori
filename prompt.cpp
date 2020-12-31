#include "prompt.h"

unsigned Prompt::command_left () {
  if (this->curr_entry != this->entries.begin ()) {
    this->curr_entry->deselect ();
    this->curr_entry--;
    this->curr_entry->select ();
    this->curr_entry_num--;
  }
}

unsigned Prompt::command_right () {
  if (this->curr_entry != --this->entries.end ()) {
    this->curr_entry->deselect ();
    this->curr_entry++;
    this->curr_entry->select ();
    this->curr_entry_num++;
  }
}

unsigned Prompt::command_enter () {
  return this->curr_entry->get_ori_op ();
}

void Prompt::render () {
  std::list<Menu::MenuEntry>::iterator entry = this->entries.begin ();
  /* move cursor to start of text box */
  unsigned curr_row = this->entries_row_offset;  // curr_row relative to view_port
  unsigned curr_col = this->entries_col_offset;
  printf ("\033[%u;%uH", this->entries_row_offset, this->entries_col_offset);

  /* print MenuHeader */
  printf ("\033[38;2;246;246;246m\033[48;2;80;80;100m%-*s", 
      this->width, 
      this->title.c_str ());
  curr_row++;


  /* mount default color */
  printf ("%s", this->default_entry_color.c_str ());

  /* for every entry; render it in entry list section */
  while (entry != this->entries.end () && curr_row < this->entries_height + this->row_anchor) {
    if (entry->is_selected ()) {
      printf ("%s", this->selected_entry_color.c_str ());
      printf ("\033[5m");
      entry->draw (curr_row, curr_col);
      printf ("%s", this->default_entry_color.c_str ());
      printf ("\033[0m");
    } else {
      entry->draw (curr_row, curr_col);
    }
    curr_col += entry->get_width ();
    entry++;
  }

  /* print footer */
  printf ("\033[38;2;246;246;246m\033[48;2;80;80;100m\033[%u;%uH%*s",
          this->row_anchor + this->entries_height,
          this->col_anchor,
          this->width,
          "");

  /* reset console to default colors */
  printf ("\033[0m"); 

}
