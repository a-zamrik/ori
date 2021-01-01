#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>
#include "ori_entity_manager.h"

void OriEntityManager::init (TextBox &_text_box, FileExplorer &_file_explorer) {
  file_explorer = &_file_explorer;
  text_box = &_text_box;
}

FileExplorer & OriEntityManager::get_file_explorer () {
  return *file_explorer;
}

TextBox & OriEntityManager::get_text_box () {
  return *text_box;
}

void OriEntityManager::load_new_text (const std::string &file_name) {
  text_box->load_file (file_name);
}

const struct winsize OriEntityManager::initialize_window () {
/* get view_port size */
  if (-1 == ioctl (STDOUT_FILENO, TIOCGWINSZ, &view_port)) {
    std::cout << "view_port init failed" << strerror (errno) << std::endl;
    exit (-1);
  }

  /* TODO: add error handling */
  /* Disables stdin echo and buffered I/O */
  struct termios t;
  tcgetattr (STDIN_FILENO, &t);
  t.c_lflag &= (~ICANON & ~ECHO);
  tcsetattr (STDIN_FILENO, TCSANOW, &t);
  return view_port;
}

void OriEntityManager::clean_up () {
  delete text_box;
  delete file_explorer;
 
  /* clear screen */
  printf ("\033[0;1H");
  printf ("\033[0m\n");
  for (int i = 0; i < view_port.ws_row * view_port.ws_col; i++) {
    printf (" ");
  }
  printf ("\033[0;1H");
  
  /* reset terminal to as before */
  struct termios t;
  tcgetattr (STDIN_FILENO, &t);
  t.c_lflag |= (ICANON | ECHO);
  tcsetattr (STDIN_FILENO, TCSANOW, &t);

  fflush (stdout);
}

const struct winsize OriEntityManager::get_view_port () {
  return view_port;
}

Prompt * load_ok_prompt (const std::string &title) { 
  struct winsize view_port = OriEntityManager::get_view_port ();
  unsigned prompt_height = 4;
  unsigned title_length = title.length ();

  /* fine offset for propmt to be center of screen */
  unsigned row_offset = view_port.ws_row / 2;
  unsigned col_offset = view_port.ws_col / 2;
  col_offset -= title_length / 2;
  row_offset -= prompt_height  / 2;

  Prompt * prompt = new Prompt (col_offset, row_offset,
                                title_length, prompt_height, title);
  prompt->add_entry (Menu::MenuEntry (title_length, prompt_height -1,
                                      "OK", ORI_DESTROY));
  prompt->mount_cursor ();
  return prompt;
}

Prompt * load_unsaved_changes_prompt (const std::string &title) { 
  struct winsize view_port = OriEntityManager::get_view_port ();
  unsigned prompt_height = 4;
  unsigned title_length = title.length ();

  /* fine offset for propmt to be center of screen */
  unsigned row_offset = view_port.ws_row / 2;
  unsigned col_offset = view_port.ws_col / 2;
  col_offset -= title_length / 2;
  row_offset -= prompt_height  / 2;

  Prompt * prompt = new Prompt (col_offset, row_offset,
                                title_length, prompt_height, title);
  prompt->add_entry (Menu::MenuEntry (title_length / 2, prompt_height -1,
                                      "Yes", ORI_WRITE));
  prompt->add_entry (Menu::MenuEntry (title_length / 2, prompt_height -1,
                                      "No", ORI_DONT_WRITE));
  prompt->mount_cursor ();
  return prompt;
}
