
#include <string>
#include <experimental/filesystem>
#include "file_explorer.h"
#include "ori_entity_manager.h"


void FileExplorer::load_explorer (const std::string & path) {
  this->entries.clear ();
  
  /* add parent directory */
  Menu::MenuEntry end_entry = Menu::MenuEntry (30, 3, path + "/..");
  end_entry.set_mark ("[ ]");
  this->add_entry (end_entry);

  for (const auto & entry : std::experimental::filesystem::directory_iterator (path)) {
    Menu::MenuEntry m_entry = Menu::MenuEntry (30, 3, entry.path ());
    if (std::experimental::filesystem::is_directory (entry.path ())) {
      m_entry.set_mark ("[ ]");
    }
    this->add_entry (m_entry);
  }

  end_entry = Menu::MenuEntry (30, 3, "END");
  end_entry.set_mark ("!!!");
  this->add_entry (end_entry);

  this->mount_cursor ();
}

unsigned FileExplorer::command_enter () {

  /* If entry is a directory, load that dir into file explorer */
  if (std::experimental::filesystem::is_directory (this->curr_entry->get_str ())) {
    this->load_explorer (this->curr_entry->get_str ()); 
    return ORI_NO_OP;
  }
  /* load parent dir into file explorer */

  /* TODO: Can only navigate 1 level up and down from current working dir */
  else if (this->curr_entry->get_str ().find ("..") != std::string::npos) {
    this->load_explorer (this->curr_entry->get_str ()); 
    return ORI_NO_OP;
  }
  /* END entry should do nothing */
  else if (this->curr_entry->get_str () == "END") {
    return ORI_NO_OP;
  } 
  /* load this file into the main TextBox and tell main to swap current
   * selected OriEntity */
  else {
    OriEntityManager::load_new_text (this->curr_entry->get_str ());
    return ORI_SWAP_ENTITY;
  }
}

