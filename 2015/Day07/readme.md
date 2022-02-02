# Day 7

https://adventofcode.com/2015/day/7

This introduces:
- Virtual Machine
- More advanced hash map (dictionary) usage

This day is something of a staple of Advent of Code.  The task will define a series of operators that the Virtual Machine supports, and you have to implement them.  This one is simpler than most because it doesn't have any form of jump commands, however, what makes it a tad more complex is the notion that a command can only be processed when the inputs are available.  For anyone who's ever written any form of VHDL or equivalent, or maybe even LabView, this will be very familiar.

One approach I could have taken was to build a dependency tree, or recursively walk back from signal "a" until all inputs are solved (since there could very well be signals being processed that are actually irrelevant to the solving of signal "a"), however this is Day 7, so typically that isn't required...

Instead, I chose to just iterate over the commands again and again until all the outputs are solved.  This does assume that every output is unique, which on my puzzle input they were, I suspect this is true of everyones.  If it isn't, then this becomes potentially alot more complex... but let's face it, day 7, they're going to be unique!

**Part 1**

Starting with no solved signals, keep solving until the value of signal "a" is known.

**Part 2**

Using the solved value of "a", clear all solved signals, and repeat with signal "b" now containing the value of "a" from part 1.

**Python vs C++ vs C**

Python sort of trivialised this a bit, with all the inputs being refactored into: OPERATOR, INPUTS, OUTPUT, which greatly simplifies subsequent processing.

C++/C : TBD
