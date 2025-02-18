#ifndef ORI_KEY_WORD_H
#define ORI_KEY_WORD_H

#include <string>
#include <unordered_map>

class KeyWord {

  private:
    std::string color;

    /* if is_capped, color should be reset after coloring key word. If
     * not, should be left open until its capped; like comments */
    bool capped;
    bool end_capped = false;

  public:
    KeyWord ();
    KeyWord (unsigned char, unsigned char, unsigned char, bool);
    KeyWord (unsigned char, unsigned char, unsigned char, bool, bool);
    bool is_capped ();
    bool is_end_capped ();
    const std::string & get_color ();
};

void init_key_word_map ();
KeyWord * is_keyword (const std::string &);

#endif
