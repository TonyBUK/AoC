# Day 2

https://adventofcode.com/2016/day/2

This is actually more similar to Part 1 than you might think, but with three differences.

1. We're now just moving N/E/S/W, except E/W are referred to as L/R respectively.
2. We only ever move one space at a time
3. Our movement is limited to the keypad

**Part 1**

The basic algorithm is:

    FOR each set of moves

      FOR each move in the current set
      
        Set the new Position to U/D/L/R based on the move
        
        Limit the Position to the Keypad
        
      NEXT
      
      Add the current Keypad Value represented by the current Position to the Pass Code
      
    NEXT
    
The main thing of interest is that in the previous task, I set Up to +1 in the Y axix, and Down to -1 in the Y axis, since that's how we tend to visualise it, but the problem is, when dealing with Y as an offset into an array, it's actually the other way around.

    9 8 7
    7 5 4
    3 2 1

In terms of a 2D Array, this will be indexed row, then column, and positive is down / right for row/column respectively meaning:

     Y  X    Keypad Value
    [0][0] = 9
    [0][1] = 8
    [0][2] = 7
    [1][0] = 6
    [1][1] = 5
    [1][2] = 4
    [2][0] = 3
    [2][1] = 2
    [2][2] = 1

So we need to keep track of a Y position, and an X position.  These can never go negative, and can never be larger than the number of rows (for Y) and number of columns (for X).

Incidently, questioning whether Y increases or decreases when we move up is a very common occurrence, and the answer is always the same I'm afraid, check the documentation of whatever it is you're using.

**Part 2**

This is basically the same, but adds a wrinkle.  The Keypad is no longer a grid.  No problem, we'll make it a grid anyway, meaning:

        1
      2 3 4
    5 6 7 8 9
      A B C
        D

becomes...

    . . 1 . .
    . 2 3 4 .
    5 6 7 8 9
    . A B C .
    . . D . .

The revised rule is that not only does Y/X have to be within the Rows/Columns of the Grid, it can't end up on an invalid position, represented above with dots.  Think of it as walls of a maze if you will.  So the core loop is very slightly adjusted:


    FOR each set of moves

      FOR each move in the current set
      
        Set the new *possible* Position to U/D/L/R based on the move
        
        Limit the *possible* Position to the Keypad
        
        IF the *possible* Position is Valid
        
          Update the Position to the *possible* Position
          
        END
        
      NEXT
      
      Add the current Keypad Value represented by the current Position to the Pass Code
      
    NEXT

The nice thing is that loop also solves Part 1 as well.

**Python vs C++ vs C**

**Python**

The main thing of note here is that Python has a very nice value called None.  This can be tested for, and will never accidently correspond with any other value, and by that I mean if you tried the equality:

    assert(0 == None)
    
The assertion would fail, because None is distinct from any value, meaning it's perfect for marking positions as invalid for Part 2.  In languages like C/C++, the closest equivalent would be NULL, except NULL is just another way of saying 0.

I appreciate it's moot as 0 is never used on the Keypad, but I do typically like the idea that the solutions could grow if needed.

**C++**

**C**
