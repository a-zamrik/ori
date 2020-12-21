#include <stdio.h>
#include <iostream>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#include "list.h"
#include "line.h"

#define DEBUG

static void initialize (void);
static bool user_input (void);
static void render (void);

/* holds rows and coloumns of console */
struct winsize view_port;
LinkedList<Line> lines;

struct cursor {
  unsigned row;
  unsigned col;
  unsigned col_offset;
  Line *line;
} cursor;

int main () {
  initialize ();
  render ();

  while (user_input ())
    render ();
  
  return 0;
}

static void initialize () {
  /* get view_port size */
  if (-1 == ioctl (STDOUT_FILENO, TIOCGWINSZ, &view_port)) {
    std::cout << "view_port init failed" << strerror (errno) << std::endl;
    exit (-1);
  }

  lines.append (new Line (strdup ("Howdy There")));
  lines.append (new Line (strdup ("Up Down")));

  cursor.row = 2;
  cursor.col = 4;
  cursor.col_offset = 4;
  cursor.line = lines.next ();

  /* TODO: add error handling */
  /* Disables stdin echo and buffered I/O */
  struct termios t;
  tcgetattr (STDIN_FILENO, &t);
  t.c_lflag &= (~ICANON & ~ECHO);
  tcsetattr (STDIN_FILENO, TCSANOW, &t);


#ifdef DEBUG
  std::cout << "row:" << view_port.ws_row << " col:" << view_port.ws_col << std::endl;
  lines.print();
#endif
}

static bool user_input () {
  
  char c;
  switch ((c = getchar ())) {
    /* Escape sequence given */
    case '\033':
      if ((c = getchar ()) == '[') {
        switch (getchar ()) {
          case 'C': // right
            cursor.col++;
            break;
          case 'D': // left
            cursor.col--;
            break;
          case 'A': // up
            cursor.row--;
            cursor.line = lines.prev ();
            break;
          case 'B': // down
            cursor.row++;
            cursor.line = lines.next ();
            break;
        }
      }
      else
        return false;
      break;

    default:
      cursor.line->insert_char (c, cursor.col - cursor.col_offset - 1);
      cursor.col++;
      return true;
  }
  return true;
}

static void render () {


  printf ("\033[0;0H");  /* move cursor to top left cornor */

  /* Header. Top Bar */
  printf("\033[38;2;40;40;0m\033[48;2;175;246;199m%s%*s\n", "[ Ori ]", view_port.ws_col - 7, " ~ ");
  printf("\033[0m");

  /* TODO: rember to use \r */

  
  /* Render lines with text */
  Line *line = NULL;
  lines.reset_cursor ();
  int i;
  for (i = 1; i < view_port.ws_row && (line = lines.next()); i++) {
    printf("\033[48;2;175;246;199m\033[48;2;40;40;0m%2d%s%s%*s\n",
        i,
        "| ",
        line->get_str (),
        view_port.ws_col - line->length () - cursor.col_offset,
        "[ ]");
  }

  /* Render remaining empty lines */
  for (; i < view_port.ws_row - 1; i++) {
    printf("\033[48;2;175;246;199m\033[48;2;40;40;0m%2c%s%*s\n", '*',"| ", view_port.ws_col - 4, "[ ]");
  }
  printf("\033[48;2;175;246;199m\033[48;2;40;40;0m%2c%s%*s", '*',"| ", view_port.ws_col - 4, "[ ]");
  printf("\033[0m"); 
  
  /* reset cursor to where user wanted it */
  printf ("\033[%u;%uH", cursor.row, cursor.col);
  lines.go_to (cursor.row);

}

