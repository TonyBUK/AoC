# Day 25: Let It Snow

https://adventofcode.com/2015/day/25

Day 25 is typically a cool-down day, with a reasonably challenging puzzle that is very likely going to be solved relatively quickly by people.  Plus as is the tradition, there is no part 2!

Because of that, this is going to be solved three ways...

1. Purely Algorithmically
2. A hybrid approach which blends the Algorithmic approach with the Mathematical Approach
3. Purely Mathematically

**Part 1**

So before we get into anything, let's look at the order in which we solve the puzzle.

       | 1   2   3   4   5   6  
    ---+---+---+---+---+---+---+
     1 |  1   3   6  10  15  21
     2 |  2   5   9  14  20
     3 |  4   8  13  19
     4 |  7  12  18
     5 | 11  17
     6 | 16

We're going to be supplied with a Row/Column input, and this is going to dictate how many times we run through the basic algorithm:

    X = (X * 252533) % 33554393 (where X is initially 20151125)

If we're at column 5, row 2, this would be run 20 times, if we're at column 2, row 5, it would be 17 times, and so on.

**Algorithmically**

So to perform this algorithmically, we're going to want to know what row/column each position corresponds to.  A basic algorithm to achieve this is:

    ROW = 1
    COL = 1

    WHILE ROW, COL are not at the Target Position
      IF ROW == 1
        ROW = COL + 1
        COL = 1
      ELSE
        ROW = ROW - 1
        COL = COL + 1
      END
    END

If we walk through the loop a few times:

    Position 1: Intially COL is 1, ROW is 1

    Position 2: Because ROW is 1 :
                  ROW is set to COL (1) + 1
                  COL is set to 1
                Therefore COL is 1, ROW is 2

    Position 3: Because ROW isn't 1 :
                  ROW is set to ROW (2) - 1
                  COL is set to COL (1) + 1
                Therefore COL is 2, ROW is 1

    Position 4: Because ROW is 1 :
                  ROW is set to COL (2) + 1
                  COL is set to 1
                Therefore COL is 1, ROW is 3

And so on... 

So the core algorithm is to calculate the ROW/COL until they match our target ROW/COL, and for each position, run the modulo algorithm.

**Hybrid**

So the only point of keeping tabs of the Rows/Columns is to know how many times we need to re-run the modulo algorithm.  So rather than walking through the grid, and checking whether we're there yet, what if we could algorithmically calculate the position number from the row/column.  i.e. if we're at column 5, row 10, that means we need to run the algorithm 20 times.

So let's take another look at that grid, specially row 1.

       | 1   2   3   4   5   6  
    ---+---+---+---+---+---+---+
     1 |  1   3   6  10  15  21

This is a sequence we've seen quite alot, those are triangle numbers: https://en.wikipedia.org/wiki/Triangular_number

So for Row 1, the arbitrary solution would be:

    iterations = triangle number of column

But of course, we're not just dealing with Row 1.

       | 1   2   3   4   5   6  
    ---+---+---+---+---+---+---+
     1 |  1   3   6  10  15  21
     2 |  2   5   9  14  20
     3 |  4   8  13  19
     4 |  7  12  18
     5 | 11  17
     6 | 16

Now let's walk the rows...

- Row 1 is the basic Triangle Number sequence (+2, +3, +4, +5, ...)
- Row 2 is the Triangle Number sequence offset by + 1 (+3, +4, +5, ...) from a higher starting point
- Row 3 is the Triangle Number sequence offset by + 2 (+4, +5, +6, ...) from a higher starting point

So we can assume the Triangle Number is actually derived from the triangle number of the row + column.

Then there's the starting point...

- Column 1: Row 1 is offset by (+1, +2, +3, +4, ...)
- Column 2: Row 1 is offset by (+2, +3, +4, +5, ...)
- Column 3: Row 1 is offset by (+3, +4, +5, +6, ...)
- Column 4: Row 1 is offset by (+4, +5, +6, +7, ...)

And so on.  It's effectively offset by the column number.

So bringing this all together, we can derive the expression:

    iterations = Triangle Number of (Row + Column - 2) + (Column - 1)

From here, we just need to run the modulo algorithm the number of times specified by iterations.

**Mathematically**

Hooo boy, getting into Crypto territory now.  We can take an iterative modulo sequence:

    X = N
    RUN C Times
      X = (X * Y) % M

And using Modulo Exponentiation: https://en.wikipedia.org/wiki/Modular_exponentiation

Re-express this as:

    X = ((Y ^ C) * N) % M
    
So plugging this into our puzzle:

    iterations = Triangle Number of (Row + Column - 2) + (Column - 1)
    RUN iterations Times
      X = (X * 252533) % 33554393 (where X is initially 20151125)

We can solve this as:

    iterations = Triangle Number of (Row + Column - 2) + (Column - 1)
    X = ((252533 ^ iterations) * 20151125) % 33554393

Now a practical note on this is that it's probably very obvious that 252533 ^ iterations will result in an absurdly high value, which is why most power operators typically offer a third parameter, being a modulo in order to prevent the value going *too* high.

i.e.

    iterations = Triangle Number of (Row + Column - 2) + (Column - 1)
    X = (power(252533, iterations, 33554393) * 20151125) % 33554393

We've now turned this whole problem into an O(1) solve time... power implementation notwithstanding.

**Part 2**

Ho ho ho...

**Python vs C++ vs C**

**Python**

Nothing overly complicated.  The Hybrid approach is around 2X the speed of the Algorithmic approach, and the Mathematical one just blows both out of the water.  I suspect for C++/C, there actually won't be a huge difference between the Hybrid/Algorithmic approach, beyond some processor level features due to the lessened branch predictions etc, whereas with the Algorimic approach in Python, it suffers the same fate as all interpretted languages, being the core Fetch/Decode/Execute cycle tending to be where the time is lost, more lines typically has a linear impact on throughput, and the simple act of calculating rows/columns/branching is enough to halve the speed.  Whereas nativelly, the impact of more SLOCS is far more slight.

**C++**

As predicted, nearly an instant solve for the Algorithmic solution, and the others are instant.  One interesting thing is that because C++ doesn't have a native modPow, but a simple google shows tons of very similar implementations, but as predicted, there's not much between them!

**C**

Basically same as C++ due to the nature of the puzzle.
