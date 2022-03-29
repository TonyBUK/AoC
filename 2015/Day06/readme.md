# Day 6

https://adventofcode.com/2015/day/6

This introduces:
- 2D Array Handling
- More advanced string parsing.

**Part 1**

From a given list of commands, either switch lights on, off or toggle the light from off to on and vice versa.

**Part 2**

From a given list of commands, either switch increase the light brightness, or decreate it, ensuring it never goes below 0.

**Python vs C++ vs C**

Python again showing how versatile it can be with input parsing, where the data is in a highly processible state from the point it is read.

C++ mostly follows Python, albeit it has to unravel the list comprehension summations, plus conversion of raw text to input commands is interleaved with the processing.  One choice that may be of interest is that I'm using std::vector instead of std::array.  Whilst std::array is a more logical choice, as the size of the array is known (1000x1000), it is also massive compared to how much a compiler normally allocates for an executable, so I went with std::vector which uses the heap via dynamic memory allocation.  A bit more wasteful, but easier for others to build/run.

C now really shows the weakness of not having nice library functions for being able to arbitrarily slice up a string into an array of substrings (nesting strtok calls is non-trivial), meaning I've instead gone pointer manipulation, and relied on atoi's parsing basically returning the instance a non-digit character is found.

This also represents one of the few times I'll end up choosing to represent the full data set on a puzzle like this.  Whilst it's easy to conceptualise, it's typically very wasteful, as most of the grid will remain unwritten.  A better approach is typically only storing the elements of the grid that have actually been written to, but I'll go into more detail there when I implement a solution using that particular approach.

**Times**

    Language : Time      : Percentage of Best Time
    =========:===========:=========================
    Python   : 2.3746s   : 525.50%
    C++      : 1.1808s   : 261.31%
    C        : 451.88ms  : 100.00%
