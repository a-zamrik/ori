#include "menu.h"
#include "keybinding.h"
#include <assert.h>
#include "cursor.h"

Menu::Menu (unsigned _col_offset, unsigned _row_offset, unsigned _width,
            unsigned _length) {
  /* stdout does not use 0 based indexing. Using zero based indexing
   * will break rendering */
  assert (_col_offset > 0);
  assert (_row_offset > 0);

  this->entries_col_offset = _col_offset;
  this->entries_row_offset = _row_offset;
  this->row_anchor = _row_offset;
  this->col_anchor = _col_offset;
  this->width = _width;
  this->length = _length;
  this->scroll_offset = 0;
  this->jump_dist = _length / 2;

  this->entries_width = _width / 4;
  this->entries_height = _length;
  this->title = "MENU TITLE PLACE HOLDER";
  this->default_entry_color = "\033[48;2;175;246;199m\033[48;2;0;0;0m";
  this->selected_entry_color = "\033[48;2;175;246;199m\033[48;2;50;20;50m";
}

void Menu::add_entry (MenuEntry entry) {
  this->entries.push_back (entry);
}

void Menu::render () {
  std::list<Menu::MenuEntry>::iterator entry = this->entries.begin ();
  /* move cursor to start of text box */
  unsigned curr_row = this->entries_row_offset;  // curr_row relative to view_port
  printf ("\033[%u;%uH", this->entries_row_offset, this->entries_col_offset);

  /* print MenuHeader */
  printf ("\033[48;2;246;246;246m\033[48;2;80;80;100m%-*s", 
      this->width, 
      this->title.c_str ());
  curr_row++;

  /* Account for scroll offset */
  for (int i = 0; i < this->scroll_offset; i++) 
    entry++;

  /* mount default color */
  printf ("%s", this->default_entry_color.c_str ());

  /* for every entry; render it in entry list section */
  while (entry != this->entries.end () && curr_row < this->entries_height + this->row_anchor) {
    if (entry->is_selected ()) {
      printf ("%s", this->selected_entry_color.c_str ());
      entry->draw (curr_row, this->col_anchor);
      printf ("%s", this->default_entry_color.c_str ());
    } else {
      entry->draw (curr_row, this->col_anchor);
    }
    curr_row += entry->get_height ();
    entry++;
  }

  /* reset console to default colors */
  printf ("\033[0m"); 
}

void Menu::command_up (struct cursor &cursor) {
  if (this->curr_entry != this->entries.begin ()) {
    this->curr_entry->deselect ();
    this->curr_entry--;
    this->curr_entry->select ();
    this->curr_entry_num--;

    if (this->curr_entry_num < this->scroll_offset)
      this->scroll_offset--;
 
  }
}

void Menu::command_down (struct cursor &cursor) {
  if (this->curr_entry != --this->entries.end ()) {
    this->curr_entry->deselect ();
    this->curr_entry++;
    this->curr_entry->select ();
    this->curr_entry_num++;

    unsigned max_entries_in_view = this->entries_height / this->entries.begin ()->get_height ();
    /* -1 accouns for header height */
    if (this->curr_entry_num - this->scroll_offset > max_entries_in_view - 1)
      this->scroll_offset++;
  }
}

void Menu::do_command (struct cursor &cursor, unsigned command, char c) {
  switch (command) {
    case UP:
      this->command_up (cursor);
      break;
    case DOWN:
      this->command_down (cursor);
      break;
    case LEFT:
      break;
    case RIGHT:
      break;
    case ENTER:
      break;

    default:
      break;
  }
}

void Menu::mount_cursor (struct cursor &cursor) {

  this->curr_entry = this->entries.begin ();
  this->curr_entry->select ();
  this->curr_entry_num = 0;
  this->scroll_offset = 0;

}

struct cursor& Menu::unmount_cursor (struct cursor &cursor) {

  this->curr_entry->deselect ();
  return cursor;

}


/*  
 *  ----------------
 *
 *  MenuEntry Below;
 *  
 *  ----------------
 */

Menu::MenuEntry::MenuEntry (unsigned _width, unsigned _height, const std::string &_text) {
  this->width = _width;
  this->height = _height;
  this->text = _text;
  this->selected = false;
}

bool Menu::MenuEntry::is_selected () {
  return this->selected;
}

unsigned  Menu::MenuEntry::get_height () {
  return this->height;
}

unsigned  Menu::MenuEntry::get_width () {
  return this->width;
}

void  Menu::MenuEntry::draw (unsigned row_offset, unsigned col_offset) {
  printf ("\033[%u;%uH", row_offset, col_offset);
  for (int i = 0; i < this->height / 2; i++) {
    printf ("%*s", this->width, " ");
      printf ("\033[%u;%uH", ++row_offset, col_offset);
  }

  printf ("%s %-*s", this->mark.c_str (), this->width - 4, this->text.c_str ());

  for (int i = 0; i < this->height / 2; i++) {
    printf ("\033[%u;%uH", ++row_offset, col_offset);
    printf ("%*s", this->width, " ");
  }
}

void Menu::MenuEntry::select () {
  this->selected = true;
  this->mark[1] = '*';
}

void Menu::MenuEntry::deselect () {
  this->selected = false;
  this->mark[1] = ' ';
}

