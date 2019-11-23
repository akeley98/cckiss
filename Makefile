cckiss/cckiss : cckiss/cckiss.cc
	$(CXX) -std=c++14 -O2 -Wall -Wextra cckiss/cckiss.cc -o cckiss/cckiss

.cckiss.PHONY :
	@echo -n ""

cckiss/%.c.s : .cckiss.PHONY cckiss/cckiss
	cckiss/cckiss $@ $(CC) .cckiss.CPPARGS $(CPPARGS) .cckiss.CXXARGS $(CARGS)

cckiss/%.c.o : .cckiss.PHONY cckiss/cckiss
	cckiss/cckiss $@ $(CC) .cckiss.CPPARGS $(CPPARGS) .cckiss.CXXARGS $(CARGS)

cckiss/%.s : .cckiss.PHONY cckiss/cckiss
	cckiss/cckiss $@ $(CXX) .cckiss.CPPARGS $(CPPARGS) .cckiss.CXXARGS $(CXXARGS)

cckiss/%.o : .cckiss.PHONY cckiss/cckiss
	cckiss/cckiss $@ $(CXX) .cckiss.CPPARGS $(CPPARGS) .cckiss.CXXARGS $(CXXARGS)

cckiss/Makefile:
	@echo -n ""
