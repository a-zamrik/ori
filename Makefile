ori: ori.cpp line.h text_box.h
	g++ ori.cpp -o ori -lstdc++ -g

clean:
	rm ori
	rm *.o

