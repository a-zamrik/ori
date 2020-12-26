#include <fstream>
#include <iostream>
#include <list>
#include "text_box.h"
#include "line.h"


namespace loader {
  bool load_file (std::string const&, TextBox &, std::fstream &);
  void write (std::fstream, std::list<Line>);

  bool load_file (std::string const&file_name, TextBox &text_box, std::fstream &file) {
    std::string line;
    file.open (file_name, std::ios::out | std::ios::in);

    if (file.is_open ()) {
      while (getline (file, line)) {
        text_box.add_line (line);
      }
      return true;
    }

    return false;
  }

  void write (std::fstream file, std::list<Line> lines) {
    std::list<Line>::iterator it = lines.begin ();

    /* TODO: add error handling */
    for (it = lines.begin (); it != lines.end (); it++) {
      file << it->get_str_obj ();
    }
  }  
}


