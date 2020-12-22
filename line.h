#ifndef ORI_LINE_H
#define ORI_LINE_H

#include <iostream>
#include <string.h>

class Line {
  private:
    std::string text;
  
  public:
    Line (char*);
    Line (void);
    Line (std::string);
    const char *get_str ();
    unsigned length ();
    void insert_char (char, unsigned);
    void delete_char (unsigned);
    void clip (unsigned);
    std::string substr (unsigned, unsigned);
};

Line::Line (char* _text) {
  this->text = _text;
}

Line::Line (std::string _text) {
  this->text = _text;
}

Line::Line () {
  this->text = strdup ("/DEBUG/");
}

const char *Line::get_str () {
  return this->text.c_str();
}

std::string Line::substr (unsigned pos, unsigned length) {
  return this->text.substr (pos, length);
}

unsigned Line::length () {
  return this->text.length ();
}

void Line::insert_char (char c, unsigned pos) {
  this->text.insert (pos, std::string (1, c));
}

void Line::delete_char (unsigned pos) {
  this->text.erase (pos, 1);
}

void Line::clip (unsigned pos) {
  this->text.erase (pos, this->text.length ());
}

#endif

