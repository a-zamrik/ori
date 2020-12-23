#include <stdio.h>
#include <iostream>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#include "line.h"
#include "text_box.h"

#define DEBUG

static void initialize (void);
static bool user_input (void);
static void render (void);

/* holds rows and coloumns of console */
struct winsize view_port;
TextBox* text_box = NULL;

struct cursor cursor;

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

  /* TODO: FREE THIS */
  text_box = new TextBox (4, 2, view_port.ws_col - 4,  view_port.ws_row - 2);
  text_box->add_line (strdup ("Howdy There"));
  text_box->add_line (strdup ("Up Down"));

  cursor.line = text_box->begin ();
  cursor.row = 2;
  cursor.col = 5;
 

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

static bool user_input () {

  char c;
  switch ((c = getchar ())) {
    /* Escape sequence given */
    case '\033':
      if ((c = getchar ()) == '[') {
        switch (getchar ()) {
          case 'C': // right
            if (cursor.col < cursor.line->length() + text_box->get_col_offset () + 1)
              cursor.col++;
            break;
          case 'D': // left
            if (cursor.col > text_box->get_col_offset () + 1)
              cursor.col--;
            break;
          case 'A': // up
            if (cursor.line != text_box->begin ()) {
              cursor.row--;
              cursor.line--;
            }
            break;
          case 'B': // down
            if (++cursor.line != text_box->end ()) {
              cursor.row++;
            } else {
              cursor.line--;
            }
            break;
        }

        /* push back cursor to end of line when moving up or down into
         * no mans land */
        if (cursor.col - text_box->get_col_offset () > cursor.line->length()) {
          cursor.col = text_box->get_col_offset () + cursor.line->length() + 1;
        }


      }
      else
        return false;
      break;

    case '\n':
      text_box->command_new_line (cursor);
      break;

    case '\177': /* backspace */
      text_box->command_backspace (cursor);
      break;

    default:
      cursor.line->insert_char (c, cursor.col - text_box->get_col_offset () - 1);
      cursor.col++;
      return true;
  }

  /* if cursor needs to scroll text box down */
  if (cursor.row + 1 > text_box->get_row_offset () + text_box->get_length ()) {
    text_box->inc_view_port_offset ();
    cursor.row--;
  }
  
  return true;
}

static void render () {


  printf ("\033[0;0H");  /* move cursor to top left cornor */

  /* Header. Top Bar */
  printf("\033[38;2;40;40;0m\033[48;2;175;246;199m%sR:%3d C:%3d%*s\n", "[ Ori ]",cursor.row, cursor.col, view_port.ws_col - 18, " ~ ");
  printf("\033[0m");

  /* TODO: rember to use \r */


  /* Move to lines that are in view port */
  std::list<Line>::iterator line = text_box->begin ();
  int j;
  for (j = 0; j < text_box->get_view_port_offset (); j++)
    line++;
  
  /* Render lines with text */
  int i = j;
  for (i += 1; i < view_port.ws_row + j && line != text_box->end (); i++) {
    printf("\033[48;2;175;246;199m\033[48;2;40;40;0m%2d%s%s%*s\n",
        i,
        "| ",
        line->get_str (),
        view_port.ws_col - line->length () - text_box->get_col_offset (),
        "[ ]");
    line++;
  }

  /* Render remaining empty lines */
  for (i -= j; i < view_port.ws_row - 1; i++) {
    printf("\033[48;2;175;246;199m\033[48;2;40;40;0m%2c%s%*s", '*',"| ", view_port.ws_col - 4, "[ ]");
    printf("\033[%u;%uH", i - j + 1, 0);
  }

  //printf("\033[48;2;175;246;199m\033[48;2;40;40;0m%2c%s%*s", '*',"| ", view_port.ws_col - 4, "[ ]");
  printf("\033[0m"); 

  /* reset cursor to where user wanted it */
  printf ("\033[%u;%uH", cursor.row, cursor.col);

}

