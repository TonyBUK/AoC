# Day 1

https://adventofcode.com/2016/day/1

Compared to 2015's day 1, this is a bit of a jump.  At it's core, we need to keep track of position and direction.  So we need to understand how a left/right turn impacts each of the cardinal directions (N/E/S/W)

- North : Left turns face West, Right turns face East
- East  : Left turns face North, Right turns face South
- South : Left turns face East, Right turns face West
- West  : Left turns face South, Right turns face North

As well as this, we need to understand the basics of what it means to move in each direction.  So if we're moving North, we're adding 1 to the Y position, if we're moving West, we're subtracting 1 from the X position and so on.

So we could do something like:

    IF Direction = North
      Y + 1
    ELSE IF Direction = East
      X + 1
    ...
    
But if we express all positions as Vectors, consisting of an X and Y offset, we no longer need the IF statements.

    North : [ 0,  1]
    East  : [ 1,  0]
    South : [ 0, -1]
    West  : [-1,  0]

The thing to note is at any time, only one of the values is set to a non-zero value, and the non-zero value is always either +1 or -1.

Now we can just do:

    [X, Y] + Position Vector

Next we just need to understand Manhatten Distance: https://en.wikipedia.org/wiki/Taxicab_geometry

This is an absolute staple of Advent of Code.  In most real world scenarios we'd actually use the Pythagorean theorem, https://en.wikipedia.org/wiki/Pythagorean_theorem, but that very specifically isn't used in Advent of Code... ever.  Main reason, it involves floating point, which Advent of Code never requires, presumably because accepting solutions based on floating point would be a nightmare: https://en.wikipedia.org/wiki/Floating-point_error_mitigation

Manhatten Distance on the other hand stays purely in the integer domain, so everyone's answer should always match!

**Part 1**

For Part 1 we can optimise the loop somewhat.  We just need to travel a certain distance in a certain direction, making the core algorithm:

    FOR each move
    
      Change Direction based on the current moves turn direction
      
      Move in the current direction by multiplying the vector for the current direction by the current moves distance.

    END

This is why having the position vectors as storing only -1, 0 or 1 is so important, as it lets us directly multiply by the distance.  Then all that's left is to is the Manhatten Distance, which is basically:

    abs(X Position) + abs(Y Position)

Note: This is absolutely not the same as:

    abs(X Position + Y Position)

**Part 2**

So there's two ways of solving this part, since we now actually do need to keep tabs on where we've been.  Method 1 is to keep tabs on each position we end up on before we turn, and connect each with a line, checking to see if any lines cross.  This is doable, and there are puzzles in AoC that do require this, however, we can be lazy and use method 2.

Method 2 is:

    FOR each move
    
      Change Direction based on the current moves turn direction
      
      FOR each whole block move we need to move based on the current moves distance
      
        Move 1 block in the current direction
        
        Determine if we've been on this position before, if so, we have our answer
        
        If not, add this position to the list of positions we've already been to
      
      END

    END

We lose some of our efficiency gains from Part 1, but it is a simpler solution.  But be warned, it only works because we're moving relatively small distances (mostly <= 5 except for a few larger moves).  What would happen if we were asked to move 1,000,000 blocks in a given direction, what would that do to this solution...

**Python vs C++ vs C**

**Python**

This adds quite a few staples of Python pretty much immediately.

We have:

- Lists : These are also known as arrays, and are essentially a list of things that we can access in a predictable order.
- Dictionaries : These are also known as maps, and are sort of like a list, but indexed by pretty much anything we want to, in this case, I've used text.
- Sets : These are relatively unique to languages like Python/Ruby etc., and is effectively an unordered data set (in that we can't guarentee what order we'll read them in), and each item in the data set *has* to be unique.

So in terms of how I use these...

- Lists are used to store each move we need to make.
- Dictionaries are used to define the moves themselves
  - what direction are we turning, how far are we travelling.
  - It also defines how a turn effects our current direction, and how our current direction moves in X and Y.
- Sets are used to store all the places we've already been...
  - This doesn't actually have to be a set, but sets also have the nice property of being faster to test for duplicates, and faster to add new items compared to lists.

Plus this makes use of:
- List comprehension
  - Which allows an action to be used on anything iterable (such as another list) to generate a new list
    - For example, converting our input file into something we can use
- Zip
  - This is an extremely useful way of iterating on more than one thing at once, I use this to add positions for instance

The 2015 Day 1 challenge was positively basic in comparison!

**C++**

**C**
