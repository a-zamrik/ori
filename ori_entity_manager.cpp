#include "ori_entity_manager.h"

void OriEntityManager::init (TextBox &_text_box, FileExplorer &_file_explorer) {
  file_explorer = &_file_explorer;
  text_box = &_text_box;
}

FileExplorer & OriEntityManager::get_file_explorer () {
  return *file_explorer;
}

TextBox & OriEntityManager::get_text_box () {
  return *text_box;
}

void OriEntityManager::load_new_text (const std::string &file_name) {
  text_box->load_file (file_name);
}
