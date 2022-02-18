# Day 18

https://adventofcode.com/2015/day/18

This is the classic Conway's Game of Life, something that is another staple of the Advent of Code series.  The key to solving this one is relatively straight forwards...

    Ensure that the grid you're counting is *not* being updated at the same time.

Everything from that point onwards falls into the category of optimisation.  Let's consider just how far down the rabbits hole we can go...  So the simplest algorithm here is:

    Copy the Conway Grid to a Target Grid
    
    FOR each point in the Conway Grid
    
      IF the Light is off, count the neighbours, and if the count is exactly 3, turn the light on
      
      IF the Light is on, count the neighbours, and if the count is not 2 or 3, turn the light off

    NEXT

The problem is that there are 1000 points in the Conway Grid, and we want to re-run that loop 100 times, and counting neighbours is quite expensive, and will invariably result in reading each location around 9 times.

So maybe instead of doing this, we pre-calculate the Neighbour Count for all positions, knowing that this can only occur around On Lights.

So that would be something like:

    Copy the Conway Grid to a Target Grid
    
    Calculate the Neighbour Count for all Grid Elements using a list of On Light locations
    
    FOR each point in the Conway Grid
    
      IF the Light is off, if the pre-calculated neighbour count is exactly 3, turn the light on and add to the list of On Light locations
      
      IF the Light is off, if the pre-calculated neighbour count is not 2 or 3, turn the light off and remove from the list of On Light locations
      
    NEXT

This removes the counting from the loop, which for my input resulted in about a 3-6x speedup from counting Neighbours as we go.  But of course looking at this, you can't help but think that it's still going over *every* point in the Conway Grid, except we know interesting things only happen around On Lights anyway...

So just iterate over the On Lights and their neighbouring points.  Plus now if we're pre-calculating the Neighbour Count, we actually don't need to copy the On Light locations, since the whole reason we worked on a copy in the first place was the volatility of the Neighbour Count.

So this brings the whole thing down to:

    Pre-calculate the Positions and Neighbour Counts for all On Light Locations and their Neighbours
    
    FOR each of the Pre-calculated positions
    
      IF the Light is off, if the pre-calculated neighbour count is exactly 3, add to the list of On Light locations

      IF the Light is off, if the pre-calculated neighbour count is not 2 or 3, remove from the list of On Light locations

    NEXT

**Part 1**

Exactly as stated above...

**Part 2**

Same as Part 1, but the corners now have to forcibly remain on, even if the rules say to switch them off.

**Python vs C++ vs C**

**Python**

Sets and Dictionaries pay dividends here, albeit, whilst I've stuck with the set/dictionary solution as it's more in keeping with the Python approach, it isn't actually faster than the previous solution that used the grid with cached neighbour counts.  However it would be weird in Python to go with a half way measure between the grid based solution and hash map/set solution, so I've gone with the latter.

**C++**

**C**
