#include "lexer.h"
#include <string>

/* should be called before being a render pass. sets the state of the
 * lexer for proper syntax highlighting */
void
Lexer::start (struct line_state & state) {
  this->in_comment_block = state.in_comment_block;
  this->in_string_block = state.in_string_block;
}

/* default lexer start: assumes that rendering is occuring at top of
 * file. The very first line */
void
Lexer::start () {
  this->in_comment_block = false;
  this->in_string_block = false;
}

/* searches for a pattern within TEXT using the regex in K_EXP. if found
 * a expression piece will be made */
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

/* searches for multiple pattersn with in TEXT using the regex in K_EXP.
 * an expression piece will be made for each match */
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

/* searches for a comment block in text. If a comment block is found
 * but its end is not found, then the lexer will enter a block comment
 * state. Only Lexer::try_regex_cap_comment can take the lexer out of
 * that state */
void
Lexer::try_regex_comment_block (const std::string &text) {
  
  bool found = false;

  std::smatch m;
  for (std::sregex_iterator it = std::sregex_iterator (text.begin (), text.end (), block_comment_exp.get_regex ());
       it != std::sregex_iterator ();
       it++) {

    m = *it;
    struct exp_piece ep;
    ep.length = m.length (0);
    ep.color_index = block_comment_exp.get_color_index ();

    expressions.insert (std::pair <unsigned, struct exp_piece> (m.position(0), ep));

    found = true;
  }

  /* see if there was an uncapped comment block */
  if (found) {
    unsigned i = m.position (0) + m.length (0) - 1;
    if (text[i - 1] != '*' || text[i] != '/') {
      this->in_comment_block = true;
    } else {
      this->in_comment_block = false;
    }
  }

}

void
Lexer::try_regex_string_block (const std::string &text) {
  
  bool found = false;

  std::smatch m;
  for (std::sregex_iterator it = std::sregex_iterator (text.begin (), text.end (), str_exp.get_regex ());
       it != std::sregex_iterator ();
       it++) {

    m = *it;
    struct exp_piece ep;
    ep.length = m.length (0);
    ep.color_index = str_exp.get_color_index ();

    expressions.insert (std::pair <unsigned, struct exp_piece> (m.position(0), ep));

    found = true;
  }

  /* see if there was an uncapped comment block */
  if (found) {
    unsigned i = m.position (0) + m.length (0) - 1;
    if (text[i] == '\\') {
      this->in_string_block = true;
    } else {
      this->in_string_block = false;
    }
  } else {
    this->in_string_block = false;
  }

}

bool
Lexer::try_regex_cap_string (const std::string &text) {
  
  std::smatch m;

  if (std::regex_search (text, m, str_cap_exp.get_regex ())) {
    struct exp_piece ep;
    ep.length = m.length (1) + m.position (1);
    ep.color_index = str_cap_exp.get_color_index ();

    expressions.insert (std::pair <unsigned, struct exp_piece> (0, ep));


    
    unsigned i = m.position (1) + m.length (1) - 1;
    if (text[i] == '\\') {
      return true;
    }

    return false;
  }
  return false;
}



/* will search for end of block comment in text. If found; a expression piece
 * will be used to end to comment syntax highlight */
bool
Lexer::try_regex_cap_comment (const std::string &text) {
  
  std::smatch m;

  if (std::regex_search (text, m, block_comment_end_exp.get_regex ())) {
    struct exp_piece ep;
    ep.length = m.length (0) + m.position (0);
    ep.color_index = block_comment_end_exp.get_color_index ();

    expressions.insert (std::pair <unsigned, struct exp_piece> (0, ep));

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


  color_index = this->add_color (175, 227, 231);
  this->define_exp = KeyExpression ("#[ \t]*(define|ifndef)[ \t]*[a-zA-Z0-9_]", 1, color_index);
  this->preprocess_exp = KeyExpression("#[ \t]*endif\\b", 0, color_index);

  color_index = this->add_color (177, 255, 200);
  this->str_aftr_inc_exp = KeyExpression ("(\".*\")|(<.*>)", 0, color_index);

  /* TODO: need way to know string continues over to next line */
  this->str_exp = KeyExpression ("\".*(\"|\\\\)", 0, color_index);
  this->str_cap_exp = KeyExpression ("(\\\\|\")\\s*$", 0, color_index);
  
  color_index = this->add_color (200, 200, 255);
  this->data_type_exp = KeyExpression ("\\b(bool|signed|unsigned|short|int|long|char|float|double|size_t|wchar_t|void|NULL|true|false)\\b", 0, color_index);

  color_index = this->add_color (244, 157, 110);
  this->key_word_exp = KeyExpression ("\\b(if|else|while|for|switch|case|do|default)\\b", 0, color_index);

  color_index = this->add_color (227, 231, 175);
  this->key_aux_exp = KeyExpression ("\\b(struct|break|const|this|return|static|class|private|public|protected|auto)\\b", 0, color_index);


  color_index = this->add_color (227, 231, 100);
  this->inline_comment_exp = KeyExpression ("//", 0, color_index);
  this->inline_comment_exp.set_cap_color (false);  // rest of line is not color capped

  this->block_comment_exp = KeyExpression ("\\/\\*(\\/(?!\\*)|[^*])*(\\*\\/|$)", 0, color_index);
  this->block_comment_end_exp = KeyExpression ("\\*\\/", 0, color_index);

  color_index = this->add_color (200, 50, 200);
  this->digit_exp = KeyExpression ("\\b(0x[\\da-fA-F]+|\\d+)\\b", 0, color_index);

  color_index = this->add_color (157, 110, 244);
  this->char_exp = KeyExpression ("'(\\w|0x{1,2}|\\\\[0-9]{3}|\\\\[a-z])'", 0, color_index);
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
    struct line_state &line_state) {

  /* TODO: Could use a regex iterator to split the string based on 
   * spaces and special chars, such as #, comment blocks, or " */ 

  frame_buffer.clear ();
  expressions.clear ();
  unsigned curr_pos = 0;

  /* previous line had an uncapped comment */
  if (this->in_comment_block) {
    
    /* look for comment end cap */
    if (try_regex_cap_comment (text)) {
      this->in_comment_block = false;
      line_state.in_comment_block = false;
      
    } else {
      struct exp_piece ep;
      ep.length = text.length ();
      ep.color_index = block_comment_exp.get_color_index ();

      expressions.insert (std::pair <unsigned, struct exp_piece> (0, ep));

      line_state.in_comment_block = true;
    }

  } else {
    line_state.in_comment_block = false;
  }


  if (this->in_string_block) {

      struct exp_piece ep;
      ep.length = text.length ();
      ep.color_index = str_exp.get_color_index ();

      expressions.insert (std::pair <unsigned, struct exp_piece> (0, ep));


      line_state.in_string_block = try_regex_cap_string (text);
      this->in_string_block = line_state.in_string_block;

  }

  
  line_state.in_string_block = this->in_string_block;
  if (!line_state.in_comment_block && !line_state.in_string_block) {


    /* search and color comment blocks */
    try_regex_comment_block (text);
    if (this->in_comment_block)
      line_state.in_comment_block = true;

    /* search and color strings */
    try_regex_string_block (text);
    line_state.in_string_block = this->in_string_block;
    
    /* search and color include preprocessor key word */
    if (try_regex_match (this->inc_exp, frame_buffer, text, curr_pos)) {
      try_regex_match (this->str_aftr_inc_exp, frame_buffer, text, curr_pos);
    }

    /* search and color defined words */
    try_regex_match (this->define_exp, frame_buffer, text, curr_pos);

    /* search and color preprocessor commands */
    try_regex_match (this->preprocess_exp, frame_buffer, text, curr_pos);

    /* search and color inline comments */ 
    try_regex_match (this->inline_comment_exp, frame_buffer, text, curr_pos);

    /* search and color key words */
     try_regex_match_multiple (this->key_word_exp, frame_buffer, text, curr_pos);
    
    /* search and color data type */
     try_regex_match_multiple (this->data_type_exp, frame_buffer, text, curr_pos);

    /* search and color aux key words */
     try_regex_match_multiple (this->key_aux_exp, frame_buffer, text, curr_pos);


     /* search and color char strings */
     try_regex_match_multiple (this->char_exp, frame_buffer, text, curr_pos);

     /* search and color digits */
     try_regex_match_multiple (this->digit_exp, frame_buffer, text, curr_pos);
  }
   
   stitch_frame_buffer (frame_buffer, text);
  
  return curr_pos;
}

/* using the expression peices that were created from the various KeyPieces;
 * a colored string can be stitched together and placed into FRAME_BUFFER.
 * TEXT is the text that will be colored */
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
