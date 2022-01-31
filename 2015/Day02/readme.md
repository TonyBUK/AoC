# Day 2

This introduces:
- Data Conversion
- Functions
- Sorting
- Basic Algorithms

Parts 1/2 both process the same data, the box dimensions, but with different algorithms, however there's no big difference in complexity.

**Part 1**

The main difference here is that this needs to known the minimum value of a given array to complete the algorithm.

**Part 2**

The main difference here is that this needs to sort the values of a given array to complete the algorithm.

**Python vs C++ vs C**

Here Python begins to flex a bit, with one of its core strengths being list comprehension, as this allows me to read the file and store the box dimensions in a single line, whereas with C/C++ I need to be a bit more verbose, first storing the text, and then converting it to an integer.

For C++ I've introduced dynamic arrays, which, via the STL library tend to be rather trivial.

For C, I've opted to not perform the boiler plate code needed to buffer any data beyond the current box dimensions and a single character from the input file, however as can be plainly seen, C is starting to need additional code, such as a function for determining the minimum value of an array, and a function for comparing array elements for sorting, whereas Python/C++ have these features readily available.

The addition of this sort of basic functionality is going to become a running theme as the challenges progress.
