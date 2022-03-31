# Day 8

https://adventofcode.com/2016/day/8

Relatively simple grid handling.  My only complaint about this puzzle is that Part 1/2 are really the same thing.  We have to process the input data in the same way, the only difference is how we represent the results.

In a nutshell, we have a grid area of 50x6 that's initially blank.

    ..................................................
    ..................................................
    ..................................................
    ..................................................
    ..................................................
    ..................................................

And we'll be provided 2 commands.

- rect
    -  This command will create a grid, from the top left for a given width/height.  For example, 5x3 would be to create a grid 5 across, 3 high as follows:

      #####.............................................
      #####.............................................
      #####.............................................
      ..................................................
      ..................................................
      ..................................................

- rotate row
    -  This command will rotate a given row right by a specified number of spaces.  For example, y=2 by 3 would move row 2 by 3 spaces to the right.

      #####.............................................
      ...#####..........................................
      #####.............................................
      ..................................................
      ..................................................
      ..................................................

    - Note: This will wraparound the end of a row back to the beginning.

- rotate column

    - This command will rotate a given column down by a specified number of spaces.  For example, x=4 by 2 would move column 4 by 2 spaces downwards.

          ####..............................................
          ...#.###..........................................
          #####.............................................
          ....#.............................................
          ....#.............................................
          ..................................................

    - Note: This will wraparound the bottom of a column back to the top.

**Part 1**

This works initially by initialising a grid comprising soley of spaces to the dimensions 50x6.

For each rectangle command, the basic algorithm is:

    FOR ROW in 0 .. Rectangle Height
      FOR COL in 0 .. Rectangle Width
        Set the Grid position specified by ROW, COL to a block character #
      END
    END

For the rotate command, the basic algorithm is:

    Buffer the Row or Column specified by the Rotate Command
    
    FOR either the Row or Column specified by the Rotate Command
      Set the Grid position specified by the (ROL, COL) + the amount to shift the Row/Column by to the unshifted value from the Buffered Row/Column
    END

Once all commands are processed, we just count the number of block characters.

**Part 2**

This uses the same output as part 1, but now prints the grid (consisting of spaces/block characters) itself rather than counting the block characters.

**Python vs C++ vs C**

**Python**

For better or worse, strings in Python are immutable, which means they can't be changed, the best we can do is create new strings.  This is why we use a 2D Array, rather than say a 1D Array of strings.  Otherwise, this is all pretty simple, the only gotcha here is Pythons shallow vs deep copy.  For example consider:

    # Buffer the Row Data
    kBufferedRow = [C for C in kGrid[nRow]]

This could have been:

    kBufferedRow = kGrid[nRow]
    
Except this is actually a shallow copy.  What this means is if I then do something like:

    kBufferedRow[0] = "A"
    
It will also update kGrid[nRow] at the same time, and this is because this didn't copy the contents of the row, it just made them both point to the exact same data.  There are other ways of achieving this, such as deep copy, but ultimately this is a relatively easy way of copying simple data.  Were this a highly nested data structure, I'd probably resort to a deep copy.

In terms of why Python does this, I'd actually argue it's better Python does default to shallow copies, since this is a massive performance boost, and most of a time, a true copy isn't strictly necessary anyway, so deep copies tend to be the exception rather than the rule, and it's easier to track down bugs caused by shallow copies and fix those than it is to try and work out which copies can be replaced with shallow copies, as one you have to fix, one you can get away with leaving as is even if it means leaving performance on the table.

**C++**

**C**
