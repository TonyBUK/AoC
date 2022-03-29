# Day 5

https://adventofcode.com/2015/day/5

This introduces:
- More complex algorithms.

**Part 1**

From a given list of random letters, test how many meet a given ruleset, being:
- Must contain at least three vowels.
- Must contain at least one pair of repeated consecutive letters.
- Must not contain any of a pre-determined list of bad words.

**Part 2**

As per Part 1, but with a different ruleset.
- Must have at least one pair of letters duplicated, in their entirety elsewhere in the string without overlap.
- Must have at least one letter sandwiched between another pair of letters, i.e. "owo".

**Python vs C++ vs C**

Another Python flex where we can use list comprehension, and the all important zip function.  Zip allows me to iterate on data sets in tandem, in this particular case, I can use it to evaluate each pair of letters by combining zip with slicing.  We also see a bit of a weakness in Python, being that for loops in C/C++ can have any number of esoteric means of automatically terminating, whereas in Python it's nested breaks, albeit even that can be managed...
https://stackoverflow.com/questions/653509/breaking-out-of-nested-loops

When we get to C++ we see what the Python script is really doing with list comprehension, otherwise it's essentially the Python solution.

And when we get to C, we see a destructive, yet relatively efficient way of splitting a string.  From a starting position of a single string containing every line within the file, running strtok effectively destroys the string as it goes, by overwriting key points in the string with NUL terminators.  It has the neat advantage of allowing the code to iterate over the string without any copying, but it does render the original string completely unuseable.

This also does hammer home something some may not be aware of, C does *NOT* have a boolean type.  All if statements in C/C++ are actually 0 (false) or non-zero (true).  C++ added the bool type as a convenience, whereas in C a very typical bodge is to just use an unsigned type and use 1/0 (defined as constants) for True/False respectively.

**Times**

    Language : Time      : Percentage of Best Time
    =========:===========:=========================
    Python   : 23.97ms   : 1267.81%
    C++      : 2.73ms    : 144.22%
    C        : 1.89ms    : 100.00%
