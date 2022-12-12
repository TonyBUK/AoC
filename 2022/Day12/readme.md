# Day 12: Hill Climbing Algorithm

https://adventofcode.com/2022/day/12

Another staple of the Advent of Code... path finding.

**Part 1**

The crux of this is to abandon your first thought, which was no doubt a recursive function like:

    POS = Start Poisition

    RECURSIVE FUNCTION

      IF the Current Pos is the Goal

        RETURN the Route Length if it's better than the Current Length

      END

      FOR each Neighbour

        IF the Neighbour is moveable...

          RECURSIVE FUNCTION (Neighbour, Best Length)

        END

      NEXT

    END

This is DFS (Depth First Search) and would solve the puzzle, but is utterly thwarted by the size of the input, as this will exponentially increase the processing time, especially as we'll end up visiting the same locations multiple times.

Instead we'll go with BFS: https://en.wikipedia.org/wiki/Breadth-first_search

A BFS algorithm applies more intelligence but at the cost of complexity of implementation.  In a nutshell, the BFS algorithm has a sorted queue of searchable nodes that results in finding the target in a far quicker time, especially as we don't need to visit the same node multiple times.

The basics of a BFS algorithm for searching is something like:

    Add the Start Position to the Queue with a Route Length of 0

    WHILE Queue isn't Empty

      POP the first item in the Queue

      IF the item has already been visited (route length to point is non-zero)
        SKIP this iteration
      END

      FOR each Neighbour

        IF the Neighbour is moveable and not already visitied...

          Add the Neighbour to the Queue with a Route Length of the current item + 1

        END

      NEXT

      SORT the Queue by Route Length

    WEND

There are permutations of this that are specialised for path finding, two popular ones are:

- A* https://en.wikipedia.org/wiki/A*_search_algorithm
- https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm

This is also the kind of algorithm that's readily available if you don't wish to implement it yourself.  Just be aware you may be forced down the naive route for Part 2.

**Part 2**

This is one that can be solved naively.  The basic algorithm could be:

    FOR each position that contains 'a'

      Find the Route Length to 'Z' from 'a' using Part 1's solution

    NEXT

    Output the Lowest Route Length

On a relatively good computer that will take around 4 seconds, which is fine, but an eternity based on what the solution *could* be.  The problem is the naive solution runs the BFS (or other) algorithm by the number 'a's within the puzzle input.

But there's a way we can run this once.  We have to consider what data it is that we've discarded from the BFS solution, and that is the visit data.  This is the length from our start point to each reachable point on the grid.  So from a specific 'a' we know how long it will take to get to any other point, including 'E'.  So... what if we started from 'E'.

This would now provide, from 'E', how long it will take to get to any other point, inclusive of every 'a'.

So now just running it once, we just find the lowest length route value for each of the 'a' positions.

And we can even merge this into Part 1, by checking the position of 'S' as well.

**Python vs C++ vs C**

**Python**

This follows the solution, but implements A*.  In order to *cache* the visitation data, this cache's the G-Scores and Came From's as calculated by trying to route from the 'E' position to an out of bounds area in order to force completeness of calculation.

The subsequent calls just perform a backtrace using the cached data.

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

This follows the solution with more of a vanilla BFS.  Because queue's require alot of memory shifting etc., this implements a circular buffer instead, albeit if sorting has to traverse the circular buffer boundaries, it copies the data into something more qsort friendly.

The better fix would be a qsort derivative that accepts indirect indexing to perform the comparisons in-place, but this is good enough for now.