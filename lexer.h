#ifndef ORI_LEXER_H
#define ORI_LEXER_H

#include <regex>
#include <vector>

class KeyExpression {

  private:
    std::regex reg;         // describes the pattern of thie KeyExpression
    unsigned append_cutoff; // how far to not append from end
    unsigned color_index;   // index into a ANSI color list

  public:
    KeyExpression (const std::string &, unsigned, unsigned);
    ~KeyExpression () = default;
    KeyExpression () = default;

    std::regex & get_regex ();
    unsigned get_append_cutoff ();
    unsigned get_color_index ();

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

  /* searches for "#include < */
  KeyExpression inc_exp;
  /* searches for included file to highlight */
  KeyExpression str_aftr_inc_exp;
  /* searches for data types */
  KeyExpression data_type_exp;

  /* Stores list of ANSI color strings */
  std::vector<std::string> color_list;

  /* Could use std::string::find_fisrt_of () to tokenize the string. Just
   * search for spaces (' '), and we have a token. We then can check the
   * first char of that token and see it if matches a charkeyword, like
   * '#', '/', '*', and then see if it matches any patterns */

  public:
    /* sets up state of lexer given a starting line. May need to know if
     * in a comment block or not, or string block */
    void start (unsigned start_line) {}
    unsigned color_line (std::string &, const std::string &, unsigned);
    Lexer ();
    ~Lexer () = default;
    size_t add_color (unsigned char, unsigned char, unsigned char);
    bool try_regex_match (KeyExpression &, std::string &, const std::string &, unsigned &);
};
#endif

