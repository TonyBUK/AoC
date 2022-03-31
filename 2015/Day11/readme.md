# Day 11

https://adventofcode.com/2015/day/11

This introduces:
- Number Bases
- More solution scaleability

Much like Day 10, this is partly a scaleability issue, where Part 1 will solve relatively easily using a brute force approach, whereas Part 2 will penalize any inherent slowness in Part 1.

This one can be solved a number of ways, I chose to solve it by treating the password as a base 23 number.  For those not familiar, most people are familiar with 1 or 2 number bases.  I've picked two I'm hoping you're aware of.

    Base 10: This is what you use every day, where you count 0 - 9, 10 - 19, 20 - 29 and so on.
    Base 16: Same as above, but now it's 0 - F, 10 - 1F, 20 - 2F and so on.

All the base really means is how many units are there before you have to move to the next column, i.e. in base 10, there's a sequence of 10 values (0,1,2,3,4,5,6,7,8,9) that can be expressed before moving onto the tens column.

Similarly with base 16, there's 16 values (0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F) before you have to move to the next column.

So for this puzzle, we can think of the condensed alphabet as our values, a,b,c,d,e,f,g,h,j,k,m,n,p,q,r,s,t,u,v,w,x,y,z (note: I've removed i, o and l as stated in the puzzle rules).

Or to put it another way:

    a = 0
    b = 1
    ...
    x = 20
    y = 21
    z = 22

There's 23 possible values, hence we can consider this to be a variant of base 23.  What this means is that instead of storing and processing the password as: a,b,y,z

I can store it as: 0,1,21,22

Which simplifies certain operations such as range checking, arithmetic processing and so on.  It also means I never have to worry about i, o or l.

So this now leads to the main algorithm, which is:

    Convert the starting Password to Base 23
    
    Increment the Password by 1

    WHILE either rules 1 or 3 are not met:
      Increment the Password by 1
    WEND

That's it really.  The major optimisation this is missing is instead of always incrementing by 1, determining what the first password that meets the currently failing rule is, and jump to that one instead.

i.e. Rule 1: Password must contain three consecutive values which are incrementing by 1 in ascending order.

So consider:

0,1,1,0

The next value that will meet this rule is: 0,1,2,0

But incrementing by 1 each time means we end up testing: [0,1,1,0], [0,1,1,1], [0,1,1,2] .. [0,1,1,22].

However, at least with my inputs, it's a subsecond solve on my aging hardware for both parts, so I'm happy to leave it as a more naive method for the sake of readability.  Typically I only go the route of implementing more effectient (but typically less intuitive) solutions to AoC when forced to.

One thing that you may have noticed is that Rule 1 includes the removed letters in the ascending rule, meaning hij are considered valid for rule 1 (even if rule 2 discards them).  Problem is by moving to base 23 and discarding these, hjk are now considered ascending, which is very clearly wrong.  The trivial, but non-ideal fix here is to convert the number from base 23 -> base 26 prior to considering whether they're consecutive or not.  Albeit for my original solution to this way back, I didn't notice this and it still got the correct answer, your mileage may vary.

And finally, how to actually do addition.  The answer, add and carry: https://en.wikipedia.org/wiki/Carry_(arithmetic)

In much the same way you'd solve 12 + 9, which is:

    25 +
     9
    ==
     4 (Carry the 1 to the tens column)
    ==
    34

In the same way:

    1,2,3,4  + 1 = 1,2,3,5
    1,2,3,22 + 1 = 1,2,4,0 (carry the 1 to the next column)

Algorithmically this is (for adding 1):

    FOR each column starting with the units column...
        column value = column value + 1
        IF column value < 23
            STOP
        ELSE
            column value = 0
        END
    NEXT

In other words, from the initial addition of a 1, it keeps carrying the 1 until the column no longer overflows the base of the number (base 23).

**Part 1**

Feeds the puzzle starting point into the system, the input supplied is deliberately one that has the next password being very close in terms of how many letters are actually changed.

**Part 2**

This feeds the solution from part 1 into the system, except now the next password is no longer as close, making it a scaleability problem.  Even so, the Python solution still solves this pretty quickly.

**Python vs C++ vs C**

**Python**

The zip function combined with array slicing does alot of the heavy lifting for checking pairs / sets of 3, but to me that's more something Python *has* to do for throughput as opposed to being some huge advantage Python has over C/C++, since the non-zip method is still trivial.

**C++**

This is essentially a straight port of the Python code.  There are a few things to be wary of, for example, the solvePassword function deliberately returns std::string by copy since kResult is destroyed once solvePassword completes.  Otherwise the only real irksome aspect is base C++/STL being a bit naff at initialising the contents at the point of declaration, which means there's a couple of points the code isn't technically const correct.  It's solveable with a bit of thought, but not worth the effort.

**C**

So here's where we start pulling back the curtain a bit.  Python abstracts away what letters really are, numbers.  In Python you'd need to use chr/ord to go betwen them, and to a certain extent, C++ can be used in such a way where it's non-obvious as to what strings/characters really are.  In C, there's absolutely no getting around it, a string is just an array, and a character is just a number, meaning the whole notion of converting from/to base 23 is really just an extra set of array lookups to enable arithmetic.

And if we embrace the fact we're only ever adding 1 to the password value at a time, what we're really doing is just working our way through a sequence, which we can pre-compute.

Instead of

    a = 0
    b = 1
    ...
    y = 21
    z = 22

and adding 1 each time...

What if we had an array where:

    [a] = b
    [b] = c
    ...
    [y] = z
    [z] = a

Now I just need to do:

    next = lookup[current]

Plus I can make h point to j (skipping i) and still get all that goodness, plus the relationship between sequences such as ijk are preserved.  In other words the C solution removes a whole level of repeated conversions.

Now this can absolutely still be done in Python (dictionaries) or C++ (maps/vectors/arrays), but the point is that the abstraction they provide would often take you down the arguably more legible route, whereas C kind of guides you to the jankier but faster route.  So as usual, the C solution to this ends up being faster, but also due to the removal of the abstraction layer, isn't that far off in terms of code size either.
