# Day 14

https://adventofcode.com/2015/day/15

This introduces:
- Optimisations...

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

Basically no point going to the next layer of the loop if we've already hit our 100 target.  And since we can see time you go down the loop, that you're essentially doing the same thing, this means that this is probably best solved using something like Recursion.  To say this speeds this whole thing up is something of an understatement...

With only 4 ingredient in my input set, using the naive method vs the recursive method:

- Only 176851 out of 103030100 times the IF statement was assessed resulted in the ingredient count being valid (0.17% of the time)
- Only 176851 out of   4962329 times the IF statement was assessed resulted in the ingredient count being valid (3.56% of the time)

This results in about a 25X speed improvement on my system.  Definately worth it!

If you want to read up on this sort of problem in a more abstract sense...  https://en.wikipedia.org/wiki/Subset_sum_problem

Otherwise, the rest of the puzzle is trivial, sum the properties of each ingredient (taking into account the quantities we've calculated), handle any negative values, and multiply them all together.

**Part 1**

Uses the recursive algorithm and keeps the best cookie score found.

**Part 2**

Performed in the same pass as Part 1, but now only bothers evaluating a given cookie score if the calorie count was exactly 500.

**Python vs C++ vs C**

**Python**

A small saving can be made by not resizing the ingredientList list in the recursive function (instead just keeping tabs on our depth), but in the scheme of things, this only nets a very small saving compared to overall executing time (around 5%).

**C++**

**C**

