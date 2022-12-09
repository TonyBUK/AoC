# Day 8: Treetop Tree House

https://adventofcode.com/2022/day/8

Todays problem is essentially a tree problem..................

Today's problem is one of those where the size of the input data favours a more naive solution, so let's go with that rather than being too clever.  The aim is for any given point in a grid, we want to be able to compare points in a given cardinal direction (N/E/S/W).


**Part 1**

This part wants to how many of the visible points on the grid are visible from the outside of the grid.  An item is visible if *all* points on a straight line in any of the 4 cardinal directions (N/E/S/W) have a value (height) that is smaller than the tree of interest.

So to use the most trivial of grids:

    123
    456
    789

The outer border:

    123
    4 6
    789

These are all visible because they are on the edge, leaving only the middle to consider.

    #.#
    .56
    #8#

This replaces all of the smaller heights that are smaller in the cardinal directions with dots, and replaces irrelevant heights with hashes, meaning the middle tree is visible if viewed from the North, or the East.  Diagonals are not conisdered.

Now if we take the example from the Advent of Code challenge.

    30373
    25512
    65332
    33549
    35390

Again, all the entire outer ring *cannot* be obstructed and will be considered.  So now it's just the inner grid to consider.  For convenience, I'll replace smaller heights in the cardinal directions with dots, and equal, higher or irrelvent heights (tree's that are either not in the cardinal directions, or occur after a tree that is equal or higher in cardinal directions) with X's.

Row 1 of the inner grid:

The first tree is visible from 2 points (N/W).

    X.XXX
    .5XXX
    XXXXX
    XXXXX
    XXXXX

The second tree is visible from 2 points (N/E).

    XX.XX
    XX5..
    XX.XX
    XXXXX
    XXXXX

The third tree is not visible from any point.

    XXXXX
    XXX1X
    XXXXX
    XXXXX
    XXXXX

Row 2 of the inner grid:

The first tree is visible from 1 point (E).

    XXXXX
    XXXXX
    X5...
    X.XXX
    XXXXX

The second tree is not visible from any point.

    XXXXX
    XXXXX
    XX3XX
    XXXXX
    XXXXX

The third tree is visible from 1 point (E).

    XXXXX
    XXX.X
    XXX3.
    XXXXX
    XXXXX

Row 3 of the inner grid:

The first tree is not visible from any point.

    XXXXX
    XXXXX
    XXXXX
    X3XXX
    XXXXX

The second tree is visible from 1 point (W).

    XXXXX
    XXXXX
    XX.XX
    ..5.X
    XXXXX

The third tree is not visible from any point.

    XXXXX
    XXX.X
    XXX.X
    XXX4X
    XXXXX

Finally let's consider the border:

There's 16 points visible.

    30373
    2XXX2
    6XXX2
    3XXX9
    35390


So in total, 5 tree's are visible for the inner grid, and when added to the border, 6, this makes a total of 21 visible tree's.  So now we need to express this algorithmically.

For the border, we can use the dimension.  In the examples instance, it's 5 across, and 5 high.  The generalised algorithm will be:

    Visible = (2 * Width) + (2 * Height) - 4

So where does the -4 come from?  Well, let's look at that outer border again.  What we did was add up the following 4 views.

Widths:

  Top

    30373
    XXXXX
    XXXXX
    XXXXX
    XXXXX

  Bottom

    XXXXX
    XXXXX
    XXXXX
    XXXXX
    35390

Heights:

  Left

    3XXXX
    2XXXX
    6XXXX
    3XXXX
    3XXXX

  Right

    XXXX3
    XXXX2
    XXXX2
    XXXX9
    XXXX0

Notice that the corners appear twice, hence -4.  So finally... we need to calculate visibility of the inner points.  Let's start with just looping through the whole grid.  This assumes 0 based indexing, i.e. the first row is row 0, the second is row 1 and so on.  The means we need to subtract 1 from the Height/Width.

Looping through the whole grid:

    FOR Y = 0 .. (Height - 1)

      FOR X = 0 .. (Width - 1)

        ...

      NEXT

    NEXT

To use the inner grid, it's just add/subtract 1.

    FOR Y = 1 .. (Height - 2)

      FOR X = 1 .. (Width - 2)

        ...

      NEXT

    NEXT

Now for each point we encounter, the logic is something like:

North Check:

    FOR tY = (Y - 1) .. 0

      IF Tree at (X, tY) is Taller than the Tree at (X, Y)
        Obstructed in this Axis
      END

    END

    IF no Obstructions were found, it is visible in this axis.

For the South Check, the range of tY is instead:

    (Y + 1) .. (Height - 1)

It may seem counter intuitive that going North subtracts, and going south adds, but let's view the example grid with the axis filles in.

        X 0 1 2 3 4
      Y :==========
      0 : 3 0 3 7 3
      1 : 2 5 5 1 2
      2 : 6 5 3 3 2
      3 : 3 3 5 4 9
      4 : 3 5 3 9 0

As you can see, left (West) in X is descending, right (East) in X is ascending, however up (North) is descending, and down (South) is ascending.

With that in mind, just the East/West checks...

West Check:

    FOR tX = (X - 1) .. 0

      IF Tree at (tX, Y) is Taller than the Tree at (X, Y)
        Obstructed in this Axis
      END

    END

    IF no Obstructions were found, it is visible in this axis.


For the East Check, the range of tX is instead:

    (X + 1) .. (Width - 1)

So to bring it all together...

    Set the Number of Visible Trees = (Width * 2) + (Height * 2) - 4

    FOR Y = 1 .. (Height - 2)

      FOR X = 1 .. (Width - 2)

        IF the Tree at X, Y is NOT obstructed in the North Axis OR
           the Tree at X, Y is NOT obstructed in the South Axis OR
           the Tree at X, Y is NOT obstructed in the West  Axis OR
           the Tree at X, Y is NOT obstructed in the East  Axis

          Increment the Number of Visible Trees

        END

      NEXT

    NEXT


**Part 2**

This is actually quite a similar puzzle.  Part 1 was interested in whether all trees in the 4 directions were smaller.  Part 2 is interested in the *number* of trees in the 4 directions.  This is actually why for Part 1 I worked from the tree outwards.  It could have just as easily been checking from the outer border inwards, but that wouldn't help with Part 2.

So let's take one of our ranges for Part 1 and adapt it.

North Check:

    Initialise the Number of Visible Tree's in this Axis to 0

    FOR tY = (Y - 1) .. 0

      Increment the Number of Visible Tree's in this Axis by 1

      IF Tree at (X, tY) is Taller than the Tree at (X, Y)

        Stop Processing this Loop

      END

    END

The adaptation to the other axis is the same as Part 1.  The key changes are the stop processing statement, as when we've found a tree that's an obstruction, we don't want to process any more tree's.  The second part that may seem odd is we increment the number of tree's before even testing it.  That's because even if a tree is an obstruction, you can still see it, meaning even a small tree surrounded by 4 tall tree's will still see 4 tree's (each of the taller ones in each of the 4 directions).

This means bringing it all together...

    Set the Most Scenic Tree to 0

    FOR Y = 1 .. (Height - 2)

      FOR X = 1 .. (Width - 2)

        Set the Scenic Score for the Current Tree to
          (Number of Tree's Visible from X, Y in the North Axis) *
          (Number of Tree's Visible from X, Y in the South Axis) *
          (Number of Tree's Visible from X, Y in the East  Axis) *
          (Number of Tree's Visible from X, Y in the West  Axis)

        IF the Scenic Score > Most Scenic Tree

          Set the Most Scenic Tree to the Scenic Score

        END

      NEXT

    NEXT

**Python vs C++ vs C**

**Python**

With Python, for better or worse, Part 1 really didn't care which direction it processed the data in, this is mainly because it was using array slices to get the tree's in each direction.  This means Part 2 ends up slicing pretty differently.  It could absolutely transplant the slices in Part 2 and replace those in Part 1, but not vice versa.

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

Let's get hacky!  A classic problem in C is to do with reading a file into a RAM buffer.  The problem is, often you need to read the file before knowing how to layout/size the RAM buffer.

I could have absolutely hard coded it, I know the puzzle input for me was 99x99, but maybe it won't be for other people.  Instead...

1. Open the file
2. Jump to the end of the file to determine length
3. Jump back to the beginning of the file.
4. Allocate a RAM buffer to the file length.
5. Read the entire file to the RAM buffer.

Job done right?  Not quite, there's still going to be horrible items in there, like end of line buffers, and that stuff is OS dependend and nasty.  So the C version does something a bit sneak, it shunts the data in the RAM buffer.

The basic algorithm is:

    Set Data Pointer to 0

    FOR File Pointer in 0 .. (File Length - 1)

      IF the Data at the File Pointer is an ASCII Digit

        Set the Data at the Data Pointer to Data at the File Pointer

        Increment the Data Pointer

      ELSE IF this is the first Non-ASCII Digit

        Set the Grid Width to the Data Pointer

      END

    NEXT

    Set the Grid Height to the (Data Pointer / Grid Width)

Now we could also have converted the ASCII Digits to Numbers, in ASCII, the relationship between '0' .. '9' is the same as the relationship between 0 .. 9, i.e. their sequatial and 1 different from each other, so the usual trick is:

    Number = ASCII Digit - '0'

Thing is for the tree's we never needed the heights, we only care about the relative difference between two heights, and that doesn't change if we do/don't convert the data.

Now we have that, we can process the puzzle the same as the algorithm as we have the data in a fashion we can access as a grid.  Now you might ask how me can access the data, surely it's just in a 1D Array, how can I do a X, Y index.

Simples:

    GRID[(Y * Grid Width) + X]

And believe it or not that's usually faster than:

    GRID[Y][X]

When GRID is a dynamically generated 2D array.  The reason is one is a trivial calculation and a single de-reference, the other is a double de-reference.

Whilst I typically don't talk about timings here anymore, due to their volatility on environment etc., the C solution is around over 50x faster than the Python.  Sometimes the effort to go low level is utterly worth it.