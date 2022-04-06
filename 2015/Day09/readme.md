# Day 9: All in a Single Night

https://adventofcode.com/2015/day/9

This introduces:
- Handling Permutations
- Brute Force...

So this one can be solved in a number of ways.  Arguably this is a route finding problem, which would mean something like Dijkstra would be a suitable solution, however, the input set is small, as we can do a brute force attack, especially since we also want the longest path.

My solution does this recursively, as one thing recursion is very good at doing is handling exhaustive permutations.  The psueo code would be something like:

    RECURSIVE FUNCTION

    IF this is the first pass

      FOR each pair of places connected by a route
        CALL the Recursive Function for the route
      NEXT

    ELSE

      IF all locations have been visited
        IF this is better than the current Shortest/Longest total route
          RETURN the new Shortest/Longest total route
        ELSE
          RETURN the existing Shortest/Longest total route
        END
      END

      Note: The next check is an optimisation that speeds up processing, but isn't required for the solution

      IF we've visited all the same places before...
        IF the current score is the same worse than the previous time we were here
          RETURN the existing Shortest/Longest total route
        END
      END

      FOR each target location that's connected to the current location by a route
        CALL the Recursive Function for the route
      END

    END

    RETURN the Shortest/Longest total route

In a nutshell, when writing a recursive solution you need to:
1) Ensure that the Recursive Function will eventually end... In this case this is achieved by keeping track of places already visited when selecting a new target location.
2) Ensure the best answer at any given time gets correctly passed up/down the recursive stack, or if that's too tricky, you can consider storing the best answer outside of the function.
3) Ensure that nested calls of the Recursive Function don't corrupt data in the current nesting level.  For example, whilst we do want the Shortest/Longest total route to be updated by nested calls, we don't want the current state of visited locations to be updated.  This means really understanding what paramaters are references (i.e. where there's only a single copy of data between all layers) and which are copied (where each layer has its own unique copy
4) Optionally find ways to detect whether you're already in a state where a better answer is impossible, thereby eliminating unnecessary steps.

For this solution, it does abuse the input data size, being that there's only 8 unique places.  This means we can actually store whether we've visited all the locations in a single byte of data, using each bit of that data as a flag for whether we've visited a location.  Were there more locations, this might have needed to be an array, which has a complication, as lists in Python are passed by reference.

One thing that's instantly noticable with the solution is that I actually perform the whole recursion twice.  Whilst it might seem like calling it once and storing both the longest and shortest routes would be quicker, this would break item 4) in the list above, as shortest/longest are contradictary goals, meaning the only optimisation we'd be able to do is not processing any further if we've seen the *exact* same state before.

**Part 1**

This just invokes the recursive function notifying it that we want the shortest route.  It starts off by setting the "best" solution to infinity (this is just a value where, amongst other things, and time you use the less than operator to compare sizes, you're guarenteed to return true.  i.e. 1,000,000 < infinite equates to true.  In the C/C++ versions, the max integer value for the size is used, but it's in essence a very similar aim.  Then after that, it's only interested in states where the total distance is less than prior states.

**Part 2**

As per part 1, but now set to use the longest route.  This time instead of setting the "best" solution to infinity, and checking for shorter states, it now sets the "best" solution to 0, and checks for longer states.

**Python vs C++ vs C**

**Python:**

If you've ever searched for Python on youtube, you'll have invariably come across things like Top 10 Don'ts, and this has one of those.  Functions which have default values to parameters should never be lists, sets or dictionaries.  Instead these should be initialised to None as a parameter, and initialised inside the function.  The reason being, the parameter initialisation is a compile time construct, not a runtime, meaning if I were to have the function written as:

    def findRoute(places, routes, findShortest, currentScore = 0, bestScore = None, bestScores = {}, visitedMask = 0, completeMask = 0, currentPlace = None) :

bestScores would *only* be an empty dictionary the *first* time I call findRoute.  i.e.

    findRoute(places, routes, True) # will work
    findRoute(places, routes, True) # won't work as bestScores is now populated,
                                    # meaning we'll always have our processing
                                    # halted by step 4) of my guidance above (the
                                    # optimisation to eliminate unnecessary steps).
    findRoute(places, routes, False) # Still won't work

**C++**

This essentially follows the Python algorithm, but with the C++ boiler plate code added.  In some ways I find the C++ code a tad clearer, as whilst you can type parameters in a Python function, it's typically not to the detail of a language like C/C++, and even affords nicities such as const correctness, whereby I can now far more easily distinguish between data that's changed up/down the stack, versus data that will remain constant.

One decision some may find odd is the data buffer for the best scores is passed by pointer, not reference.  One weakness of call by reference is the inability to pass NULL, which on the one hand guarentees there's actually going to be an instance of something when passed in to the function, but in this instance, we want to force it to start from an initial state.  By pointer passing, the first call to findRoute creates the best scores buffer, and neatly keeps the scope inside the recursive function only.  The alternative would be to declare the best scores buffer outside of this scope which would be more annowing.

**C**

C showing a few more signs of age.  This example really needs two hash maps.  One to index routes, and one to keep track of previous states.  The issue with the code is two-fold:

- The tree hash map itself is never cleaned up, it really needs to walk the nodes and delete all leafs, as the library selected irksomely doesn't support deleting the root node.
- The hash map itself buffers pointers to an array which stores the data itself.  This is fine when the size of the hash map is known in advance, if not, we get to this very annoying scenario whereby traditionally, you'd allocate more memory, and copy over the data deleting the old buffer, this is fine for the array, but the hash map would then point to destroyed data.  What this means is what I've been sort of dreading, the library hash maps won't cut it.  So moving forwards, I'm going to have to roll my own, as whilst for this task, it's not necessary, for future tasks, it's impossible without it.

Note: I may re-open prior tasks with have used search.h, as I've suspicions as to how portable this is, so if I do end up rolling my own, then back porting it would probably make sense.
