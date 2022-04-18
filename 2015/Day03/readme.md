# Day 3: Perfectly Spherical Houses in a Vacuum 

https://adventofcode.com/2015/day/3

This introduces:
- Basic Arrays
- Sets/Maps/Unique Arrays

This is very similar to Day 1, which can be trivially thought of as this puzzle but with a single axis.  Now we have to worry about X/Y.

Parts 1/2 are again *very* similar.  Part 2 just requires an extra logical step.

**Part 1**

The basic algorithm is to count how many unique times each pair of X/Y values occurs.

**Part 2**

As per Part 1, except rather than a single X/Y pair, we now need to keep track of two indepently moving X/Y pairs.  This is achieved internally with a very simple trick to manipulate an array index, whereby we just want to alternative Santa moves and Robot moves every other cycle.  The code achieves this with:

    nCurrent = 1 - nCurrent

Which will alternative the value of nCurrent every cycle between 0,1,0,1 etc.  This could have also been achieved with:

    nCurrent = (nCurrent + 1) % 2

or

    if nCurrent == 0 : nCurrent = 1
    else             : nCurrent = 0

They all achieve the same thing.

**Python vs C++ vs C**

Once again, something of a flex for Python.  Sets are unordered sets of data where every value is guarenteed to be unique.  Any duplicates are discarded, meaning once we've completed the loop, the size of a set will be equivalent to the number of unique locations visited.

C++ mimicks the set behaviour using std::set.  This is effectively data that also acts as as a unique key which can be any arbitrary data type, as long as the compiler understands how to compare two elements with each other.

C doesn't have an easy equivalent to this, meaning either I would have to create some sort of unique hashing based system to mimick set functionality, or for a more trivial, but ultimately slower (especially as the data size increases) solution, I've ended up just testing whether a new entry has been seen before.

This also represents the first time I had to perform heap allocations in C to ensure I could store all the data required.  This is going to be a running theme where writing in C requires some upfront interrogation of the input data in order to determine the memory footprint required for processing.
This normally results in more efficient code from an execution point of view, as Python/C++ will invariably have to perform numerous heap re-allocations / memory copies as the data set grows, but it does make the code fiddlier to write.
