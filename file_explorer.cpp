
#include <string>
#include <experimental/filesystem>
#include "file_explorer.h"


void FileExplorer::load_explorer (const std::string & path) {

  for (const auto & entry : std::experimental::filesystem::directory_iterator (path)) {
    this->add_entry (Menu::MenuEntry (30, 3, entry.path ()));
  }

}

