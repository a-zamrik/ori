
CC = g++
CCFLAGS = -std=c++17 -lstdc++fs -g

SRC_FILES = ori_entity.cpp menu.cpp text_box.cpp line.cpp ori.cpp file_explorer.cpp ori_entity_manager.cpp prompt.cpp key_word.cpp

H_FILES = ori_entity.h menu.h text_box.h line.h file_explorer.h ori_entity_manager.h prompt.h key_word.h


ori: $(SRC_FILES) $(H_FILES)
	$(CC) $(SRC_FILES) -o ori $(CCFLAGS)

clean:
	rm ori

