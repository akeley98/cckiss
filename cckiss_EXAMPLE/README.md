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
g++ -I include -E main.cct
Compiling <b><i>cckiss/main.cc.o</i></b>; not yet built
g++ -O3 -Wall -Wextra cckiss/main.cc.ii -c -o cckiss/main.cc.o
cckiss/cckiss cckiss/shtuff/hello.c.s cc .cckiss.CPPFLAGS -I include .cckiss.CXXFLAGS -O3 -Wall -Wextra
New target file "cckiss/shtuff/hello.c.s".
Created directory "cckiss/shtuff/".
cc -I include -E shtuff/hello.c
Compiling <b><i>cckiss/shtuff/hello.c.s</i></b>; not yet built
cc -O3 -Wall -Wextra cckiss/shtuff/hello.c.i -S -o cckiss/shtuff/hello.c.s
g++ cckiss/main.cc.o cckiss/shtuff/hello.c.s -o main
$ ./main
Hello, cckiss.
Gadget #3 named Robot
</pre>

The first time `make` is run, `cckiss` itself gets compiled, and both
object files are compiled for the first time, using a C++ compiler
for `main.cc` and C for `shtuff/hello.c`.

If we run `make` again, `cckiss` correctly observes that no
recompilation is needed:

<pre>
$ make
cckiss/cckiss cckiss/main.cc.o g++ .cckiss.CPPFLAGS -I include .cckiss.CXXFLAGS -O3 -Wall -Wextra
"cckiss/main.cc.o": no dependency changes detected.
Up to date: <b><i>cckiss/main.cc.o</i></b>
cckiss/cckiss cckiss/shtuff/hello.c.s cc .cckiss.CPPFLAGS -I include .cckiss.CXXFLAGS -O3 -Wall -Wextra
"cckiss/shtuff/hello.c.s": no dependency changes detected.
Up to date: <b><i>cckiss/shtuff/hello.c.s</i></b>
</pre>

Now modify `include/config.h`. Set `config_shout` to `1`:

    static const int config_shout = 1;

Run `make` again:

<pre>
$ make
cckiss/cckiss cckiss/main.cc.o g++ .cckiss.CPPFLAGS -I include .cckiss.CXXFLAGS -O3 -Wall -Wextra
Dependency "include/config.h" modified; needed by "cckiss/main.cc.o".
g++ -I include -E main.cc
Compiling <b><i>cckiss/main.cc.o</i></b>; change in <b><i>include/config.h</i></b>
g++ -O3 -Wall -Wextra cckiss/main.cc.ii -c -o cckiss/main.cc.o
cckiss/cckiss cckiss/shtuff/hello.c.s cc .cckiss.CPPFLAGS -I include .cckiss.CXXFLAGS -O3 -Wall -Wextra
Dependency "include/config.h" modified; needed by "cckiss/shtuff/hello.c.s".
cc -I include -E shtuff/hello.c
Compiling <b><i>cckiss/shtuff/hello.c.s</i></b>; change in <b><i>include/config.h</i></b>
cc -O3 -Wall -Wextra cckiss/shtuff/hello.c.i -S -o cckiss/shtuff/hello.c.s
g++ cckiss/main.cc.o cckiss/shtuff/hello.c.s -o main
</pre>

cckiss correctly detects that both files include `include/config.h` --
directly by `shtuff/hello.c`, and indirectly by `main.cc` through
`include/gadget.hpp`. If we rerun `./main`,

    $ ./main
    HELLO, CCKISS!!!
    GADGET #3 NAMED ROBOT

We see that the changes have taken effect.

Now let's add a new file to the project. Create a file
`shtuff/goodbye.c`:

    $ cat > shtuff/goodbye.c
    const char* goodbye() {
            return "怎么又是你?";
    }

with corresponding header `include/goodbye.h`:

    $ cat > include/goodbye.h
    #ifndef CCKISS_EXAMPLE_GOODBYE_H
    #define CCKISS_EXAMPLE_GOODBYE_H

    #ifdef __cplusplus
    extern "C"
    #endif
    const char* goodbye();

    #endif

Call `goodbye` in `main.cc`:

...

    #include "goodbye.h"

...

    std::cout << goodbye() << '\n';

...

And finally, add `cckiss/shtuff/goodbye.c.o` to the list of objects
to be linked together in the `Makefile`:

<pre>
OBJECTS=cckiss/main.cc.o cckiss/shtuff/hello.c.s <b><i>cckiss/shtuff/goodbye.c.o</i></b>
main: $(OBJECTS)
	$(CXX) $(OBJECTS) -o main
</pre>

When we re-`make`, the result is as expected. The new file, and the
modified `main.cc` files are compiled, but the unchanged `hello.c` is
not:

<pre>
$ make
cckiss/cckiss cckiss/main.cc.o g++ .cckiss.CPPFLAGS -I include .cckiss.CXXFLAGS -O3 -Wall -Wextra
"main.cc" modified, needed by "cckiss/main.cc.o".
g++ -I include -E main.cc
Compiling <b><i>cckiss/main.cc.o</i></b>; change in <b><i>main.cc</i></b>
g++ -O3 -Wall -Wextra cckiss/main.cc.ii -c -o cckiss/main.cc.o
cckiss/cckiss cckiss/shtuff/hello.c.s cc .cckiss.CPPFLAGS -I include .cckiss.CXXFLAGS -O3 -Wall -Wextra
"cckiss/shtuff/hello.c.s": no dependency changes detected.
Up to date: <b><i>cckiss/shtuff/hello.c.s</i></b>
cckiss/cckiss cckiss/shtuff/goodbye.c.o cc .cckiss.CPPFLAGS -I include .cckiss.CXXFLAGS -O3 -Wall -Wextra
New target file "cckiss/shtuff/goodbye.c.o".
cc -I include -E shtuff/goodbye.c
Compiling <b><i>cckiss/shtuff/goodbye.c.o</i></b>; not yet built
cc -O3 -Wall -Wextra cckiss/shtuff/goodbye.c.i -c -o cckiss/shtuff/goodbye.c.o
g++ cckiss/main.cc.o cckiss/shtuff/hello.c.s cckiss/shtuff/goodbye.c.o -o main
$ ./main
HELLO, CCKISS!!!
GADGET #3 NAMED ROBOT
怎么又是你?
</pre>
