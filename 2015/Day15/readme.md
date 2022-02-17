# Day 15

https://adventofcode.com/2015/day/15

This introduces:
- Forced optimisations...

So here's where we need to actually start thinking about optimisations, the crux of the problem is that the total number of ingredients has to be 100, meaning at its most basic level, for the number of ingredients provided, we have to determine the score when any combination of those ingredients (keeping in mind it's permitted to not use an ingredient) gives a total ingredient count of 100.

So the easy way would be to calculate all possible combinations of each ingredient between 0 and 100, i.e.

    FOR Ingredient 1 Count = 0..100
      FOR Ingredient 2 Count = 0..100
        ...
          FOR Ingredient N Count = 0..100
            IF the Total Ingredient Count is 100
              Calculate the Recipe Score for using the Ingredient Counts
            END
          NEXT
        ...
      NEXT
    NEXT

Thing is we'd end spending more time *not* performing the Recipe Scores (just incrementing loop counts) compared to actually doing what we really want to do, being for all valid permutations, calculate the Recipe Score.

What we really want to do is more like:

    FOR Ingredient 1 Count = 0..100
      IF the Total Ingredient Count so far is 100
        Calculate the Recipe Score for using the Ingredient Counts
      ELSE IF the Total Ingredient Count so far is less than 100
        FOR Ingredient 2 Count = 0..100
          IF the Total Ingredient Count so far is 100
            Calculate the Recipe Score for using the Ingredient Counts
          ELSE IF the Total Ingredient Count so far is less than 100
            ...
            FOR Ingredient N Count = 0..100
              IF the Total Ingredient Count is 100
                Calculate the Recipe Score for using the Ingredient Counts
              END
            NEXT
            ...
          END
        NEXT
      END
    NEXT

Basically no point going to the next layer of the loop if we've already hit our 100 target.  And since we can see that as you traverse down the loop, that you're essentially doing the same thing just on the next ingredient, this means that this is probably best solved using something like Recursion.  To say this speeds this whole thing up is something of an understatement...

Note: There is a bit of logic missing, as you don't actually want to allow a total score to be evaluated if the current ingredient count is 0, since that would have already been evaluated on the previous pass, as this means calculating a set of ingredient quantities that have already been dealt with, however it bloats the basic algorithm pseuo-code, and isn't technically wrong, just slightly less efficient.

With only 4 ingredient in my input set, using the naive method vs the recursive method:

    Naive:     Only 176851 out of 103030100 times the IF statement was assessed resulted in the ingredient count being valid (0.17% of the time)
    Recursive: Only 176851 out of   4962329 times the IF statement was assessed resulted in the ingredient count being valid (3.56% of the time)

This results in about a 25X speed improvement on my system.  Definately worth it!

But wait, that's not all.  When I said that the each iteration of the loop is the same, that was a lie.  When we're at the innermost iteration of the loop, we can specialise the range.  Rather than:

    FOR Ingredient N Count = 0..100
      IF the Total Ingredient Count is 100
        Calculate the Recipe Score for using the Ingredient Counts
      END
    NEXT

By the time we reach here, we know the count can only be:

    Ingredient N Count = 100 - sum(Ingredient 1 Count .. Ingredient N-1 Count)

This brings the final number of iterations to:

    Recursive: Only 176851 out of    353701 times the IF statement was assessed resulted in the ingredient count being valid (50% of the time)

... and the total speed up to around 60X!

If you want to read up on this sort of problem in a more abstract sense...  https://en.wikipedia.org/wiki/Subset_sum_problem

Otherwise, the rest of the puzzle is trivial, sum the properties of each ingredient (taking into account the quantities we've calculated), handle any negative values, and multiply them all together.

**Part 1**

Uses the recursive algorithm and keeps the best cookie score found.

**Part 2**

Performed in the same algorithm as Part 1 (since the slow part is generating the permutations), but only evaluated a given cookie score if the calorie count was exactly 500.

**Python vs C++ vs C**

**Python**

A small saving can be made by not resizing the ingredientList list in the recursive function (instead just keeping tabs on our depth), but in the scheme of things, this only nets a very small saving compared to overall executing time (around 5%).

**C++**

No real difference from the Python code, albeit with a nice speed boost as a natural advantage of the nature of the language.

**C**

As ever, same code as C++, only faster, mainly due to the lack of abstraction in memory management / array handling... but at the expense of encapsulation/simplicity/readibility.
