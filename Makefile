ori: ori.cpp menu.cpp line.h text_box.h ori_entity.h
	g++ ori.cpp menu.cpp -o ori -lstdc++ -g

clean:
	rm ori

