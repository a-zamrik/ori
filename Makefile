ori: ori.cpp menu.cpp line.cpp line.h text_box.cpp text_box.h ori_entity.h
	g++ menu.cpp text_box.cpp line.cpp ori.cpp -o ori -lstdc++ -g


clean:
	rm ori

