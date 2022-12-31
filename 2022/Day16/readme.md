# Day 16: Proboscidea Volcanium

https://adventofcode.com/2022/day/16

These ones are always enjoyable, where the problem is deceptively simple, even has a relatively small data set compared to other puzzles, but is the kind of puzzle that was solve in minutes/hours if not done right!

**Part 1**

So this asks what order to visit a series of tunnels in order to release valves, and to calculate the optimal route, which favours visiting the valves sooner rather than later.  Every movement, and every valve open incurs a cost.

Firstly hopefully by now, you'll know DFS is not the way forward here, well, you can use it for the easy bit, but we'll get to that...

Now onto the puzzle at hand.  On first glance, it is actually pretty deceptive.  It seems like a path finding puzzle, so let's break out Dijkstra and call it a day right?

Unfortunately not, path finding is the easy bit, trivial even.  The problem is not how do we get from one node to another, the problem is what order to we travel from one node to another, so we can really simplify this as the task is essentially going to be:

Starting at "AA"
  - Travel to a non-zero valve and open it
  - Travel to the next non-zero valve and open it
  - Travel to the next non-zero valve and open it
  - etc.

So rather that thinking of this puzzle as:

Minute 1: Move one space
Minute 2: Move one space
Minute N: Open Valve
Minute N+1: Move one space

etc.

We need to determine how many moves are needed to go from one valve to another.  i.e.

Minute 1    : Calculate time to move to next valve and open it.
Minute N + 1: Calculate time to move to next valve and open it.

To do this we need to calculate an *exhaustive* set of routes, which as a minimum need to get from...
  - The start to all of the tunnels which, when turned, will release pressure.
  - But you can make it complete as it's only getting from 59 points to 58 other points with my puzzle input.
  - This is the bit you can use DFS for if you must.

So to bring it together algorithmically, let's solve this with BFS.

    Reset the Best Pressure Score

    FOR each Open Valve
      Add the Target Valve, and the pressure score, and the time to get there/open it, and the remaining Open Valves to the Queue
    END

    WHILE the Queue is not empty

      Pop an entry from the Queue

      IF the Pressure Score > Best Pressure Score
        Update the Best Pressure Score
      END

      FOR each Remaining Open Valve

        Add the Target Valve, and the new pressure score, and the time to get there/open it, and the remaining Open Valves to the Queue

      NEXT

    WEND

**Part 2**

So go back to Part 1 and keep track of two things in the queue right... right?  If you've answered yes, see you in 30 minutes when your program finally finishes running.

And don't worry, this isn't me smugly proclaiming this as someone who knew that this would be the case, this is from experience.  Before I got to a fast(ish) solution I tried:

1. Moving both at the same time 1 minute at a time.
2. Moving both at the same time using the Part 1 algorithm and arbitrating movement between them to force the slower path to tie up with faster paths (i.e. if the elephant will arrive in 5 minutes, and the human in 10, the time ticks on by 5 minutes for a round, and the human only travels 1/2 the route).

Both were slow, but the arbitration code for 2. actually outweighed the apparent benefits.  And that's why we profile!  Know your bottlenecks, before optimising.  Just because something feels like something should be faster doesn't mean it is...

With that in mind... what we want is to run Part 1 almost *exactly* as-is, but with two changes.

1. Starting Minutes should now be 26 instead of 30.
2. We no longer care about the Best Pressure score overally, we care about the Best Pressure Score for a given set of closed valves.
- Note: We absolutely do not care about the order the valves were closed, only that they are closed.  Do if hash mapping etc., make sure the valves are either sorted, or stored in such a way that removes/normalises ordering.
- In other words: [AA, BB, CC] is equivalent to [AA, CC, BB] and [BB, AA, CC] etc.

i.e.

    Reset the Best Pressure Score

    Reset the Closed Valve Pressure Scores

    FOR each Open Valve
      Add the Target Valve, and the pressure score, and the time to get there/open it, and the remaining Open Valves to the Queue
    END

    WHILE the Queue is not empty

      Pop an entry from the Queue

      IF the Pressure Score > Best Pressure Score for the Current Closed Valves
        Update the Best Pressure Score for the Current Closed Valves
      END

      FOR each Remaining Open Valve

        Add the Target Valve, and the new pressure score, and the time to get there/open it, and the remaining Open Valves to the Queue

      NEXT

    WEND

Once we've done this, what we have is all the best ways a single person (or elephant) could close each of the possible valve cominations.

So where does this get us, well, the trick is that the Elephant Movement and Our Movement are entirely independent of each other, so all we need to do is...

    Reset the Best Pressure Score

    FOR all entries in the Closed Valves Data Set (Us)

      FOR all entries in the Closed Valves Data Set (Elephant)

        IF Our Closed Valves don't overlap with the Elephant's Close Valves

          IF combined Best Pressure Scores for Us and the Elephant is Better than the Best Pressure Score

            Update the Best Pressure Score

          END

        END

      NEXT

    NEXT

There are a few optimisations, such as only checking Closed Valves for the Elephant that haven't already been considered for Us.  We can also test to see with the remaining sets whether the Best Pressure Score is *potentially* beatable and so on.  But overally, this should get you into a relatively quick solution.  Not instant, but near instant.

**Python vs C++ vs C**

**Python**

Follows the solution as-is.  Uses frozen sets as the unordered keys for the Closed Valves dictionary.

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

Here's where we can get creative... if we apply a limitation to the inputs that, when all is said and done, there will be a sane number of tunnels with non-zero flow rates, we can be a bit creative with how we work around the lack of hash maps.  In my particular input, I end up with 13 non-zero flow rates, and the start tunnel, which means if I were to treat each of these tunnels as a unique bit in a bit-mask, i.e.

    1 : 0000000000001
    2 : 0000000000010
    3 : 0000000000100
  
  etc.

  Then I can store all possible permutations of unique tunnels with a 16,384 element array.  Wasteful, but *fast*... very very fast.

  It also neatly means for Part 2, instead of testing the intersections of two data sets, we have a single bitwise AND on two integers.

  This gives us a 10,000% delta between C and Python.  However, this is absolutely *not* a scaleable solution.  I believe it should solve any permutation of the AoC puzzle, but wouldn't solve a general case input unlike the Python solution.