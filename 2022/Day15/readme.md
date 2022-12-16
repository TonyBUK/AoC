# Day 15: Beacon Exclusion Zone

https://adventofcode.com/2022/day/15

This one is all about scaleability.  The problem is presented with an example which encompasses a very small area, meaning it's absolutely possible to create a fast brute force solution.  However the input data and the puzzle itself is such that solving it in the same way is impractical for multitudes of reasons, primarily time and memory.

**Part 1**

This asks for given line, how much of it *can't* be populated by beacons.  This basically means we want to calculate the number of points on a line that are covered by sensors, and subtract the number of beacons on that line.  So let's consider a sensors range.  For simplicity, I've removed the beacon, but since it's Manhatten Distance, the Beacon *must* have been on the perimiter.

         0123456
    [-3]    #
    [-2]   ###
    [-1]  #####
    [ 0] ###S###
    [ 1]  #####
    [ 2]   ###
    [ 3]    #

So in this case the Manhatten Distance is 3.  And given we're dealing with rows we're solely interested in the length of #'s on a row.

So for each row let's calculate the X range.

         0123456
    [-3]    #    : Range  : [3,3]
                   Length : 1

         0123456
    [-2]   ###   : Range  : [2,4]
                   Length : 3

         0123456
    [-1]  #####  : Range  : [1,5]
                   Length : 5

         0123456
    [ 0] ###S### : Range  : [0,6]
                   Length : 7

         0123456
    [ 1]  #####  : Range  : [1,5]
                   Length : 5

         0123456
    [ 2]   ###   : Range  : [2,4]
                   Length : 3

         0123456
    [ 3]    #    : Range  : [3,3]
                   Length : 1

We can abstractly represent the range and length as:

    X Delta  = (ManhattenDistance - abs(SensorY - nRow))
    X Range  = [SensorX - X Delta, SensorX + XDelta]
    X Length = X Range Max - X Range Min + 1

    [-3] : X Delta  = 3 - abs(0 - -3) = 0
           X Range  = [3 - 0 , 3 + 0] = [3, 3]
           X Length = 3 - 3 + 1       = 1

    [-2] : X Delta  = 3 - abs(0 - -2) = 1
           X Range  = [3 - 1, 3 + 1]  = [2, 4]
           X Length = 4 - 2 + 1       = 3

    [-1] : X Delta  = 3 - abs(0 - -1) = 2
           X Range  = [3 - 2, 3 + 2]  = [1, 5]
           X Length = 5 - 1 + 1       = 5

    [ 0] : X Delta  = 3 - abs(0 -  0) = 3
           X Range  = [3 - 3, 3 + 3]  = [0, 6]
           X Length = 6 - 0 + 1       = 7

    [ 1] : X Delta  = 3 - abs(0 -  1) = 2
           X Range  = [3 - 2, 3 + 2]  = [1, 5]
           X Length = 5 - 1 + 1       = 5

    [ 2] : X Delta  = 3 - abs(0 -  2) = 1
           X Range  = [3 - 1, 3 + 1]  = [2, 4]
           X Length = 4 - 2 + 1       = 3

    [ 3] : X Delta  = 3 - abs(0 -  3) = 0
           X Range  = [3 - 0 , 3 + 0] = [3, 3]
           X Length = 3 - 3 + 1       = 1

So this makes the data needed to solve the algorithm as:

    Initialise the List of X Ranges to Empty

    FOR each Sensor

      Calculate the Manhatten Distance to the Beacon

      IF the (Row of Interest - Sensor Y) <= Manhatten Distance

        Calculate the Sensors X Range for he Row of Interest and Add to the List of X Ranges

      END

    NEXT

    Combine all the overlapping X Ranges until we have only non-overlapping ranges left

    Subtract the Number of Beacons on the Row from the Length of the non-overlapping ranges

Note: Overlapping ranges was part of the Day 4 puzzle: https://github.com/TonyBUK/AoC-Personal-Solutions/tree/main/2022/Day04


**Part 2**

This one is trivial to brute force, albeit slow.  Part 1 is effectively a generic 1 to describe a row as ranges.  What you may have realised from implementing part 1 is the ranges always merge down to a single range... well, except for 1 row, and that's the one we need to find, as the puzzle states there is *exactly* one location in the puzzle where a beacon could be hidden, meaning we need to find the one location within the X/Y Range given by the puzzle.

The problem is the puzzle says it could be anywhere between Rows (and columns) 0 and 4,000,000.  So you *could* write:

  FOR Rows in 0 .. 4000000

    Calculate the X Range for the Current Row

    IF there are more than 1 X Range (should be 2)

      Calculate the X co-ordinate of the gap in the ranges
      (This should be the End of Range 1 + 1, or the Start of Range 2 - 1)

      Answer = (X * 4000000) + ROW

    END

  NEXT

Most people have found this takes between 5 and 20 minutes to solve.  Not the worst, but we can do it far quicker.

Note: The examples below are incomplete, yes, there's more than 1 gap shown, but the assumption is that other sensors would fill those gaps.

To consider this, let's consider scenarios where gaps *can* actually occur.  And to do that let's consider two ranges.

      #
     ###   #
    ##### ###
     ###   #
      #

As I bring one closer to the other...

      #
     ###  #
    ########
     ###  #
      #

      #
     ### #
    #######
     ### #
      #

We finally start to see gaps of exactly 1 location, and it *only* occured when the beacons overlapped, as what was 18 unique locations covered by the beacons is now 17 locations due to overlap.

Let's consider another.

     #    #
    ###  ###
     #    #

       #
      ###
       #

Let's start to nudge these closer.

     #   #
    ### ###
     #   #
      #
     ###
      #

     #  #
    ######
     ## #
     ###
      #

This time we have a gap but there's no overlap, we still have 15 unique locations, but we do have a gap of 1 location appear, in-fact that was true on both nudges, once in the centre rows of the top 2 diamond, and once when the bottom diamond tesselated with the top two diamonds.

The second scenario seems alot less likely, and may not even be possible to fill in all the other blanks (such that there's only one unique location) without causing overlaps in the same areas.  But I'm not enough of a maths wizz to conclusively prove this, so I'll consider it as well to be on the safe side.

This means we're interested in:

1. Y Co-ordinates of Intersection
2. Y Co-ordinates if the Tops of Sensor Ranges
3. Y Co-ordinates if the Bottom of Sensor Ranges
4. Y Co-ordinates of Sensor

And to be safe, +/-1 on each.

This *massively* cuts down the search range, and we probably want to look for a solution using non-duplicates in that order too.

Now I'm sure because part of the puzzle is that there's only one answer may actually eliminate some of those possibilities, in-fact, my puzzle occurs on the Y Co-ordinate of an Intersection, as too have all the other inputs I've tried.

When a decent definitive proof comes out that this must be the case, or a counter proof comes that actually it could be any of the 4 sources I've cited, I'll link to that, for now this is just my personal take/reasoning.

Line intersection is here: https://en.wikipedia.org/wiki/Line–line_intersection

Or let's face it, Stack Overflow :)

Oh, and to bring it full circle to the algorithm:

    Calculate:
    1. Y Co-ordinates of Intersections of each Sensor Range
    2. Y Co-ordinates if the Tops of Sensor Ranges
    3. Y Co-ordinates if the Bottom of Sensor Ranges
    4. Y Co-ordinates of Sensor

    FOR Each Row calculated above

      Calculate the X Range for the Current Row

      IF there are more than 1 X Range (should be 2)

        Calculate the X co-ordinate of the gap in the ranges
        (This should be the End of Range 1 + 1, or the Start of Range 2 - 1)

        Answer = (X * 4000000) + ROW

      END

    NEXT

So it's exactly the same algorithm, but we've pre-computed only the Y Co-ordinates (rows) that we're interested in.

**Python vs C++ vs C**

**Python**

Follows the solution as-is.  It treats Intersections as the priority queue, and all others as a secondary queue.  I've yet to see any input requiring the secondary queue.

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

TBD - Playing catch-up with multiple solutions now as the puzzles get harder!