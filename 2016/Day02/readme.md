# Day 2

https://adventofcode.com/2016/day/2

This is actually more similar to Part 1 than you might think, but with three differences.

1. We're now just moving N/E/S/W, except this is now referred to as U/R/D/L respectively.
2. We only ever move one space at a time.
3. Our movement is limited to the keypad, whereas in Day 1, it was moving in an infinite space.

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

          --X--
          0 1 2

    | 0   9 8 7
    Y 1   7 5 4
    | 2   3 2 1

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

The revised rule is that not only does Y/X have to be within the Rows/Columns of the Grid, it can't end up on an invalid position, represented above with dots.  Think of the dots as impassable walls if that helps.  So the core loop is very slightly adjusted:


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

Note: Whilst I've not done it here, the addition of walls actually affords us a possible cheat.  What if we extended the boundary of the grid...

    . . . . . . .
    . . . 1 . . .
    . . 2 3 4 . .
    . 5 6 7 8 9 .
    . . A B C . .
    . . . D . . .
    . . . . . . .

Why might you do this?  Well, it means that as long as we stard on one of the number/letters, we can never go out of bounds.  The reason is there's no Up/Down/Left/Right movement that would put us outside the grid, it would just put us on an invalid value.  Meaning the loop could be optimised to:

    FOR each set of moves

      FOR each move in the current set
      
        Set the new *possible* Position to U/D/L/R based on the move
        Note: The Keypad design is such that the Position can never go outside the Grid
        
        IF the *possible* Position is Valid
        
          Update the Position to the *possible* Position
          
        END
        
      NEXT
      
      Add the current Keypad Value represented by the current Position to the Pass Code
      
    NEXT

Since we no longer need to limit the position to the grid due to the bounding walls.  This is actually a super common trick, sacrificing a little bit of memory to simpilify the processing.  And hopefully demonstrates how sometimes, the design of the input data itself is just as important as the algorithms to process it.

**Python vs C++ vs C**

**Python**

The main thing of note here is that Python has a very nice value called None.  This can be tested for, and will never accidently correspond with any other value, and by that I mean if you tried the equality:

    assert(0 == None)
    
The assertion would fail, because None is distinct from any value, meaning it's perfect for marking positions as invalid for Part 2.  In languages like C/C++, the closest equivalent would be NULL, except NULL is just another way of saying 0.

I appreciate it's moot as 0 is never used on the Keypad, but I do typically like the idea that the solutions could grow if needed.

**C++**

Moved the position class to its own file, and extended it slightly to add position clamping.  The main solution now doesn't look anywhere near as bloated.  And as alluded to in the Python solution, I've used a different character "." to represent an unmoveable space.  The logical thing to check might be NULL ('\0'), but because in C/C++, strings are nul terminated, this ends not being trivial, so "." was a good enough compromise.

**C**
