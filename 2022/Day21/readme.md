# Day 21: Monkey Math

https://adventofcode.com/2022/day/21

This one reminds me of an issue I came up with when writing a compiler, being that sometimes at the point you try to parse an expression, all the parts of that expression might not be known until later.  Processing may seem to be the hard part, but Part 2 will turn that on its head!

**Part 1**

For Part 1 there's a few ways of solving this.  The most obvious one is DFS.  Where you'll do something like:

    Recursive Function:

      IF node is a literal

        RETURN literal

      ELSE

        IF Operation is "+"

          RETURN Recurse(Left) + Recurse(Right)

        ELSE IF Operation is "-"

          RETURN Recurse(Left) - Recurse(Right)

        ELSE IF Operation is "/"

          RETURN Recurse(Left) / Recurse(Right)

        ELSE IF Operation is "*"

          RETURN Recurse(Left) * Recurse(Right)

        END

      END

    END

And that's really perfectly valid.  I personally went with BFS (with no queue priority), and alot of that was to do with caching.  The basic BFS algorithm goes something like:

    1. If a monkey is a literal, put it in the list of solved monkeys.
    2. Otherwise put it in the list of unsolved monkeys.

    WHILE there are unsolved monkeys

      WHILE there are unsolved monkeys

        Pop the current monkey from the list of unsolved monkeys

        IF either the Left/Right of the expression is in the list of the unsolved monkeys

          Add the unsolved monkey to the "new" unsolved monkeys

          Skip this monkey

        END

        IF Operation is "+"

          Set this Monkey to Left + Right

        ELSE IF Operation is "-"

          Set this Monkey to Left - Right

        ELSE IF Operation is "/"

          Set this Monkey to Left / Right

        ELSE IF Operation is "*"

          Set this Monkey to Left * Right

        END

        Add this Monkey to the list of solved monkeys

      END

      Set the unsolved monkeys to the "new" unsolved monkeys

      Clear the "new" unsolved monkeys

    END

    RETURN the "root" monkeys value

Both solve Part 1, but the BFS version will play nicer into Part 2...

**Part 2**

So there's a couple of ways here.  Firstly, forget brute force.  My answer was 3678125408017, good luck brute forcing that sort of magnitude.

However, from the few inputs I've seen, the following seems to be true.

1. Changing the value of the humn monkey will only impact the value of one side of the root monkeys equality statement, the other side will effectively be a constant.

2. The closer the humn value is to being the right answer, the closer the "other" side of the root monkeys equality statement is to being correct.

Method 1: Binary Search

So one method could be a binary search.  So rather than having the root monkey test for equality, you could have it subtract the constant value from the humn derived value, or you could have it divide the humn derived value by the constant value.

What you're looking for is a way of determining how close a value is to being correct, and whether you're improving.

For this example, I'll use division, whereby as long as we force the humn derived value to being the numerator, we know we have the right answer based to how close to 1.0 is returned.

So a basic algorithm for a Search Range would be:

    Set the Root Monkey to return unknown / known
    
    Set Min to 1

    Set the humn monkey to Min

    Perform the Monkey Math

    Determine which side of 1.0 this lies.

    Set Max to Min * 10
    
    WHILE True

      Set the humn monkey to Max

      Perform the Monkey Math

      Determine which side of 1.0 this lies.

      IF it's the other side of 1.0 compared to Min
        Exit the Loop
      END

      Set Min to Max

      Set Max to Max * 10

    WEND

Now we have a range where we know the answer is between Min and Max, so let's start cracking down

    WHILE (Max - Min) > 1

      Mid = ((Max - Min) / 2) + Min

      Set the humn monkey to Mid

      Perform the Monkey Math

      Determine which side of 1.0 this lies.

      IF it's the same side as Min
        Set Min to Mid
      ELSE
        Set Max to Mid
      END

    WEND

Eventually Max/Min will be 1 away from each other, and one of these is the right answer.  You could test for being exactly 1.0, personally I don't like testing whether floats are equal, tends to lead to weirdness, so instead I personally would change the root monkey back to an equality statement, and see which value returns True.

Method 2: Algorithmically

So remember when I said that the BFS approach could help us here...

Let's tweak the algorithm at bit.

    1. If a monkey is a literal, put it in the list of solved monkeys (excluding humn).
    2. Otherwise put it in the list of unsolved monkeys (excluding humn).

    WHILE there are unsolved monkeys

      WHILE there are unsolved monkeys

        Pop the current monkey from the list of unsolved monkeys

        IF either the Left/Right of the expression is in the list of the unsolved monkeys

          Add the unsolved monkey to the "new" unsolved monkeys

          Skip this monkey

        END

        IF Operation is "+"

          Set this Monkey to Left + Right

        ELSE IF Operation is "-"

          Set this Monkey to Left - Right

        ELSE IF Operation is "/"

          Set this Monkey to Left / Right

        ELSE IF Operation is "*"

          Set this Monkey to Left * Right

        END

        Add this Monkey to the list of solved monkeys

      END

      Set the unsolved monkeys to the "new" unsolved monkeys

      IF unsolved monkeys hasn't changed...
        RETURN the Unsolved Monkeys
      END

      Clear the "new" unsolved monkeys

    END

So now what this is doing is solving all the Monkeys that can be solved, and the only remaining "unsolved" monkeys are all of those with a dependency on the humn monkey.  Pretty neat right!

From this, we basically reverse all the operations.  From the root monkey, we should have two reference monkeys, a solved and an unsolved.  And the solved monkey is the starting point for deriving our answer.

So the method will now be DFS, but walking back the unknown monkey and "reversing" all of the operations.

So let's consider this on the test inputs.

We know the answer is 150, and the root monkey is currently wanting to shout:

    pppw = sjmn

    - pppw is not solved.
    - sjmn is solved, and equal to 150.

So we want to walk back pppw until we get to humn...

    pppw: cczh / lfqf

    pppw = 150 (as it has to equal sjmn)
    
    150 = cczh / lfqf

Now as we've pre-solved this, we also know the value of lfqf, which is 4.  We know this as we've pre-solved everything we can pre-solve, and this had no dependencies on humn.

Therefore:

    150 = cczh / 4

So let's re-arrange this in terms of cczh

    cczh = 150 * 4 = 600

Now let's go a layer deeper...

    cczh:  sllz + lgvd
    600  = sllz + lgvd
    600  = 4    + lgvd
    lgvd = 600  - 4 = 596

And again...

    lgvd:  ljgn * ptdq
    596  = ljgn * ptdq
    596  = 2    * ptdq
    ptdq = 596 / 2 = 298

And one last time...

    ptdq:  humn - dvpt
    298  = humn - dvpt
    298  = humn - 3
    humn = 298  + 3 = 301

Therefore to get an answer of 150, humn needs to be set to 301.

So all we need to do now is figure out how to reverse all the possible expressions generically.

    Normal:   Value           = Left (Known) + Right (Unknown)
    Reversed: Right (Unknown) = Value - Left (Known)

    Normal:   Value           = Left (Unknown) + Right (Known)
    Reversed: Left (Unknown)  = Value - Right (Known)

    Normal:   Value           = Left (Known) - Right (Unknown)
    Reversed: Right (Unknown) = Left (Known) - Value

    Normal:   Value           = Left (Unknown) - Right (Known)
    Reversed: Left (Unknown)  = Value + Right (Known)

    Normal:   Value           = Left (Known) * Right (Unknown)
    Reversed: Right (Unknown) = Value / Left (Known)

    Normal:   Value           = Left (Unknown) * Right (Known)
    Reversed: Left (Unknown)  = Value / Right (Known)

    Normal:   Value           = Left (Known) / Right (Unknown)
    Reversed: Right (Unknown) = Left (Known) / Value

    Normal:   Value           = Left (Unknown) / Right (Known)
    Reversed: Left (Unknown)  = Value * Right (Known)

And the recursive function is:

      Recursive Function:

        IF the Monkey is the humn Monkey
          Return the Current Value
        END

        RETURN Recurse with the Monkey changed to the Unknown Monkey, and the Current Value set to the reverse of the Current Operation

      END

The first Current Value will be the known Monkey's value of the root monkey.

**Python vs C++ vs C**

**Python**

This implements the Algorithmic Solution detailed above.  My only annoyance is multiple return types in the Monkey Math function.  I'm sure I could clean it up somewhat, but I'm loathed to as I know it'd be a non-issue in C/C++.  It's basically where shallow copying didn't update the instance of the set outside the function call to monkeyMath, whereas in C/C++, I can be very explicit about this sort of thing if needed.

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

TBD - Playing catch-up with multiple solutions now as the puzzles get harder!