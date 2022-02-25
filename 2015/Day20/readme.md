# Day 20

https://adventofcode.com/2015/day/20

This particular puzzle is one that's fairly trivial to solve, but no necessarily trivial to solve in a sensible amount of time.

**Part 1**

The basic algorithm is something like:

    FOR each House

      Set the Total Presents to 0

      FOR each Elf (1 .. House)

        IF the House Number is Divisible by the Elf Number
        
          Add 10 * Elf Number to the Total Presents
          
        END

      END

      IF the Total Presents >= Our Target Present Count
      
        All done!

      END

    NEXT

The problem is that as house number goes up, so to does the number of Elves that we need to process.  However, we can refactor this slightly...

    FOR each Elf
    
      FOR each House the Elf can deliver to (limited to a sensible number derived from our Target Present Count)
      Note: This is *NOT* the absolute House Number, but is better described as, first house the Elf will deliver to, second house etc.
            The actual House Number can be found by multipling the House by the Elf Number, i.e.
            First Houses Number  = 1 * Elf Number
            Second Houses Number = 2 * Elf Number
            Third Houses Number  = 3 * Elf Number
            etc.

        Add 10 * Elf Number to the number of presents for the Current House Number (House Number * Elf Number)
        
        IF the Total Presents for the Current Elf is >= Our Target Present Count

          All done!

        END
        
      NEXT
      
    NEXT

What this neatly avoids is checking whether an elf *can* deliver to a house.  We just loop through multiples of the elf adding the present count as we go, and of course as the next elf can't deliver to the current elfs starting house, we know that's the largest it will ever get, making it the right time to compare against our target present count.

Now there are some possible optimisations, for example "limited to a sensible number derived from our Target Present Count", the smaller this number is, the faster everything runs.  And no doubt there's a far more "maths'y" solution here that will get it in one, however, this method seems fine for now, if something leaps out at me later on that I think is obvious/legible then of course I'll revisit this.  We also don't need to start counting from elf 1, we know Elf 1 will deliver 10 presents to every house, so if we assume every house starts with 10 presents, we can start with Elf 2, and skip the slowest Elf (twice as slow to process as any other elf).

**Part 2**

Here's where we've done ourselves *alot* of favours in Part 1.  Because the inner loop is based around the concept of "first house the elf can deliver to", "second house the elf can deliver to" and so on, it means it's perfectly suited to stop when we get to the "fiftieth house the elf can deliver to".

This now makes the algorithm:

    FOR each Elf
    
      FOR each the first 50 Houses the Elf can deliver to
      Note: This is *NOT* the absolute House Number, but is better described as, first house the Elf will deliver to, second house etc.
            The actual House Number can be found by multipling the House by the Elf Number, i.e.
            First Houses Number  = 1 * Elf Number
            Second Houses Number = 2 * Elf Number
            Third Houses Number  = 3 * Elf Number
            etc.

        Add 10 * Elf Number to the number of presents for the Current House Number (House Number * Elf Number)
        
        IF the Total Presents for the Current Elf is >= Our Target Present Count

          All done!

        END
        
      NEXT
      
    NEXT

So this actually ends up being a good deal faster!

**Python vs C++ vs C**

**Python**

Nothing overly fancy here, as this is really more of an algorithmic puzzle than one that leverages data structures etc.  As a little bodge, to try and minimise just how much gets executed in the inner loop, I sacrifice array element 0, and use array elements 1 onwards in order to *very* slightly simplify array indexing etc.

**C++**

Identical to Python, but signficantly faster (around 3x).  One of the perks of being closer to "bare metal".

**C**

Identical to C++/Python, and twice as fast as the C++ solution.  Major difference is a directly allocated RAM buffer rather than using STD Vector as the accessor/mutator.
