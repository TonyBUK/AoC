# Day 18: Boiling Boulders

https://adventofcode.com/2022/day/18

Usually at least once a year there will be a puzzle that goes into the third dimension.  This one is all about keeping track of adjacent sides.

**Part 1**

This just needs to find all cube sides which aren't touching another cube, or in other words, how many non-adjacent cube sides do we have.  Because we have the cube defined in X/Y/Z for a 1x1 cube, we can simplify the adjacent test.

First, let's consider an identical test.  An identical test would be where all 3 axis are identical.  Therefore an adjacent test is where one of those axis is off by 1.

    Is Adjacent:

    FOR Cube1/Cube2 axis in Cube Pair
      IF axis' are the same
        INCREMENT the Identical Counter
      ELSE IF ABS(Cube1 axis - Cube2 axis) == 1
        INCREMENT the Adjacent Counter
      END
    NEXT

    RETURN Identical Counter == 2 AND Adjacent Counter == 1

So the simple solution here is to count how many cubes are adjacent to each other.  We only need to test each pair once, as we can tell they're both adjacent, so the simple loop would be:

    FOR i = 0 .. Num Cubes - 1
      FOR j = i + 1 .. Num Cubes - 1
        IF Is Adjacent Cube[i] paired with Cube[j]
          Increment Adjacent Count by 2
      NEXT
    NEXT

So now the solution is: (Num Cubes * 6) - Adjacent Count

**Part 2**

Now things are getting trickier, the puzzle wants us to identify which sides are *external*.  I'm sure there's some very clever solutions to this, however I've gone with a BFS (Breadth First Search) and treated this as part of a path finding algorithm.

So let's start with what we know.  We know at least one cube (maybe more) that must be on the outside, we can get these by finding any cubes that have an axis value in X/Y/Z that are either bigger or smaller than any other cube.  So those cubes at those axis must be on the outside.

So how do we solve this... Water Cubes.  Or more specifially, we're going to start adding cubes from these points.  And we'll be adding them in all 26 possible combinations of 1-off displacements, and testing whether any of the cubes we've added or adjacent.  We'll keep going until no more adjacent cubes are found.

The idea is that we're going to iteratively fill the outside of the cube structure with more cubes, and for each adjacent water cube we found, it means we've found one exposed side to the water, so the solution should be the number of adjacent water cubes we found.

To support this, much like we had adjacent cubes, we now have diagonally connected cubes.  These are similar to adjacent cubes, except we now only require all three axis to be -1..+1 of our original cube.

    Is Connected:

    FOR Cube1/Cube2 axis in Cube Pair
      IF ABS(Cube1 axis - Cube2 axis) == 1
        INCREMENT the Adjacent Counter
      END
    NEXT

    RETURN Adjacent Counter == 3

Clear as mud?  Ok, let's break it into steps.

    1. Find the cubes with the largest/smallest X axis values
    2. Find the cubes with the largest/smallest Y axis values
    3. Find the cubes with the largest/smallest Z axis values
    4. Start with an empty BFS Queue

    FOR each of these Cubes

      IF the cube was the smallest in a given axis
        Add a cube to the BFS Queue that is identical to this cube, but with 1 subtracted from the given axis.
      END

      IF the cube was the largest in a given axis
        Add a cube to the BFS Queue that is identical to this cube, but with 1 added to the given axis.
      END

    NEXT

So to start with, we'll have a queue with potential (well, guarenteed) water cubes on a given axis.

So the BFS loop itself is then:

    Initialise the Surface Area to 0

    WHILE BFS Queue isn't Empty

      Pop a Water Cube from the Queue

      IF the Water Cube is already in the Water Cube set, skip processing this cycle

      IF the Water Cube is identical to an existing Cube, skip processing this cycle

      FOR each Cube

        IF the Water Cube is Adjacent to the Cube

          Increment the Surface Area by 1

        END

        IF the Water Cube as Adjacent or Diagonal to the Cube

          Indicate we should Add New Cubes to the Queue

        END

      NEXT

      IF New Cubes need Adding to the Queue

          FOR each Axis (X/Y/Z)

            Add a new Water Cube based on the Current X/Y/Z value, with the Axis of interest adjusted by -1

            Add a new Water Cube based on the Current X/Y/Z value, with the Axis of interest adjusted by +1

          END

        END

    WEND

By the end of this, the Surface Area will be the external surface area.

Note: This does assume that the outside of the cube structure is complete and entirely connected, and that we're not dealing with weirdness like an exterior with a hole, and internal non-connected cubes floating in the ether, or that we have multiple cube strucutures rather than one.

And this BFS solution is actually still sub-optimal, in that we could structure the data in such a way that we don't have to iterate all cubes to determine adjacency, but I'll leave that one to you (or you can read my code solutions).

**Python vs C++ vs C**

**Python**

This changes the adjacency test.  Because the Water Cubes / Cubes are stored in sets, rather than testing whether a Water Cube is adjacent by testing it against all Cubes, we instead iterate over all 6 cube permutations around the Water Cube (+/-1 in each individual axis), and see if those cubes are present in the Cube set.  If they are, the Water Cube must be adjacent.

We can then test diagonal connections by doing the same thing for the remaining 20 permutations (+/-1 in multiple axis).

This nets a huge speed-up over the generic solution.

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

This limits the scope of the solution my assuming all cubes occur within an 8 bit window (i.e. no axis is outside the range 0 .. 255).  It further limits this by adding on a small nominal value of 10 to each axis, the primary reason is to ensure no adjacent cubes during Part 2 will ever go negative.  In doing so, we can replace a pesky hash table with 16 Meg of RAM, allocated 8 bits for X, 8 bits for Y and 8 bits for Z, and indexing directly on the position.  Wasteful for RAM, however very quick for lookups.