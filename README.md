Vortex
=====
Vortex README

Vortex is a cross-platform library written in C(C11), designed to make it easier to write portable and efficient code.

Vortex is currently composed of the following libraries:

—VortexMem—

Provides some nice macros and a function to make dealing with memory a little easier in C, as well as:

A C List implementation.

A C Dictionary type implementation.

—VortexArgs—

Allows for dynamically typed variadic functions in C.

—VortexMath—

A n-dimensional vector math library designed to take advantage of auto-vectorization.

Xor based PRNG.

—VortexTasks—

A thread pool library. Uses tinycthread as a backend.

Uses:

 -TinyCThread https://github.com/tinycthread/tinycthread

-VortexGraph-

A libary that is designed to make working with graph based data structures easier.

-VortexParser-

A lexer and parser libary, including a parse tree implementation.

## Building

Vortex uses [builder][1] for its build system.

[1]:https://github.com/JHG777000/builder

To download and build use this command:


	builder -u https://raw.githubusercontent.com/JHG777000/Vortex/main/buildfile

To run tests, add -i __t:

	builder -i __t -u https://raw.githubusercontent.com/JHG777000/Vortex/main/buildfile
