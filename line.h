#ifndef ORI_LINE_H
#define ORI_LINE_H

#include <iostream>
#include <string.h>
#include <list>

class Line {
  private:
    std::string text;
    std::string mark = "[ ]";
  
  public:
    Line (char*);
    Line (void);
    Line (std::string);
    Line (std::string&, std::string&);
    const char *get_str ();
    const std::string get_str_obj (); 
    unsigned length ();
    void insert_char (char, unsigned);
    void append (std::list<Line>::iterator &);
    void delete_char (unsigned);
    bool clip (unsigned);
    std::string substr (unsigned, unsigned);
    const char* get_mark ();
    void set_mark (char);
};

const char* Line::get_mark () {
  return this->mark.c_str ();
}

void Line::set_mark (char c) {
  this->mark[1] = c;
}

Line::Line (std::string &text, std::string &mark) {
  this->text = std::string (text);
  this->mark = std::string (mark);
}

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

const std::string Line::get_str_obj () {
  return this->text;
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

void Line::append (std::list<Line>::iterator &line) {
  this->text.append (line->get_str_obj ());
}

void Line::delete_char (unsigned pos) {
  this->text.erase (pos, 1);
}

// return true if line changed
bool Line::clip (unsigned pos) {
  unsigned old_length = this->text.length();
  this->text.erase (pos, this->text.length ());
  return old_length != text.length ();
}

#endif

