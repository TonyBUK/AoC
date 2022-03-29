# Day 17

https://adventofcode.com/2015/day/17

This is similar to Day 15 in many ways.  We're looking for all unique permutations of the contains that give us a certain result, and we don't want to repeat any permutations:

i.e. using the data below, if we're trying to get to 15, there's two ways of doing this, the first 5 and the 10, and the second 5 and the 10.

    5
    10
    5

Simpliest way to do this is basically to walk down the indexes...

    FOR each container

      Set the current total based on the current container

      FOR each remaining container

        Set the current total based on the current container

        FOR each remaining container
          Set the current total based on the current container
        NEXT

      NEXT

    NEXT

So to walk this through:

    Outer loop considers 5 : Total is 5 (too small)
    
      Inner Loop considers 10 : Total is 15 so accept

        Innermost Loop Considers 5 : Total is 20 (too big)

      Inner Loop considers 5 : Total is 10 (too small)
      
        Innermost Loop has no remaining containers
      
      Inner Loop has no remaining containers
      
    Outer loop considers 10 : Total is 10 (too small)
    
      Inner loop considers 5: Total is 15 so accept
      
        Innermost Loop has no remaining containers
        
      Inner Loop has no remaining containers
      
    Outer loop considers 5 : Total is 5 (too small)
    
      Inner loop has no remaining containers
      
    Outer loop has no remaining containers

But as we can handle this for an arbitrary number of terms through recurson...

    RECURSIVE Function:
    
      IF the current total is our target total
        Update the number of solutions found for the current number of containers used
        RETURN
      END
      
      FOR each remaining containers

        IF the current total + the current container is not larger than our target total
          RECURSE with the remaining containers
        END
        
      NEXT

There are other solutions, such as trying all of the permutations in any order and doing some technique such as sorting/sets to filter duplicates, but in all honesty, that's adding complexity for what's demonstrably a much slower way of doing this.

**Part 1**

So here's where knowing both parts helps inform how this is written.  We can solve part two by splitting the number of solutions found by how many containers each used, and for part 1, just add up the total number of solutions regardless of how many containers were used.

**Part 2**

For part 2, we just need to consider the smallest number of containers used, and how many unique solutions were found.

**Python vs C++ vs C**

**Python**

Not much to say on this really, this is an algorithm problem, not really one where a given language will make that big a difference.  There's probably an easy way of doing the same thing with something like itertools, however the ability to quickly discard "dead" paths using recursion is still probably the way to go.

**C++**

Essentially the Python solution, but slightly less elegant with respect to getting the min/total values.

**C**

Slightly different route, but that's mostly because hash maps aren't the most natural fit for C.  Instead of using a hash map for all possible solutions, I just store an array, initialised to 0, representing all possible counts from the input data set (i.e. the most items to reach a target could ever be is if all the containers are used), so slightly more effecient than the other solutions with respect to processing time.

Also fun fact, todays solution exposed a nasty malloc bug introduced in all the previous solutions involving buffering the start points of each line.

    kLines             = (char**)malloc((nFileSize)    * sizeof(char));
    
The problem is this is storing an array of pointers, it should have been:

    kLines             = (char**)malloc((nFileSize)    * sizeof(char*));

And yes, I've gone back and fixed the code in the prior days!  That's one of the joys of C, runtime errors often manifest in the weirdest of ways.  It just so happened all the other days had really long lines, making this attempt at guessing the line count (i.e. being lazy) make up for the fact the type size was a fraction of the size it should have been (8 bytes on a 64 bit system).  This puzzle had really short lines (2 digit numbers), causing this to ultimately be the day that exposed the bug, considering just how many days I got away with it.

**Times**

    Language : Time      : Percentage of Best Time
    =========:===========:=========================
    Python   : 25.74ms   : 1792.35%
    C++      : 1.46ms    : 101.59%
    C        : 1.44ms    : 100.00%
