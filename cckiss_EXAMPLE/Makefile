default: main

-include cckiss/Makefile

CPPARGS=-I include
CARGS=-O3 -Wall -Wextra
CXXARGS=-O3 -Wall -Wextra

OBJECTS=cckiss/main.cc.o cckiss/shtuff/hello.c.s
main: $(OBJECTS)
	$(CXX) $(OBJECTS) -o main
