# Day 13: Knights of the Dinner Table

https://adventofcode.com/2015/day/13

This introduces:
- Specialised Language Features

So the core idea here is to figure out all the permutations of the seating plan and brute force them.  Ideally, we can consider discarding seating plans we've already tried, which is slightly obfuscated by the fact the table is circular but we can absolutely handle it with one weird trick (Advent of Code puzzle makers hate this).

Imagine you're trying to find the permutations of A,B,C in a circular buffer.  This means the following are equivalent:
- A,B,C : B,C,A : C,A,B
- A,C,B : C,B,A : B,A,C

Now on the face of it, we could store permutations we've tried, and for each new permutation, keep rotating it until we've established whether it matches an existing permutation... OR... only allow permutations where "A" is in the first position.  For the puzzle at hand, this nets around a 10x speedup!

The only other complication is that we need to interrogate the seating plan in such a way that we can determine who is left/right of the current person, except this gets mildly complicated when the person is either the first or last in the seating plan, since we're likely to under/overflow the array.

So for the array A,B,C

We want the following:

    Seat Position 0 (Person A) : Left = Seat Position 2 (Person C), Right = Seat Position 1 (Person B)
    Seat Position 1 (Person B) : Left = Seat Position 0 (Person A), Right = Seat Position 2 (Person C)
    Seat Position 2 (Person C) : Left = Seat Position 1 (Person B), Right = Seat Position 0 (Person A)

This could be handled in logic, but a more elegant way is via modulo, where we can generalise this as:

    Left  = ((Seat Position + Number of Seats) - 1) MOD Number of Seats
    Right = ((Seat Position                  ) + 1) MOD Number of Seats

This will always return a seat number between 0 and the Number of Seats (-1).  After this it's just a matter of for each permutation, counting up the score, and recording the highest.

**Part 1**

Just running through the list with the names provided.

**Part 2**

As per Part 1, but we now add a dummy entry into the seating plan.  Only who can save the seating plan.  You have said me referring to you, the correct answer was you referring to me.

**Python vs C++ vs C**

**Python**

I cut Python quite a bit of slack here and used itertools to calculate the permutations, rather than solving it as a Depth First recursive solution.  What's nice is that this is returned as an iterable object, meaning Python isn't actually calculating the permutations up front, but rather interleaving one set of permutations each iteration of the seating plan loop, meaning it's only when the loop *asks* for the next permutation that it actually calculates it.

Plus there's one cheat I can do to the Left seat.  The whole purpose of the modulo arithmetic was to stop the index going negative, except in Python land, array[-1] is that last element, i.e. exactly what we wanted, so we only need to worry about the overflow for the right seat.

**C++**

To show it actually can happen, this is actually slower than the Python solution.  The primary culprit is the excessive copying/resizing, when performing the recursive walking of the permutations.

**C**

Whereas C with a much more bare metal approach to memory management again takes the speed crown.  This is effectively the C++ algorithm but without the dynamic array growth/shrinkage/copying (instead, just a separate variable indicating the size increments/decrements) to show just how much overhead that can add.

Of course as ever, I *could* rework the C++ algorithm to eliminate resizing to get speed more on par with C, but this is one of those cases where the more you optimize it, the more you tend to steer away from how one would typically implement it in the language, rather than an attempt for the fastest possible solution.
