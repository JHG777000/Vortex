RKLib
=====
RKLib README

RKLib is a cross-platform library written in C(C11), that makes it easier to write portable and fast code.

RKLib evolved out of several personal projects that I was working on, required such a library. I found most other libraries did not meet my requirements, mostly ease of use. Which I believe is one of the most important requirements for any piece of software. RKLib is a collection of smaller libraries that I feel (at least from my point of view) not only adhere to but are built from the ground up on the principle of ease of use.

The name comes from the smaller libraries which preceded RKLib, the first RKThreads(which became RKTasks), others simply followed the naming convention. RKThreads needed a name and “RKThreads” popped  in my head after spending too much time with NSStuff and the letters “RK” had been floating in my head since I used them as a loop indexing variable for sub pixels in a raster library I was working on, hence in this context rk stood for Raster Key.

RKLib is currently a work in progress so lack of documentation, a multitude of bugs, lack of features, and many more anti-features can be expected.

RKLib’s goal is to provide a framework for performance and cross-platform support. Yet, it already has some caveats here:

-No extensive testing has taken place, so whether it makes code actually run faster or not or even perhaps slower at least compared to alternatives, is still open. 

RKLib is currently composed of the following libraries:

—RKMem—

Provides some nice macros and a function to make dealing with memory a little easier in C, as well as:

A C List implementation.

A C Dictionary type implementation.

—RKArgs—

Allows for dynamically typed variadic functions in C.

—RKMath—

RKMath, a n-dimensional vector math library designed to take advantage of auto-vectorization.

Xor based PRNG.

—RKTasks—

RKTasks, a thread pool library. Uses tinycthread as a backend.

—Possible Future Features:

—More advanced scheduling, possibly adding a priority system.

—More advanced thread creation algorithm allowing for the dynamic adjustment of the number of threads during runtime.

Dependencies: 

 -TinyCThread https://github.com/tinycthread/tinycthread