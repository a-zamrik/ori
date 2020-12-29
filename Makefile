ori: ori.cpp menu.cpp line.cpp line.h text_box.cpp text_box.h ori_entity.cpp ori_entity.h file_explorer.cpp file_explorer.h
	g++ ori_entity.cpp menu.cpp text_box.cpp line.cpp ori.cpp file_explorer.cpp -o ori -lstdc++fs -g


clean:
	rm ori

