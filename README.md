# Keep It Simple Stupid C/C++ build dependency tool

cckiss is a tool that automatically tracks the dependencies of C and
C++ files and recompiles them if any changes are detected. cckiss is
designed to supplement a Unix makefile, rather than replace `make`
entirely. cckiss provides patterns that automate away the task of
writing these kinds of makefile entries:

    bin/foo.o foo.c foo.h bar.h xyzzy.h
            $(CC) $(CARGS) $(CPPARGS) foo.c -c -o bin/foo.o

This automation eliminates a whole class of errors related to
forgetting a (possibly indirectly) included header in the list
of dependencies.

# Quick Start

Clone `cckiss` into the directory where your top-level `Makefile` is.
Include `cckiss/Makefile` in your `Makefile`:

    -include cckiss/Makefile

`cckiss/Makefile` provides patterns that compile source files to
object files named `cckiss/`source-file-path`.o` (or `.s`, if you
prefer to compile to assembly), so you only have to provide a rule for
linking all objects together. The variables `CPPFLAGS`, `CC`,
`CFLAGS`, `CXX`, and `CXXFLAGS` are supported in the standard way.

As an example, if your program contains two source files,
`src/peach.c++` and `src/util.c`, cckiss expects to compile them to
the object files `cckiss/src/peach.c++.o` and `cckiss/src/util.c.o`,
and your `Makefile` should look something like

    default: program
    
    -include cckiss/Makefile
    
    CPPFLAGS=-I include
    CC=gcc
    CXX=g++
    CFLAGS=-O2 -Wall
    CXXFLAGS=-O2 -Wall

    # cckiss automatically compiles the listed object/asm files
    program: cckiss/src/peach.c++.o cckiss/src/util.c.o
            $(CXX) cckiss/src/peach.c++.o cckiss/src/util.c.o -o program

The above `Makefile` is the moral equivalent of

    default: program
    
    bin/peach.o: src/peach.c++ include/peach.h++ some-other-deps.h
            g++ src/peach.c++ -I include -O2 -Wall -c -o bin/peach.o

    bin/util.o: src/util.c include/util.h some-other-deps.h
            gcc src/util.c -I include -O2 -Wall -S -o bin/util.o

    program: bin/peach.o bin/util.o
            g++ bin/peach.o bin/util.o -o program

A few things to note:


1. cckiss supports compiling both C and C++. Any source file ending in
lowercase `.c` is assumed to be C; all others are assumed C++ (there's
no end to the list of C++ file extensions invented, so I just support
them all).

2. cckiss appends `.o` (or `.s`) to the compiled file name, instead of
replacing the extension. So `foo.c` becomes `cckiss/foo.c.o`, NOT
`cckiss/foo.o`.

3. cckiss strictly separates the preprocessing step from the compiling
step, so, it's important that preprocessor flags (like `-I
my/include/dir`) use the `CPPFLAGS` variable instead of `CFLAGS` or
`CXXFLAGS`.

4. You may want to provide a default target before including
`cckiss/Makefile` -- otherwise, the default target for the project
will be the `cckiss` executable itself, which is probably not what you
want.