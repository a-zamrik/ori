#include "lexer.h"
#include <string>



bool 
Lexer::try_regex_match ( KeyExpression & k_exp,
                  std::string & frame_buffer,
                  const std::string & text,
                  unsigned & curr_pos) {

  std::smatch m;
  if (std::regex_search (text, m, k_exp.get_regex ())) {

    // append any text inbetween the include and included file
    frame_buffer.append (text, curr_pos, m.position (0) - curr_pos);
    // add color to text
    frame_buffer += this->color_list[k_exp.get_color_index ()];
    /* -1: dont include " | < */
    frame_buffer.append (text, m.position(0), m.length (0) - k_exp.get_append_cutoff ());
    frame_buffer += "\033[38;2;255;255;255m";

    curr_pos = m.position (0) + m.length (0) - k_exp.get_append_cutoff ();
    return true;
  }

  return false;
}

/* adds ANSI Color to THIS color_list.
 * Returns the index to that color in THIS->color_list; */
size_t Lexer::add_color (unsigned char r, unsigned char g, unsigned char b) {
  std::string color;
  color += "\033[38;2;";
  color += std::to_string (r) + ";";
  color += std::to_string (g) + ";";
  color += std::to_string (b) + "m";

  this->color_list.push_back (color);
  return this->color_list.size () - 1;
}

Lexer::Lexer () {
  size_t color_index = this->add_color (227, 231, 175);
  this->inc_exp = KeyExpression ("#[ \t]*include[ \t]*(<|\")", 1, color_index);

  color_index = this->add_color (0, 227, 175);
  this->str_aftr_inc_exp = KeyExpression ("(\".*\")|(<.*>)", 0, color_index);

  color_index = this->add_color (171, 146, 191);
  this->data_type_exp = KeyExpression ("(^|\\s)(bool|signed|unsigned|short|int|\
      long|char|float|double|size_t|wchar_t|void|NULL)($|\\s)", 0, color_index);
}

KeyExpression::KeyExpression (const std::string & regexp,
                              unsigned _append_cutoff, 
                              unsigned _color_index) {

  this->reg = std::regex (regexp);
  this->append_cutoff = _append_cutoff;
  this->color_index = _color_index;

}

unsigned Lexer::color_line (std::string & frame_buffer, const std::string &text,
    unsigned line_num) {

  frame_buffer.clear ();
  unsigned curr_pos = 0;

  /* search and color include preprocessor key word */
  if (try_regex_match (this->inc_exp, frame_buffer, text, curr_pos)) {
    try_regex_match (this->str_aftr_inc_exp, frame_buffer, text, curr_pos);
  }

  /* search and color data type */
  try_regex_match (this->data_type_exp, frame_buffer, text, curr_pos);

  // append rest of text
  frame_buffer.append (text, curr_pos, text.length ());
  
  return curr_pos;
}

unsigned KeyExpression::get_append_cutoff () {
  return this->append_cutoff;
}

unsigned KeyExpression::get_color_index () {
  return this->color_index;
}

std::regex & KeyExpression::get_regex () {
  return this->reg;
}
