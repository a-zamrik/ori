
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

Line::Line (bool _from_read_only, size_t _pos, size_t _length) {

  struct file_piece piece;
  piece.from_read_only = _from_read_only;
  piece.pos = _pos;
  piece.length = _length;

  this->pieces = new std::list<struct file_piece> ();
  this->pieces->push_back (piece);
}

Line::Line (std::list<struct file_piece> * _pieces) {
  this->pieces = _pieces;
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
  for (int i = 0; i < this->text.length (); i++) {
    /* TODO: should not change user text */
    if (this->text[i] == '\t') {
      this->text.replace (i, 1, 2 - (i % 2), ' ');
    }
    if (this->text[i] == '\r') {
      this->text.erase (i, 1);
    }

  }
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
  unsigned length = 0;
  for (auto piece : *this->pieces)
    length += piece.length;

  return length;
}

void Line::insert_char (unsigned pos, size_t buffer_pos) {


  std::list<struct file_piece>::iterator pit;
  for (pit = this->pieces->begin (); pit != pieces->end (); pit++) {
    if (pit->length >= pos) {
      break;
    }
    pos -= pit->length;
  }

  if (pit == pieces->end ())
    pit--;

  /* inserting new leading piece */
  if (pos == 0) {
    struct file_piece new_piece;
    new_piece.from_read_only = false;
    new_piece.pos = buffer_pos;
    new_piece.length = 1;
    pieces->insert (pit, new_piece);

  }

  /* adding to end of line */
  else if (pos == pit->length) {
    struct file_piece new_piece;
    new_piece.from_read_only = false;
    new_piece.pos = buffer_pos;
    new_piece.length = 1;
    pieces->insert (++pit, new_piece);
  }

  /* need to split a piece */
  else {
    bool second_origin = pit->from_read_only;
    size_t second_length = pit->length - pos;
    // clip prev piece
    pit->length = pos;
    size_t second_pos = pit->length + pit->pos;
    
    
    struct file_piece new_piece;
    new_piece.from_read_only = false;
    new_piece.pos = buffer_pos;
    new_piece.length = 1;
    pieces->insert (++pit, new_piece);

    new_piece.from_read_only = second_origin;
    new_piece.pos = second_pos;
    new_piece.length = second_length;
    pieces->insert (pit, new_piece);
  }
  
  this->set_mark ('+');
  

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
std::list<struct file_piece> * Line::clip (unsigned pos) {
  /* TODO: set mark to + if changed */
  std::list<struct file_piece>::iterator pit;
  for (pit = this->pieces->begin (); pit != pieces->end (); pit++) {
    if (pit->length >= pos) {
      break;
    }
    pos -= pit->length;
  }

  if (pit == pieces->end ())
    pit--;

  std::list<struct file_piece>* result = new std::list<struct file_piece> ();
  if (pos == 0) {
    std::list<struct file_piece>* temp;
    temp = this->pieces;
    this->pieces = result;
    return temp;
  }

  /* cliping at end of line */
  else if (pos == pit->length) {
  }

  /* need to split a piece */
  else {
    bool second_origin = pit->from_read_only;
    size_t second_length = pit->length - pos;
    // clip prev piece
    pit->length = pos;
    size_t second_pos = pit->length + pit->pos;
     
    struct file_piece new_piece;
    new_piece.from_read_only = second_origin;
    new_piece.pos = second_pos;
    new_piece.length = second_length;
    result->push_back (new_piece);
  }

  this->frame_cached = false;
  return result;
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

static bool in_comment = false; // are we drawing in a comment?
void lexer_reset () {
  in_comment = false;
}

std::string & Line::pieces_to_string (const std::string &read_buffer,
                                      const std::string &write_buffer) {
  this->text.clear ();
  for (const auto piece : *this->pieces) {
    if (piece.from_read_only) {
      this->text += read_buffer.substr (piece.pos, piece.length);
    } else {
      this->text += write_buffer.substr (piece.pos, piece.length);
    }
  }
  return this->text;
}

void Line::draw_color (unsigned width, const std::string &r_buf,
                       const std::string &w_buf) {

  this->pieces_to_string (r_buf, w_buf);

  /* TODO: make a proper lexer */
  /* if in comment does not work if this statement is used */
  // if (!this->frame_cached) {
  if (1) {
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

