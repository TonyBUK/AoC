# Day 24

https://adventofcode.com/2015/day/24

This is one that's easy to solve, but hard to solve quickly... in-fact when looking at my original Ruby solution, it was measurable in minutes as to how long it'd take to find an answer (not complete) for each part.  I'm not entirely convinced my current method is quite there yet, but it's at least a few orders of magnitude faster.

**Part 1**

Basic algorithm is:

1. Find all the ways to combine the weights such that they're equal to exactly 1/3 of the total weight.
2. For each of these ways, store them as a mask in order to easily disallow overlapping solutions.
3. Sort each of these possible masks by:
a.  Number of Items
b.  Quantum Entanglement
4. Starting from the "best" solutions, find the first set of non-overlapping solutions that produces the total weight, because it's sorted, the first member of this set will be the right answer.

It's actually step #1 that takes all the time in Part 1.

**Part 2**

There's actually no change here, and since this is very much a Big O problem where the total weight is the biggest factor in how long step #1 of the basic algorithm takes, this part actually completes *far* quicker than Part 1.

**Python vs C++ vs C**

**Python**

Probably my slowest solution to date...  If C/C++ are equally slow, then it's back to the drawing board.  The only reason I've kept it for now is down to how slow the Ruby solution was!  One of the things I often did with those just to get the star was output answers as it found them, as typically, answers would be found quickly, but completing all permutations would not, so that was a little shortcut to see if at least the core algorithm was right.

Obviously I've not done that here!

**C++**

**C**
