# Day 4: Camp Cleanup

https://adventofcode.com/2022/day/4

Pretty straight forward one here.  The puzzle supplies a pair of ranges per line in the format:

  A-B,C-D

Where the first range starts at A, and ends at B, similarly the second starts at C, ends at D.  And we need to fill in all the whole numbers between these.

So if this were 1-3,4-6, we'd expect the first range to be: 1,2,3
And similarly the second range would be: 4,5,6


**Part 1**

This task asks us to compare each pair of ranges, and find any pair where one range is contained entirely within another.  So if the first range is 1-9, and the second range is 2-8, the second range would be contained entirely within the first.

    Range 1: 1 2 3 4 5 6 7 8 9
    Range 2:   2 3 4 5 6 7 8

So the task at hand here is to essentially test where Range 1 is a subset of Range 2, or Range 2 is a subset of Range 1.

One way of testing this is to go through all the entries in range 1, and test if they exist in range 2, and vice versa.  But there is a short-cut, if either the beginning or end of a range don't exist within the other range, it *can't* be a subset.

This means we need *both* the beginning and end of the range we're testing to be exist within the other range, and only if it does will the range be a subset.

And to find out whether an entry exists within another range?  Just test whether the entry is either the same as, or bigger than the start of the other range, AND, test whether the entry is the same as, or smaller than the end of the other range.

In other words:

    IF entry >= range.start AND entry <= range.end

      It's exists within the range

    ELSE

      It doesn't exist within the range.

    END

So to bring it all together...

    IF range1.start >= range2.start AND
       range1.start <= range2.end   AND
       range1.end   >= range2.start AND
       range1.end   <= range2.end

      Range 1 is a subset of Range 2

    ELSE

      Range 1 is not a subset of Range 2

    END

But let's assume that both the ranges have their start/end in ascending order (i.e. the end of a range will never be smaller than the start).  This means we have two redundant checks.  Comparing the start of range 1 to the end of range 2 is unnecessary, because if the end of range 1 is smaller than the end of range 2, the start of range 1 *MUST* be smaller than the end of range 2 as well.  Same goes for comparing the end of range 1 to the start of range 2.  This leaves:

    IF range1.start >= range2.start AND
       range1.end   <= range2.end

      Range 1 is a subset of Range 2

    ELSE

      Range 1 is not a subset of Range 2

    END

And of course we also need to check if range 2 is a subset of Range 1.  So we need to reverse the checks as well.  Giving...

    IF (range1.start >= range2.start AND
        range1.end   <= range2.end)  OR
       (range2.start >= range1.start AND
        range2.end   <= range1.end>)

      At least one range is a subset of another

    ELSE

      Neither range is a subset of another.

    END


**Part 2**

This time around, we're looking for intersections, which just means part of one range *has* to be within part of the other.

So consider the following:

    Range 1: 1 2 3 4 5 6 7
    Range 2:   2 3 4 5 6 7 8

Neither range is a subset of the other, but they do share values in common, being 2-7.

Testing this is really just a permutation of what we did in Part 1.  So if we focus on testing whether Range 1 intersects with Range 2, we just need to check either the start or the end exists in Range 2, meaning:

    IF (range1.start >= range1.start AND
        range1.start <= range2.end) OR
       (range1.end   >= range2.start AND
        range2.end   <= range2.end)

      Range 1 Intersect with Range 2

    ELSE

      Range 1 does not Intersect with Range 2

    END

So all we need to check now is that Range 2 Intersects with Range 1 and OR them right?  Nope, we're all done.  It's impossible for Range 2 to intersect with Range 1 without Range 1 also intersecting with Range 2.


**Python vs C++ vs C**

**Python**

So all those algorithms above in Python right?  Nah, let someone else do the work.  Python has a feature called range, which, if converted to say a list, will provide a list with all values contained within that range.  But... instead of a list, let's use a type that has built in functionality for subset / interaction tests.  If you were thinking sets, then you thought right.

Utterly trivialises the task, but it does come with a risk.  Taking a range and converting it to a set will generate a set with *all* values within that range.  If the range is say 1-99, that's not a big deal, but if the range is say 1-99999999999999999, that's going to grind the program to a halt.  Fortunately this is Day 4, and the benevolent overlords at Advent of Code haven't gone full nasty just yet, so the inputs are always small.

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

This really implements the algorithm as cited.  We do take a small short cut, by acknowledging if something is a subset, it's also an intersection, but otherwise, it's the algorithm cited above in C.