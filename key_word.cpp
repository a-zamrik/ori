#include "key_word.h"

/* ONLY USED FOR END CAPPED KEY WORDS */
KeyWord::KeyWord () { 
  this->end_capped = true;
  this->capped = true;
}

KeyWord::KeyWord (unsigned char r, unsigned char g, 
                  unsigned char b, bool _is_capped) {
  this->capped = _is_capped;
  this->color += "\033[38;2;";
  this->color += std::to_string (r) + ";";
  this->color += std::to_string (g) + ";";
  this->color += std::to_string (b) + "m";
}

KeyWord::KeyWord (unsigned char r, unsigned char g, 
                  unsigned char b, bool _is_capped, bool _is_end_capped) {
  *this = KeyWord (r, g, b, _is_capped);
  this->end_capped = _is_end_capped;
}


const std::string & KeyWord::get_color () {
  return this->color;
}

bool KeyWord::is_capped () {
  return this->capped;
}

bool KeyWord::is_end_capped () {
  return this->end_capped;
}

static std::unordered_map<std::string, KeyWord> key_word_map;

void init_key_word_map () {
  KeyWord value = KeyWord (244, 157, 110, true);


  key_word_map["if"] = value;
  key_word_map["while"] = value;
  key_word_map["for"] = value;
  key_word_map["switch"] = value;
  key_word_map["case"] = value;
  key_word_map["do"] = value;

  value = KeyWord (227, 231, 175, false);
  key_word_map["/*"] = value;
  key_word_map["*/"] = KeyWord (); // end cap KeyWord

  /* strings are not capped and serve as an end cap */
  /*value = KeyWord (185, 109, 64, false, true);
  key_word_map["\""] = value;*/

  value = KeyWord (171, 146, 191, true);
  key_word_map["bool"] = value;
  key_word_map["signed"] = value;
  key_word_map["unsigned"] = value;
  key_word_map["short"] = value;
  key_word_map["int"] = value;
  key_word_map["long"] = value;
  key_word_map["char"] = value;
  key_word_map["float"] = value;
  key_word_map["double"] = value;
  key_word_map["size_t"] = value;
  key_word_map["wchar_t"] = value;
  key_word_map["void"] = value;
  key_word_map["NULL"] = value;
  
  value = KeyWord (227, 231, 175, true);
  key_word_map["struct"] = value;
  key_word_map["const"] = value;
  key_word_map["this"] = value;
  key_word_map["return"] = value;
  key_word_map["static"] = value;
  key_word_map["class"] = value;
  key_word_map["private"] = value;
  key_word_map["public"] = value;
  key_word_map["protected"] = value;

  /* TODO: should be able to detect spaces between "#" and "include" and
   * still color correctly */
  key_word_map["#include"] = value;
}

KeyWord * is_keyword (const std::string & possible_keyword) {
  
  std::unordered_map<std::string, KeyWord>::iterator result;
  result = key_word_map.find (possible_keyword);
 
  if (result == key_word_map.end ())
    return NULL;

  return &result->second;
}
