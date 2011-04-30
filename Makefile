main: main.o processor.o memory.o
	g++ -o main main.o processor.o memory.o ../engine/libengine.a ../utils/libutils.a -lpthread

processor.o: processor.cpp processor.hpp
	g++ -I.. -c processor.cpp

memory.o: memory.cpp memory.hpp
	g++ -I.. -c memory.cpp

main.o: main.cpp 
	g++ -I.. -c main.cpp

