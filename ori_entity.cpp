#include "ori_entity.h"

void OriEntity::set_background_color (unsigned char r, unsigned char g, unsigned char b) {
  this->background_color = "\033[48;2;" + std::to_string (r) + ";" + std::to_string (g) + ";" + std::to_string (b) + "m";
}

void OriEntity::set_text_color (unsigned char r, unsigned char g, unsigned char b) {
  this->text_color = "\033[38;2;" + std::to_string (r) + ";" + std::to_string (g) + ";" + std::to_string (b) + "m";
}

