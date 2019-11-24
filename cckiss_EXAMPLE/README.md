# Example Walkthrough

This example project consists of two source files, `main.cc` and
`shtuff/hello.c`, with headers `include/config.h`,
`include/gadget.hpp` and `include/hello.h`.

# Makefile

The makefile starts by defining the default target, and including the
cckiss makefile.

    default: main

    -include cckiss/Makefile

We then provide the include path (in `CPPFLAGS`) and the flags for the
C/C++ compilers.

    CPPFLAGS=-I include
    CFLAGS=-O3 -Wall -Wextra
    CXXFLAGS=-O3 -Wall -Wextra

Finally, we just have to provide a rule for linking together the two
object files together. cckiss provides patterns that compile the object
files from source for us. (Note that the object file name is the source
file name surrounded by `cckiss/` and `.o` or `.s`).

    OBJECTS=cckiss/main.cc.o cckiss/shtuff/hello.c.s
    main: $(OBJECTS)
	    $(CXX) $(OBJECTS) -o main

# Walkthrough

Take a look at the `cckiss` directory

    $ ls cckiss
    cckiss.cc  Makefile

For now, it only contains `cckiss` itself: its C++ source code and Makefile.
Run `make` and test out the executable `./main`:

<pre>
$ make
g++ -std=c++14 -O2 cckiss/cckiss.cc -o cckiss/cckiss -Wall -Wextra -Wno-constant-logical-operand -Wno-unused-function
cckiss/cckiss cckiss/main.cc.o g++ .cckiss.CPPFLAGS -I include .cckiss.CXXFLAGS -O3 -Wall -Wextra
New target file "cckiss/main.cc.o".
g++ -I include -E main.cc
Compiling <b>cckiss/main.cc.o</b>; not yet built
g++ -O3 -Wall -Wextra cckiss/main.cc.ii -c -o cckiss/main.cc.o
cckiss/cckiss cckiss/shtuff/hello.c.s cc .cckiss.CPPFLAGS -I include .cckiss.CXXFLAGS -O3 -Wall -Wextra
New target file "cckiss/shtuff/hello.c.s".
Created directory "cckiss/shtuff/".
cc -I include -E shtuff/hello.c
Compiling <b>cckiss/shtuff/hello.c.s</b>; not yet built
cc -O3 -Wall -Wextra cckiss/shtuff/hello.c.i -S -o cckiss/shtuff/hello.c.s
g++ cckiss/main.cc.o cckiss/shtuff/hello.c.s -o main
$ ./main
Hello, cckiss.
Gadget #3 named Robot
</pre>

The first time `make` is run, `cckiss` itself gets compiled, and both
object files are compiled for the first time.

If we run `make` again, `cckiss` correctly observes that no
recompilation is needed:

<pre>
$ make
cckiss/cckiss cckiss/main.cc.o g++ .cckiss.CPPFLAGS -I include .cckiss.CXXFLAGS -O3 -Wall -Wextra
"cckiss/main.cc.o": no dependency changes detected.
Up to date: <b>cckiss/main.cc.o</b>
cckiss/cckiss cckiss/shtuff/hello.c.s cc .cckiss.CPPFLAGS -I include .cckiss.CXXFLAGS -O3 -Wall -Wextra
"cckiss/shtuff/hello.c.s": no dependency changes detected.
Up to date: <b>cckiss/shtuff/hello.c.s</b>
</pre>

Now modify `include/config.h`. Set `config_shout` to `1`:

    static const int config_shout = 1;

Run `make` again:

