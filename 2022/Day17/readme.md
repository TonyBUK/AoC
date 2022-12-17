# Day 17: Pyroclastic Flow

https://adventofcode.com/2022/day/17

This one gets into the heart of Advent of Code... a scaleability problem that whilst technically solveable using naive methods, cannot be solved that way.  In this instance, the causing factor is time.

And of course anyone looking at this is right, it's basically Tetris with more different shapes.

**Part 1**

So Part 1 is the classic gotcha... it'll give the problem, and it'll be trivially solveable with a classic approach.  The main issue is finding a way to describe the game state at any given time as we need to consider wall collisions and rock collisions for weird shapes.

It is possible to store this as a grid, but personally I'd recommend making it "rock centric".  Most og the grid is empty space, whereas all we really care about is where rocks are.

I'm going to assume you have something similar to sets/hash maps available, as I'll be using the X/Y co-ordinate of each element of a rock when it's come to rest.

So the basic algorithm is something like:

    FOR each Rock

      IF a rock needs dropping

        Initialise the Rock Left Position to 2, Highest Rock + 3

      END

      Get the Current Jet Stream Move

      IF the Move is to the Left
        IF the Rock Left Position < 0
          Indicate an X move is not possible
        END
      END

      IF the Move is to the Right
        IF the Rock Right Position > 6
          Indicate an X move is not possible
        END
      END

      IF the Rock, if moved would collide with another Rock
        Indicate an X move is not possible
      END

      IF the X move was possible
        Apply the X Move
      END

      IF the Rock Bottom is touching the Floor
        Indicate a Y move is not possible
      END

      IF the Rock, if moved down, would collide with another Rock
        Indicate a Y move is not possible
      END

      IF the Y move is possible
        Apply the Y Move
      ELSE

        FOR each Solid Position within the Rock
          Add that Position to the Collision Map
        END

        IF the top of the rock > Highest Rock
          Set the Highest Rock to the Top of the Rock
        END

        Move to the Next Rock

        Indicate a Rock need dropping

      END

      Move to the next position in the Jet Stream

    NEXT

    RETURN the Highest Rock

Testing collision is basically:

    FOR each part of the current Rock
      IF the current Part is soild
        Get the X/Y of that Part
        IF it's in the Collision Map of Other Rocks
          Return True
        END
      END
    NEXT
    Return False

And for the first part, repeating that 2022 times is fine.

**Part 2**

What isn't fine is repeating it 1,000,000,000,000 times, or One Trillion to give it a more recognisable term.  What we can instead rely upon, and this is a staple of the Advent of Code puzzles, is loops.  Specifically, if you play the same inputs repeatedly, eventually, the game will start repeating itself.  Once we've found that we don't *need* to drop One Trillion rocks, we just need to know how times the game would repeat itself is we dropped One Trillion rocks and calculate the Highest Rock from that.

So now the problem is how to describe a loop.  Well, as long as it's a simple repeating loop, it'll just be two values:

Offset and Interval.

Offset is how much of the game did we need to play before the first loop began.  This assumes that the first few moves actually don't get repeated (primarily as it's played when no rocks are dropped, which is a state that is unlikely to be repeated).

Interval is how much of the game ween need to play before the first loop ends, and the second loop begins.

So the essence of the puzzle is to come up with a trivial way of describing the state of the game.

So let's consider how it functions.  We have a jet stream that determines the left/right moves.  We have the rocks that have already set, although in truth, it's only the highest rock in each column that's important, as realistically these are going to be the reason the rocks stop moving.  We also need to know what rock is being dropped.  That's really about it.

Note: In truth, it might need to be more than just the highest rock, as it's not impossible that a small enough block could wiggle through some weird patterns and get stopped by a lower block, but let's assume that isn't the case for all our sanities.

We also need to know the effect of how each of the moves made would impact the height of the highest rock, as a delta, as we need a way to correlate any loops into actual heights.

So with all that in mind, the game now becomes:

    FOR each Rock

      Handle Dropping/Moving/Resting/Colliding the Rock as per Part 1

      IF a Rock has come to a rest

        Calculate the Peak Heights of all of the Columns

        Calculate these Peak Heights as a delta from the Current Highest Rock

        Store the difference between the Current Highest Rock and Previous Highest Rock in the Height Delta list

        Create a key with the Peak Heights, Jet Stream Position and Rock Type

        IF that key doesn't exist in the cache

          Add it to the Cache, and set the Value to the current Rock Count

        ELSE

          Set the Rock Loop Offset to the Current Value within the Cache

          Set the Rock Loop Interval to: Rock Count - Current Value within the Cache

          Indicate we've found the loop and stop processing the Rock Loop

        END

      END

    NEXT

    Using the Rock Loop Offset and Height Delta List, calculate the Initial Height

    Using the Rock Loop Offset, Interval and Height Delta List, calculate the Interval Height

    Set the Interval Count to: FLOOR((Target Rock Count - Rock Loop Offset) / Rock Loop Interval)

    Set the Interval Remainder to: (Target Rock Count - Rock Loop Offset) % Rock Loop Interval

    Using the Rock Loop Offset, Interval Remainder and Height Delta List, calculate the Remainder Height

    RETURN Initial Height + (Interval Count * Interval Height) + Remainder Height

All done!

**Python vs C++ vs C**

**Python**

Follows the solution as-is.

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

TBD - Playing catch-up with multiple solutions now as the puzzles get harder!