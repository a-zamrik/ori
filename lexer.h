#ifndef ORI_LEXER_H
#define ORI_LEXER_H

#include <regex>
#include <map>
#include <vector>

struct exp_piece {
  unsigned length;
  unsigned color_index;

  exp_piece () {
    length = 0;
    color_index = 0;
  }

};

struct line_state {
  bool in_comment_block;
  bool in_string_block;

  line_state () {
    this->in_comment_block = false;
    this->in_string_block = false;
  }
};


class KeyExpression {

  private:
    std::regex reg;         // describes the pattern of thie KeyExpression
    unsigned append_cutoff; // how far to not append from end
    unsigned color_index;   // index into a ANSI color list
    bool cap_color;         // place default color after keyword

  public:
    KeyExpression (const std::string &, unsigned, unsigned);
    ~KeyExpression () = default;
    KeyExpression () = default;

    std::regex & get_regex ();
    unsigned get_append_cutoff ();
    unsigned get_color_index ();
    bool get_cap_color ();
    void set_cap_color (bool);

};

class Lexer {

  private:
  // maybe the index could be use bit maskes to store two indexes. One
  // to know which map to use, and the other as an index into the map or list.
  // Maybe dynamic polymorphism should be used.

  // Map <string, container index>


  // Map <string Token, paired_keyword>
  // Map <string Token, inline_paired_keyword>
  // Map <string Token, key_word>
  
  /* set to true if line had an uncapped comment block */
  bool in_comment_block= false;
  bool in_string_block = false;

  /* searches for "#include < */
  KeyExpression inc_exp;
  /* searches for included file to highlight */
  KeyExpression str_aftr_inc_exp;
  /* searches for #define */
  KeyExpression define_exp;
  /* searches for preprocess exp */
  KeyExpression preprocess_exp;
  /* searches for data types */
  KeyExpression data_type_exp;
  /* searches for key words */
  KeyExpression key_word_exp;
  /* searches for aux key words */
  KeyExpression key_aux_exp;
  /* searches for inline comments */
  KeyExpression inline_comment_exp;
  /* searchs for comment blocks */
  KeyExpression block_comment_exp;
  /* searches for end of comment block */
  KeyExpression block_comment_end_exp;
  /* searches for strings */
  KeyExpression str_exp;
  KeyExpression str_cap_exp;
  /* searches for digits */
  KeyExpression digit_exp;
  /* searches for char strings */
  KeyExpression char_exp;

  /* Stores list of ANSI color strings */
  std::vector<std::string> color_list;

  std::map<unsigned, struct exp_piece> expressions;

  /* Could use std::string::find_fisrt_of () to tokenize the string. Just
   * search for spaces (' '), and we have a token. We then can check the
   * first char of that token and see it if matches a charkeyword, like
   * '#', '/', '*', and then see if it matches any patterns */

    bool try_regex_match (KeyExpression &, std::string &, const std::string &, unsigned &);
    bool try_regex_match_multiple (KeyExpression &, std::string &, const std::string &, unsigned &);
    void try_regex_comment_block (const std::string &);
    void try_regex_string_block (const std::string &, unsigned);
    bool try_regex_cap_comment (const std::string &);
    unsigned try_regex_cap_string (const std::string &);
    void stitch_frame_buffer (std::string &, const std::string &);
  public:
    /* sets up state of lexer given a starting line. May need to know if
     * in a comment block or not, or string block */
    void start (struct line_state &);
    void start ();

    unsigned color_line (std::string &, const std::string &, struct line_state &);
    Lexer ();
    ~Lexer () = default;
    size_t add_color (unsigned char, unsigned char, unsigned char);
};
#endif

