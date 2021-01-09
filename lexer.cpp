#include "lexer.h"
#include <string>



bool 
Lexer::try_regex_match ( KeyExpression & k_exp,
                  std::string & frame_buffer,
                  const std::string & text,
                  unsigned & curr_pos) {

  bool result = false;
  std::smatch m;
  if (std::regex_search (text, m, k_exp.get_regex ())) {
    struct exp_piece ep;
    if (k_exp.get_cap_color ()) {
      ep.length = m.length (0) - k_exp.get_append_cutoff ();
    } else {
      ep.length = text.length () - m.position (0);
    }
    ep.color_index = k_exp.get_color_index ();

    expressions.insert (std::pair <unsigned, struct exp_piece> (m.position(0), ep));
    result = true;

  }

  return result;
}

bool
Lexer::try_regex_match_multiple ( KeyExpression & k_exp,
                  std::string & frame_buffer,
                  const std::string & text,
                  unsigned & curr_pos) {

  bool result = false;
  std::smatch m;
  for (std::sregex_iterator it = std::sregex_iterator (text.begin (), text.end (), k_exp.get_regex ());
       it != std::sregex_iterator ();
       it++) {

    m = *it;
    struct exp_piece ep;
    ep.length = m.length (0);
    ep.color_index = k_exp.get_color_index ();

    expressions.insert (std::pair <unsigned, struct exp_piece> (m.position(0), ep));


    result = true;
  }

  return result;
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

  color_index = this->add_color (177, 255, 200);
  this->str_aftr_inc_exp = KeyExpression ("(\".*\")|(<.*>)", 0, color_index);

  /* TODO: need way to know string continues over to next line */
  this->str_exp = KeyExpression ("\".*(\"|\\\\)", 0, color_index);
  
  color_index = this->add_color (171, 146, 191);
  this->data_type_exp = KeyExpression ("\\b(bool|signed|unsigned|short\
    |int|long|char|float|double|size_t|wchar_t|void|NULL)\\b", 0, color_index);

  color_index = this->add_color (244, 157, 110);
  this->key_word_exp = KeyExpression ("\\b(if|while|for|switch|case|do)\\b", 0, color_index);

  color_index = this->add_color (227, 231, 175);
  this->key_aux_exp = KeyExpression ("\\b(struct|const|this|return|static|class|\
                      private|public|protected|auto)\\b", 0, color_index);


  color_index = this->add_color (227, 231, 175);
  this->inline_comment_exp = KeyExpression ("//", 0, color_index);
  this->inline_comment_exp.set_cap_color (false);  // rest of line is not color capped

  color_index = this->add_color (200, 50, 200);
  this->digit_exp = KeyExpression ("[0-9]*", 0, color_index);

  color_index = this->add_color (157, 110, 244);
  this->char_exp = KeyExpression ("'[a-zA-Z0-9]+'", 0, color_index);
}

KeyExpression::KeyExpression (const std::string & regexp,
                              unsigned _append_cutoff, 
                              unsigned _color_index) {

  this->reg = std::regex (regexp);
  this->append_cutoff = _append_cutoff;
  this->color_index = _color_index;
  this->cap_color = true;

}

unsigned Lexer::color_line (std::string & frame_buffer, const std::string &text,
    unsigned line_num) {

  frame_buffer.clear ();
  expressions.clear ();
  unsigned curr_pos = 0;

  /* search and color include preprocessor key word */
  if (try_regex_match (this->inc_exp, frame_buffer, text, curr_pos)) {
    try_regex_match (this->str_aftr_inc_exp, frame_buffer, text, curr_pos);
  }

  /* search and color inline comments */ 
  try_regex_match (this->inline_comment_exp, frame_buffer, text, curr_pos);

  /* search and color strings */
  try_regex_match_multiple (this->str_exp, frame_buffer, text, curr_pos);

  /* search and color key words */
   try_regex_match_multiple (this->key_word_exp, frame_buffer, text, curr_pos);
  
  /* search and color data type */
   try_regex_match_multiple (this->data_type_exp, frame_buffer, text, curr_pos);

  /* search and color aux key words */
   try_regex_match_multiple (this->key_aux_exp, frame_buffer, text, curr_pos);

   /* search and color digits */
   try_regex_match_multiple (this->digit_exp, frame_buffer, text, curr_pos);

   /* search and color char strings */
   try_regex_match_multiple (this->char_exp, frame_buffer, text, curr_pos);

  stitch_frame_buffer (frame_buffer, text);
  
  // append rest of text
  // frame_buffer.append (text, curr_pos, text.length ());
  
  return curr_pos;
}

void Lexer::stitch_frame_buffer (std::string & frame_buffer, const std::string & text) {

  unsigned prev = 0;
  unsigned prev_length = 0;
  std::map<unsigned, struct exp_piece>::iterator it;

  for (it = expressions.begin (); it != expressions.end (); it++) {
    /* if the previous piece already took care of IT's portion, dont
     * do IT's portion. Strings and Comments trump all other
     * KeyExpressions; they trigger this case */
    if (it->first >= prev) {
      if (it->first > prev) {
        frame_buffer.append (text, prev, it->first - prev);
        prev = it->first + it->second.length;
      } 
        
      frame_buffer += color_list[it->second.color_index];
      frame_buffer.append (text, it->first, it->second.length);
      frame_buffer += "\033[38;2;255;255;255m";
      prev = it->first + it->second.length;
    }
  }

  frame_buffer.append (text, prev, text.length ());
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

bool KeyExpression::get_cap_color () {
  return this->cap_color;
}

void KeyExpression::set_cap_color (bool cap) {
  this->cap_color = cap;
}
