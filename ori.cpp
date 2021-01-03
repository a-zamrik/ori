#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fstream>

#include "line.h"
#include "text_box.h"
#include "menu.h"
#include "file_explorer.h"
#include "keybinding.h"
#include "cursor.h"
#include "ori_entity_manager.h"
#include "prompt.h"
#include "ori_codes.h"
#include "key_word.h"

#include <ctime>

#define DEBUG

static void initialize (const std::string &);
static bool user_input (void);
static void render (void);

/* holds rows and coloumns of console */
TextBox* text_box = NULL;
FileExplorer* menu = NULL;
OriEntity* selectedEntity = NULL;

int main () {
  /* TODO: get cursor coords
   * stackoverflow.com/questions/5966903/how-to-get-mousemove-and-mouseclick-in-bash/5970472#5970472
   * */
  // TODO: This works!
  // fprintf (stderr, "\e[?1000h\e[?1006h\e[?1015");
  // while (1);
  initialize ("text.txt");
  render ();

  while (user_input ())
    render ();


  OriEntityManager::clean_up ();
  return 0;
}

static void initialize (const std::string &file_name) {

  init_key_word_map ();

  struct winsize view_port = OriEntityManager::initialize_window ();

  /* TODO: FREE THIS */
  text_box = new TextBox (1, 2, view_port.ws_col, 
      view_port.ws_row - 2, "text.txt");
  text_box->mount_cursor (); 
  selectedEntity = text_box;

  menu = new FileExplorer (5, 5, view_port.ws_col - 10, view_port.ws_row - 11, "Select a file to open");
  menu->load_explorer (".");
  OriEntityManager::init (*text_box, *menu);

}

/* TODO: user input should be managed by TextBox */
static bool user_input () {

  unsigned command;
  char c = getchar ();
  switch (c) {

    /* Escape sequence given */
    case '\033':
      if ((c = getchar ()) == '[') {
        switch (getchar ()) {
          case 'C': // right
            command = RIGHT;
            break;
          case 'D': // left
            command = LEFT;
            break;
          case 'A': // up
            command = UP;
            break;
          case 'B': // down
            command = DOWN;
            break;
          case '6': // page down
            command = PGDOWN;
            getchar ();
            break;
          case '5': // page up
            command = PGUP;
            getchar ();
            break;
        }
      } else
        return false;
      break;

    case '\n':
      command = ENTER;
      break;

    case '\177': /* backspace */
      command = BACKSPACE;
      break;

    case '\017': /* '^O' */
      if (selectedEntity == text_box) {
        selectedEntity->unmount_cursor ();
        menu->mount_cursor ();
        selectedEntity = menu;
      } else {
        selectedEntity->unmount_cursor ();
        text_box->mount_cursor ();
        selectedEntity = text_box;
      }
      return true;

    case '\f': /* '^L' */
      command = CTRL_L;
      break;

    case '\031': /* '^Y' */
      command = CTRL_Y;
      break;

    case '\020': /* '^P' */
      command = CTRL_P;
      break;

    case 0x17: /* '^w' */
      command = CTRL_W;
      break;

    case 0x08: /* '^H' */
      command = CTRL_H;
      break;

    case '\t': /* TAB */
      command = TAB;
      break;

    default: /* character given */
      command = TEXT;
      break;
  }

  if (selectedEntity->do_command (command, c) == ORI_SWAP_ENTITY) {
    if (selectedEntity == text_box) {
      selectedEntity->unmount_cursor ();
      menu->mount_cursor ();
      selectedEntity = menu;
    } else {
      selectedEntity->unmount_cursor ();
      text_box->mount_cursor ();
      selectedEntity = text_box;
    }
  }

  return true;
}

static void render () {

  static double time_to_render;
  std::clock_t start;
  start = std::clock ();

  struct cursor cursor = selectedEntity->get_cursor ();
  struct winsize view_port = OriEntityManager::get_view_port ();

  printf ("\033[0;0H");   /* move cursor to top left cornor */
  printf ("\e[?25l");     /* hide cursor */

  /* Header. Top Bar */
  printf("\033[38;2;40;40;0m\033[48;2;175;246;199m%sR:%3d C:%3d <Render Time: %6.3fms>%*s\n", "[ Ori ]",cursor.row, cursor.col, time_to_render, view_port.ws_col - 18 - 24, " ~ ");
  printf("\033[0m");

  selectedEntity->render ();

  /* Footer */
  /*
     printf("\033[%u;0H", view_port.ws_row);
     printf("\033[38;2;40;40;0m\033[48;2;175;246;199m[R:%3d C:%3d]%*s", cursor.row, cursor.col, view_port.ws_col - 13, " ~ ");
     */
  printf("\033[0m");

  /* reset cursor to where user wanted it; boolean needed since stdout is not zero index */
  printf ("\033[%u;%uH", cursor.row, cursor.col);

  printf ("\e[?25h");     /* show cursor */

  time_to_render = (std::clock () - start) / (double) (CLOCKS_PER_SEC / 1000);

}

