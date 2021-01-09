
CC = g++
CCFLAGS = -std=c++17 -lstdc++fs -g

SRC_FILES = ori_entity.cpp menu.cpp text_box.cpp line.cpp ori.cpp file_explorer.cpp ori_entity_manager.cpp prompt.cpp key_word.cpp lexer.cpp

H_FILES = ori_entity.h menu.h text_box.h line.h file_explorer.h ori_entity_manager.h prompt.h key_word.h lexer.h

OBJECTS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))

BUILD_DIR = ./build

all: dir $(OBJECTS) ori

dir:
	mkdir -p $(BUILD_DIR)

ori: $(OBJECTS)
	@echo "\n\n"
	$(CC) $^ -o $@ $(CCFLAGS) 

$(OBJECTS): $(BUILD_DIR)/%.o : ./%.cpp
	@echo $(OBJECTS)
	$(CC) -std=c++17 -c $< -o $@ -g

clean:
	rm -r $(BUILD_DIR)
	rm ori

