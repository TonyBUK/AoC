# Day 11: Radioisotope Thermoelectric Generators

https://adventofcode.com/2016/day/11

At heart a path finding problem with circular states (i.e. you can make several moves and just end up back where you started).  Wheras with most puzzles, the difference between the fastest/slowest execution times (if we normalise out hardware/language) is typically quite small, this is the kind of puzzle where you'll have some people boasting fractions of a second, and others lamenting 3+ hour solve times, and in many cases, the solutions themselves may even basically follow the same approach...

Very much a puzzle where the devil is in the detail.

**Part 1**

For this puzzle, I've gone with a DFS approach with aggressive optimisations/caching.  Arguably a better approach would be a BFS approach, and we can sort by some arbitrary metric, such as proximity of all items to the 4th floor or something of that ilk.  I went with DFS for simplicity, but suffice as to say, none of the optimisations I'm listing are unique to DFS.

So let's start with the core algorithm:

    RECURSIVE Function

      Check the current Floor state to see if it resulted in a radiation leak, if so, RETURN with the current best solution
      
      IF the current Floor state to see if it resulted in a win condition
      
        IF the current move count is better than the current best solution
        
          RETURN this solution as the new best solution
          
        ELSE
        
          RETURN the current best solution
          
        END
      
      END

      Create a combinational list of all possible unique pairs of Generators/Microchips
      
      FOR each possible Pair and also each individual item
      
        Try to move up a Floor (if legal) and Recurse, incrementing the move count by 1, store the result of this as the current best solution
        
        Try to move down a Floor (if legal) and Recurse, incrementing the move count by 1, store the result of this as the current best solution
        
      NEXT
      
    END

The big gotcha here is that we can end up just performing the same motions over and over again, with us looping indefinately and never solving the puzzle.

So the first optimisation: Caching the game state, and exiting if it's one we've seen before in a "better" solution (i.e. have we taken more moves just to get to a game state we've seen already), pair goes up, pair goes down, pair goes up, pair goes down.

So just keep a cache of the elevator position and what generators/microchips were on each floor right?  Well, that'd work, but is actually not aggressive enough to achieve a decent execution time.  What we need to realise is just how unimportant the specific chips/generators actually are.

Consider the following two states, one from the example on Advent of Code, and one is very slightly modified:

    F4 .  .  .  .      F4 .  .  .  .  
    F3 .  .  LG .      F3 HG .  .  .  
    F2 HG .  .  .      F2 .  .  LG .  
    F1 .  HM .  LM     F1 .  HM .  LM 

It really doesn't matter that in one instance, the Lithium Generator is on Floor 2 and the Hydrogen Generator is on Floor 3, and in the other, the Hydrogen Generator is on Floor 2, and the Lithium Generator is on Floor 3, both scenarios would take 11 moves to solve.

So what we need to do is represent the game state in such a way that the given examples would be the same.

What I'd propose is splitting this into one state for all Generators, and one for all Microchips.

    F4 .  .  .  .      F4 .  .  .  .  
    F3 .  .  LG .      F3 .  .  .  .  
    F2 HG .  .  .      F2 .  .  .  .  
    F1 .  .  .  .      F1 .  HM .  LM 

And then, we replace each of the items with the floor it's counterpart is on.

    F4 .  .  .  .      F4 .  .  .  .  
    F3 .  .  0  .      F3 .  .  .  .  
    F2 0  .  .  .      F2 .  .  .  .  
    F1 .  .  .  .      F1 .  2  .  3  

This means if we go all the way back to:

    F4 .  .  .  .      F4 .  .  .  .  
    F3 .  .  LG .      F3 HG .  .  .  
    F2 HG .  .  .      F2 .  .  LG .  
    F1 .  HM .  LM     F1 .  HM .  LM 

If we apply the same approach then sort each floor from lowest to highest, what we end up with is

    Generators         Microchips
    F4 []              F4 []
    F3 [0]             F3 []
    F2 [0]             F2 []
    F1 []              F1 [2,3]

Ok... so optimisations all done right?  Hah!  Just getting warmed up.

Next up, let's revise this bit: Create a combinational list of all possible unique pairs of Generators/Microchips

Say we start on the ground floor and we have 10 pairs of microchips/generators, we actually can disregard 9 of them since the outcome would be identical anyway.  We only care about important pairs, as such if there's more than one pair on the floor, ignore all but one pair.

Next up, we want the general trend of the generators/microchips to be upwards, so really, we want to change the main loop into four smaller steps.

1. Firstly, try to move all unique pairs upwards
2. Next, try to move all unique items downwards
3. Next, try to move all unique items upwards
4. Finally, try to move all unique pairs downwards

Plus this is the basic pattern you'd follow anyway.  Moving 4 items upwards would be:

-   2 up (2 on floor 2, 2 on floor 1)
-   1 down (1 on floor 2, 3 on floor 1)
-   2 up (3 on floor 2, 1 on floor 1)
-   1 down (2 on floor 2, 2 on floor 1)
-   2 up (4 on floor 2)

i.e. for the most part, this is the main sequence you'll use.

Next up, we want to start discarding no hoper paths, which will become prominent once we find *any* solution, and that is if we take the most *optimistic* projection of how many moves we need to solve the puzzle (even if it isn't legal), would that actually improve the solution, if not, give up.

So what's a way of calculating this...

Well first consider a pair of items on the ground floor.  We need 1 move to move them all to the next floor, nice and easy.

What about 3 items?  This is now 3 moves (2 up, 1 down, 2 up).

Similarly 4 items is 5 moves (2 up, 1 down, 2 up, 1 down 2 up)

So we can approximate this as :

    0 items     = 0 * floors to move
    1 - 2 items = 1 * floors to move
    3+ items    = (3 + ((items - 3) * 2)) * floors to move

All done now right??????? Right???????

.... Yes.

So this is what we end up with after all of that:

    RECURSIVE Function

      Check the current Floor state to see if it resulted in a radiation leak, if so, RETURN with the current best solution

      IF there's not enough moves left from the current best solution in order to win using an optimistic projection, RETURN with the current best solution
      
      Calculate the Game State (Elevator Position + Floors)
      
      IF this Game State has been seen in the Game State Cache
        IF this Game State was seen in a better solution (less moves)
          RETURN with the current best solution
        END
      END

      Update the Game State Cache with Game State

      IF the current Floor state to see if it resulted in a win condition
        RETURN this solution as the new best solution
      END

      Create a combinational list of all possible important (all but one matching pair discarded) unique pairs of Generators/Microchips
      
      FOR each possible Pairs
        Try to move up a Floor (if legal) and Recurse, incrementing the move count by 1, store the result of this as the current best solution
      NEXT
      
      FOR each possible Unique Items
        Try to move down a Floor (if legal) and Recurse, incrementing the move count by 1, store the result of this as the current best solution
      NEXT
      
      FOR each possible Unique Items
        Try to move up a Floor (if legal) and Recurse, incrementing the move count by 1, store the result of this as the current best solution
      NEXT
      
      FOR each possible Pairs
        Try to move down a Floor (if legal) and Recurse, incrementing the move count by 1, store the result of this as the current best solution
      NEXT
      
    END


**Part 2**

With all those optimisations in place, all that's left is to add the two new pairs to the ground floor and re-solve, we've already done the hard work.

Note: Something that may be worth exploring is that it is posited that each new pair on the ground floor always adds on 12 moves to the solution, and certainly for my input data, Part 1/2 are 24 apart, however I'd need to convince myself this is accurate in the general case, and given the execution time is already subsecond on my setup, I'm not *that* worried for the time being.


**Python vs C++ vs C**

**Python**

Essentially implemented exactly as stated.  As array duplication is expensive, this manipulates the current solution by swapping items between floors, then swapping them back rather than performing a deep copy.  Execution time is actually a tad unpredictable as this extensively uses sets for fast insertion/removal, which means ordering when iterating is not guarenteed.


**C++**

This is the Python solution divided into classes.  With hindsight I'd have been better off storing generators/microchips in their own arrays rather than an array of structs which would have helped with having to split alot of handling of whether to process microchips/generators inside of loops rather than outside.


**C**
