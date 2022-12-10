# Day 10: Cathode-Ray Tube

https://adventofcode.com/2022/day/10

One of the staples of Advent of Code, an interpretter.  The puzzle provides all the operations, and it's our job to code these.  The twist this year is we need to be cycle accurate.

**Part 1**

Normally an interpretter looks like this:

    WHILE the program is running...

      FETCH the Current Opcode

      DECODE the Current Opcode

      EXECUTE the Current Opcode

    END

- FETCH is getting the opcode data.
- DECODE is determining what the opcode is.
- EXECUTE is processing the opcode.

Where the twist kicks in is that we need to keep track of cycles, and perform specific actions on specific cycles.  Before we start implementing this we need to be clear what the puzzle is asking for.

Firstly, let's look at the Operations.

    ADDX : Add a value to the in-built register X.  The value can be positive or negative.
           Takes 2 Cycles to Execute.
    NOOP : Do nothing.
           Takes 1 Cycle to Execute.

Secondly, let's look at the puzzle itself, being to take a Register X, and find out what it's value after:

    20 Cycles
    60 Cycles
    100 Cycles
    etc.

i.e. 20 Cycles for the first check, and every 40 Cycles thereafter.

Now because we need to check the value on *specific* cycles, we need to know what the state of the register X is on each cycle, even if it's not a cycle where the command is executed.

So this means the implementation would look something like:

    WHILE the program is running...

      FETCH the Current Opcode

      DECODE the Current Opcode

      FOR each Interim Cycle

        IF the Cycle is 20, or increments of 40 from 20

          Add to the SUM : Register X * Cycle

        END

      EXECUTE the Current Opcode

    END

And the Cycle check is best handled with Modulo.  So if we bring it all together...

    Cycle = 0

    Sum = 0

    FOR each Opcode

      IF the Opcode is ADDX, set the Cycle Delay to 2

      IF the Opcode is NOOP, set the Cycle Delay to 1

      WHILE the Cycle Delay > 0

        Add 1 to the Cycle

        Subtract 1 from the Cycle Delay

        IF (Cycle - 20) % 40 is 0

          Sum = Sum + (Cycle * X)

        END

      WEND

      IF the Opcode is ADDX, add the ADDX value to X

    NEXT


**Part 2**

Part 2 uses the Cycle Counter in a slightly different way.  It uses it as a pixel position for a display that is 40 pixels wide, 6 pixels high.

The puzzle itself gives a decent representation of this:

    ROW 0: Cycle   1 -> ######################################## <- Cycle  40
    ROW 1: Cycle  41 -> ######################################## <- Cycle  80
    ROW 2: Cycle  81 -> ######################################## <- Cycle 120
    ROW 3: Cycle 121 -> ######################################## <- Cycle 160
    ROW 4: Cycle 161 -> ######################################## <- Cycle 200
    ROW 5: Cycle 201 -> ######################################## <- Cycle 240

Note: I use 0 based row's to simplify things later on.

So from this we really need to know for a given cycle:
- What row are we on.
- What pixel position are we on.

This is because when we compare to X, X will only be able to light a pixel if it's between 1 and 40.

To calculate the row, we just need to divide by the Width (40).

    ROW = Round Down ( (Cycle - 1) / WIDTH)

The reason we subtract 1 is because 40 / 40 = 1, meaning we'd have end up choosing the wrong row for Cycles 40, 80, 120, 160, 200 and 240.

Rounding Down ensures any values between 0 .. 39 all evaluate to 0, 40 .. 79 all evaluate to 1 etc.

Next is the Pixel Position, the column.

    COL = Cycle - (ROW * WIDTH)

So all we're doing there is subtracting the total number of pixels in rows that occur before our current row.  So if we're on row 0, we're going to subtract 0 pixels from our Cycle.  If we're on Row 2, we subtract 80 pixels from our Cycle etc.

We also don't need to subtract 1 at this point, the reason is the Cycle is 1 based, so is X, so we want to keep this consistent.

So now we know the pixel position, we need to also consider that X is 3 pixels wide, with the left-most pixel being X, the middle pixel being X + 1, and the right-most pixel being X + 2.

So to check the COL is in range it's:

    COL >= X AND COL <= X + 2

With that in mind, let's bring it all together...

    Cycle = 0

    Image =
    [["                                        "],
     ["                                        "],
     ["                                        "],
     ["                                        "],
     ["                                        "],
     ["                                        "]]

    FOR each Opcode

      IF the Opcode is ADDX, set the Cycle Delay to 2

      IF the Opcode is NOOP, set the Cycle Delay to 1

      WHILE the Cycle Delay > 0

        Add 1 to the Cycle

        Subtract 1 from the Cycle Delay

        ROW = FLOOR ( (Cycle - 1) / WIDTH )
        COL = Cycle - (ROW * WIDTH)

        IF (X >= COL) AND ((X + 2) <= COL)

          Image[ROW][COL] = "#"
        
        END

      WEND

      IF the Opcode is ADDX, add the ADDX value to X

    NEXT

**Python vs C++ vs C**

**Python**

Follows the solution as-is.  The main Python'ism here is that strings are immutable, so instead of starting with a "blank" display of 40x6, we actually start with 0x6, and add unlit pixels or lit pixels as we process the input data.

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

The nature of this puzzle means this also follows the solution as-is, and because strings are just arrays in C, we can index it as a 2D array.  The only nuance is that to simplify output, a NUL terminator is added.