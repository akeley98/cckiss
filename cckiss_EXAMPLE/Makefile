default: main

-include cckiss/Makefile

CPPFLAGS=-I include
CFLAGS=-O3 -Wall -Wextra
CXXFLAGS=-O3 -Wall -Wextra

OBJECTS=cckiss/main.cc.o cckiss/shtuff/hello.c.s
main: $(OBJECTS)
	$(CXX) $(OBJECTS) -o main
