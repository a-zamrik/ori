#include <stdio.h>
#include <iostream>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fstream>

#include "line.h"
#include "text_box.h"
#include "menu.h"
#include "keybinding.h"
#include "cursor.h"

#define DEBUG

static void initialize (const std::string &);
static bool user_input (void);
static void render (void);

/* holds rows and coloumns of console */
struct winsize view_port;
TextBox* text_box = NULL;
Menu* menu = NULL;
OriEntity* selectedEntity = NULL;
std::fstream file;

struct cursor cursor;

int main () {
  initialize ("text.txt");
  render ();

  while (user_input ())
    render ();

  file.close ();
  return 0;
}

static void initialize (const std::string &file_name) {


  /* get view_port size */
  if (-1 == ioctl (STDOUT_FILENO, TIOCGWINSZ, &view_port)) {
    std::cout << "view_port init failed" << strerror (errno) << std::endl;
    exit (-1);
  }

  /* TODO: FREE THIS */
  text_box = new TextBox (1, 2, view_port.ws_col + 1, 
                          view_port.ws_row - 2, "text.txt");
  text_box->mount_cursor (cursor); 
  selectedEntity = text_box;

  menu = new Menu (5, 5, view_port.ws_col - 50, view_port.ws_row - 5);
  menu->add_entry (Menu::MenuEntry (30, 3, "HOW ARE YOU!"));
  menu->add_entry (Menu::MenuEntry (30, 3, "HOWDY!!!"));
  menu->add_entry (Menu::MenuEntry (30, 3, "IAM DOING GREAT"));


  /* TODO: add error handling */
  /* Disables stdin echo and buffered I/O */
  struct termios t;
  tcgetattr (STDIN_FILENO, &t);
  t.c_lflag &= (~ICANON & ~ECHO);
  tcsetattr (STDIN_FILENO, TCSANOW, &t);


#ifdef DEBUG
  std::cout << "row:" << view_port.ws_row << " col:" << view_port.ws_col << std::endl;
#endif
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
        
        menu->mount_cursor (selectedEntity->unmount_cursor (cursor));
        selectedEntity = menu;
      } else {
        text_box->mount_cursor (selectedEntity->unmount_cursor (cursor));
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

    default: /* character given */
      command = TEXT;
      break;
  }





  selectedEntity->do_command (cursor, command, c);
  return true;
}



static void render () {


  printf ("\033[0;0H");   /* move cursor to top left cornor */
  printf ("\e[?25l");     /* hide cursor */

  /* Header. Top Bar */
  printf("\033[38;2;40;40;0m\033[48;2;175;246;199m%sR:%3d C:%3d%*s\n", "[ Ori ]",cursor.row, cursor.col, view_port.ws_col - 18, " ~ ");
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

}

