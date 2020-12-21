#ifndef ORI_LINE_H
#define ORI_LINE_H

#include <iostream>
#include <string>

class Line {
  private:
    std::string text;
  
  public:
    Line (char*);
    Line (void);
    const char *get_str ();
    unsigned length ();
    void insert_char (char, unsigned);
};

Line::Line (char* _text) {
  this->text = _text;
}

Line::Line () {
  this->text = strdup ("/DEBUG/");
}

const char *Line::get_str () {
  return this->text.c_str();
}

unsigned Line::length () {
  return this->text.length ();
}

void Line::insert_char (char c, unsigned pos) {
  this->text.insert (pos, std::string (1, c));
}

#endif

