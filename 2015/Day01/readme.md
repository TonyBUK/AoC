**Day 1**

https://adventofcode.com/2015/day/1

The actual algorithm is relatively trivial.  We need to keep track of how many "(" are present versus ")".

**Part 1**

This can be solved by taking the total number of "(", and subtracting the total number of ")".

**Part 2**

This is a variation of Part 1.  If you solved Part 1 by adding 1 to a counter each time you saw a "(", and subtracting 1 each time you saw a ")", then this part is basically solved, as you just need to figure out how many "(" and ")" you saw in total when the counter gets to -1.

**Python vs C++ vs C**

The solutions themselves are nearly identical.  The main difference is that Python/C++ solve part 1 by subtracting the number of ")" from the number of "(", the C solution, to save on all the boiler plate code needed to buffer the whole file, solves Parts 1/2 in the same way, by constantly adding/subtracting to a single counter as each instance of "(" and ")" is found respectively.
