main: main.o processor.o memory.o cache.o bus.o
	g++ -o main main.o processor.o memory.o cache.o bus.o ../engine/libengine.a ../utils/libutils.a -lpthread

processor.o: processor.cpp processor.hpp
	g++ -I.. -c processor.cpp

memory.o: memory.cpp memory.hpp
	g++ -I.. -c memory.cpp

cache.o: cache.cpp cache.hpp
	g++ -I.. -c cache.cpp

bus.o: bus.cpp bus.hpp
	g++ -I.. -c bus.cpp

main.o: main.cpp 
	g++ -I.. -c main.cpp

