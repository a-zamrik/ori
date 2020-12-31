
#include <string>
#include <experimental/filesystem>
#include "file_explorer.h"
#include "ori_entity_manager.h"


void FileExplorer::load_explorer (const std::string & path) {

  for (const auto & entry : std::experimental::filesystem::directory_iterator (path)) {
    Menu::MenuEntry m_entry = Menu::MenuEntry (30, 3, entry.path ());
    if (std::experimental::filesystem::is_directory (entry.path ())) {
      m_entry.set_mark ("[ ]");
    }
    this->add_entry (m_entry);
  }

}

unsigned FileExplorer::command_enter () {
  OriEntityManager::load_new_text (this->curr_entry->get_str ());
  return ORI_SWAP_ENTITY;
}

