
#include "key_word.h"
#include <iostream>
#include <string.h>
#include <list>
#include "line.h"

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
  this->set_mark ('+');
  this->text.insert (pos, std::string (1, c));
  this->frame_cached = false;
}

void Line::append (std::list<Line>::iterator &line) {
  if (line->length ()) {
    if (this->length ()) {
      this->set_mark ('+');
    }
    this->text.append (line->get_str_obj ());
  }
  this->frame_cached = false;
}

void Line::delete_char (unsigned pos) {
  this->text.erase (pos, 1);
  this->set_mark ('+');
  this->frame_cached = false;
}

// return true if line changed
bool Line::clip (unsigned pos) {
  unsigned old_length = this->text.length();
  this->text.erase (pos, this->text.length ());
  if (old_length != text.length ())
    this->set_mark ('+');
  this->frame_cached = false;
}

static inline 
bool is_deliminator (char c) {

  /* TODO: this string should be somewhere else */
  std::string delim (" ()\\-");

  for (int i = 0; i < delim.length () + 1; i++) {
    if (c == delim[i]) {
      return true;
    }
  }
  return false;
}

void Line::draw_color (unsigned width) {
  static bool in_comment = false; // are we drawing in a comment?

  if (!this->frame_cached) {
    this->frame_buffer.clear ();
    /* TODO: Need to create a lexer
     * comments should not depend on delimantor for highlihgt */
    std::size_t prev = 0;
    std::size_t curr = 0;
    bool in_delim = is_deliminator (this->text[0]);
    std::string word;
    KeyWord * key_word = NULL;
    for (int i = 0; i < this->text.length () + 1; i++) {
      bool is_delim = is_deliminator (this->text[i]);

      /* exiting delimenator portion and entered start of word */
      if (!is_delim && in_delim) {
        in_delim = false;
        this->frame_buffer += this->text.substr (prev, i - prev);
        prev = i;
      }
      /* exiting word and entered delimenator portion of text */
      else if (is_delim && !in_delim) {
        in_delim = true;
        word = this->text.substr (prev, i - prev);

        /* if the word is not a keyword, dont add color */
        if ((key_word = is_keyword (word)) != NULL) {
          if (key_word->is_end_capped ()) {
            in_comment = false;
          }
          /* dont draw colors if in a comment */
          if (!in_comment) {
            this->frame_buffer += is_keyword (word)->get_color ();
            this->frame_buffer += word;
            /* if capped, place capped color */
            if (key_word->is_capped ()) {
              this->frame_buffer += "\033[38;2;255;255;255m";
            } else {
              in_comment = true;
            }
            prev = i;
          }
        } else {
          this->frame_buffer += word;
          prev = i;
        }
      }
    }
    if (prev < this->text.length ()) {
      frame_buffer+=this->text.substr (prev);
    }
  this->frame_cached = true;
  }

  printf("%s%*s",
        this->frame_buffer.c_str (),
        width  - (unsigned) this->text.length (),
        this->mark.c_str ());


}

void Line::draw (unsigned width) {

  printf("%s%*s",
        this->text.c_str (),
        width  - (unsigned) this->text.length (),
        this->mark.c_str ());
}

