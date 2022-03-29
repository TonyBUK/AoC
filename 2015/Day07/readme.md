# Day 7

https://adventofcode.com/2015/day/7

This introduces:
- Virtual Machine
- More advanced hash map (dictionary) usage

This day is something of a staple of Advent of Code.  The task will define a series of operators that the Virtual Machine supports, and you have to implement them.  This one is simpler than most because it doesn't have any form of jump commands, however, what makes it a tad more complex is the notion that a command can only be processed when the inputs are available.  For anyone who's ever written any form of VHDL or equivalent, or maybe even LabView, this will be very familiar.

One approach I could have taken was to build a dependency tree, or recursively walk back from signal "a" until all inputs are solved (since there could very well be signals being processed that are actually irrelevant to the solving of signal "a"), however this is Day 7, so typically that isn't required...

Instead, I chose to just iterate over the commands again and again until all the outputs are solved.  This does assume that every output is unique, which on my puzzle input they were, I suspect this is true of everyones.  If it isn't, then this becomes potentially alot more complex... but let's face it, day 7, they're going to be unique!

Also since the input data is starting to get more complex, one of the things I've started doing is more liberal use of debug assertions.  These effectively crash the program if the statement inside evaluates to False/0.  Typically assertions are used to flush out any point in the code where if someone isn't true, then it means somethings gone horribly wrong somewhere.  This can be very useful later on as the challenges get more complex, as invariably the code will be written/tested against example data before being thrown at the real puzzle data.  Often that ends up disproving a core assumption.

One important thing to note, debug assertions are not error handling.  If an error can naturally occur, this should be handled with logic/exceptions etc., the point of debug assertions is you enable them during development to trap things that shouldn't occur, then when you release the code, the debug assertions all get automatically stripped out of the code.

**Part 1**

Starting with no solved signals, keep solving until the value of signal "a" is known.

**Part 2**

Using the solved value of "a", clear all solved signals, and repeat with signal "b" now containing the value of "a" from part 1.

**Python vs C++ vs C**

Python sort of trivialised this a bit, with all the inputs being refactored into: OPERATOR, INPUTS, OUTPUT, which greatly simplifies subsequent processing.

C++ follows the Python approach pretty closely, albeit it's a tad more verbose, but STL does a very good job of maintaining feature parity with Python for an out of the box solution.

C continues to expose just how much heavy lifting the dictionaries/std map functionality is really providing.  This implements a hash map for parity, albeit the code would be *far* simpler if this took the throughput hit and just searched a regular array for entries, however moving forwards this sort of functionality will be critical for C to maintain any hope of maintaining parity with the solutions.  We also start to see some of the pitfalls of C to do with memory allocation, it starts to become less obvious who owns what data and who is responsible for free'ing it after use.  This is very much a solved problem in C++ with classes (class constructor allocates, class destructor de-allocates), but with C this can get *VERY* messy.

**Times**

    Language : Time      : Percentage of Best Time
    =========:===========:=========================
    Python   : 22.37ms   : 139.42%
    C++      : 94.1ms    : 586.52%
    C        : 16.04ms   : 100.00%
