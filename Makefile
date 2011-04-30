main: main.o processor.o memory.o cache.o
	g++ -o main main.o processor.o memory.o cache.o ../engine/libengine.a ../utils/libutils.a -lpthread

processor.o: processor.cpp processor.hpp
	g++ -I.. -c processor.cpp

memory.o: memory.cpp memory.hpp
	g++ -I.. -c memory.cpp

cache.o: cache.cpp cache.hpp
	g++ -I.. -c cache.cpp

main.o: main.cpp 
	g++ -I.. -c main.cpp

