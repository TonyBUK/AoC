# Day 19

https://adventofcode.com/2015/day/19

Whilst it may not be as obvious when looking at the puzzle, but this is at heart, a path finding puzzle.  Part 1 is really a primer to give an idea just how quickly the number of permutations will become unmanagable when we perform multiple iterations, which is exactly what Part 2 requires.

**Part 1**

So starting with Part 1:

    FOR each Rule String
      
      Find all instances of the value we need to replace in the Calibration String
      
      FOR each instance we found
      
        Substitute the current instance in the Calibration String
        
        IF this creates a unique string, add to the list of unique strings
        
      NEXT
      
    NEXT

    Count the number of unique strings

**Part 2**

Part 2 is where things get more interesting.  Now the core answer is simple:

    Starting with the input list consisting only of: "e"

    RECURSIVE FUNCTION:

      FOR each string in the list of strings to process

        Find all permutations if the current string using the Rules algorithm from Part 1

        IF the Calibration String was in the permutations
          RETURN the Depth of the RECURSIVE FUNCTION
        END
        
        RECURSE with the permutations

      END

The problem is of course this will take an absurd amount of time.  So to start with, let's make things easier, with a few simple steps.

1. Instead of starting from "e", start from the calibration string and work our way towards "e", this should help reduce just how much we can actually diverge.
2. Swap over the rules, what would have been a target string before is now the string we're looking for and vice versa.

Even with this, it will still take a stupidly long time to find, however we can make a few assumptions about the puzzle itself, and here's where my solution may be incomplete or unsatisfying...

1. Assume that the puzzle is constructed such that there's really just one core route to the solution.
  - i.e. the only variability is ordering of inputs, not the inputs themselves.
2. Assume by and large that larger substitutions are preferable, being substitutions that reduce the string size by the most (i.e. take the greedy route)

If this does prove to be incomplete, then I imagine some sort of Dijkstra'esque solution would be the way to go, giving a heuristic score for sorting which string to process next.

**Python vs C++ vs C**

**Python**

Chances are this puzzle is the first time you'll realise Python has a default depth limit of 1000!  This uses the trick of using math.inf for the limit checking of the solution.  It's somewhat negated by the fact we accept the first solution.  I did toy with caching results/depths, but found it really didn't do that much, in that as the number of permutations exponentially grew, the overhead of insertion into the dictionary cache basically became a wash with the overhead of finding other solutions.

**C++**

**C**

