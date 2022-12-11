# Day 11: Monkey in the Middle

https://adventofcode.com/2022/day/11

This one initially we seem like a parsing / processing problem, whereby this has elements of the interpretter that was yesterdays problem.  This is simplified somewhat by making each of the monkeys turns happen in order (i.e. not all occuring simultaneously).  The crux of this problem is a scalability issue, but we'll get to that.

**Part 1**

So the first part of the puzzle is just parsing the data.

    Monkey 0:
      Starting items: 79, 98
      Operation: new = old * 19
      Test: divisible by 23
        If true: throw to monkey 2
        If false: throw to monkey 3

The five most trivial items are.

    Monkey 0:
      Starting items: 79, 98
      Test: divisible by 23
        If true: throw to monkey 2
        If false: throw to monkey 3

Whilst if you want to harden your code you absolutely can, we can take some shortcuts.

1. Monkeys are provided in order, i.e. Monkey 1 won't appear *before* Monkey 0.
2. Data for the Monkeys is consistently ordered and laid out.
3. The Test will always be a division check.

Because of this, we can:

1. Skip the Monkey Number
2. Store the Items in an Array
3. Store the Division Value
4. Store the True Target Monkey
5. Store the False Target Monkey

All that remains is the operation.  For this we can make a few assumptions as well:

1. The operation will only ever be a single operation, i.e. X ? Y
2. The operation will only ever be an addition or multiplication
3. The operation *may* contain a literal

It's also a safe bet that the operation will contain a reference to the current value, but that shouldn't make too much of a difference.  There's a number of ways of storing it, for the example I'll suggest 3 strings, but anything you can do to pre-convert the literals / operation will speed up your solution.

So with all the data buffered, let's get into what we need to solve the puzzle.  The puzzle itself isn't concerned with the values of the items (this is a strong hint as to where Part 2 is going), but is instead concerned with how many items each Monkey processed each cycle.

    Reset the Activity for All Monkeys

    FOR 20 Rounds...

      FOR each Monkey

        Increment the Activity for the Current Monkey by the number of Items the Monkey has to process

        FOR each Item the Monkey is holding

          Perform the Current Operation on the Item the Current Monkey is Holding and store the New Value

          Divide the New Value by 3 and trunacate (round down)

          IF the New Value is exactly divisible by the Test Value

            Add the New Value to the True Monkey's list of Items

          ELSE

            Add the New Value to the False Monkey's list of Items

        NEXT

        Remove all Items from the Current Monkey

      NEXT

    NEXT

    Sort the Monkey Activity List and multiply the two largest values together

Checking divisibility is typically done with the modulo operator, i.e.

    IF (X % Y) == 0

      X is exactly divisible by Y

    END

So that just leaves the Current Operation itself.  This is actually a relatively simple substitution problem.  Imagine the current value is 17, and the operation is:

    old = old + 35

Now we can assume "old = " is a given, and that we're always updating the current value.  That just leaves:

    old + 35

If we split this into an array:

    [0] : "old"
    [1] : "+"
    [2] : "35"

All we have to do is check items 0 and 2, any that say "old" replace with the current value.

i.e.

    [0] : "17"
    [1] : "+"
    [2] : "35"

And with that we can just use the following pseudo code:

    IF Operation[1] == "+"

      New Value = Operation[0] + Operation[2]

    ELSE

      New Value = Operation[0] * Operation[2]

    END

There's other ways of doing this but I'll leave that as an exercise for you!

**Part 2**

This removes the division by 3, then asks us to run through 10,000 times.

i.e.

    Reset the Activity for All Monkeys

    FOR 10,000 Rounds...

      FOR each Monkey

        Increment the Activity for the Current Monkey by the number of Items the Monkey has to process

        FOR each Item the Monkey is holding

          Perform the Current Operation on the Item the Current Monkey is Holding and store the New Value

          IF the New Value is exactly divisible by the Test Value

            Add the New Value to the True Monkey's list of Items

          ELSE

            Add the New Value to the False Monkey's list of Items

        NEXT

        Remove all Items from the Current Monkey

      NEXT

    NEXT

    Sort the Monkey Activity List and multiply the two largest values together

Now technically that is the right answer.  The problem is that division by 3 is the only thing that prevented the numbers from getting astronomically huge.

The solution to this is *NOT* trying to cram this into a larger data type.  For example, sure a double stores much larger values, but it doesn't store them *accurately*. The solution is to prevent the numbers from getting large *without* impacting the tests for passing the items from Monkey to Monkey.

As mentioned in Part 1, the foreshadowing was that it only cared about how many comparisons are performed by each Monkey, not the values of the Items themselves.

So let's look at a single division test, let's make it a divide by 11 check.

Now let's make the operation to Add 13 each time, i.e. old = old + 13.

And finally, make the initial value 1.

Let's run that through a few cycles:

    [ 0] :   1 + 13 =  14 (not divisible by 11)
    [ 1] :  14 + 13 =  27 (not divisible by 11)
    [ 2] :  27 + 13 =  40 (not divisible by 11)
    [ 3] :  40 + 13 =  53 (not divisible by 11)
    [ 4] :  53 + 13 =  66 (divisible by 11)
    [ 5] :  66 + 13 =  79 (not divisible by 11)
    [ 7] :  79 + 13 =  92 (not divisible by 11)
    [ 8] :  92 + 13 = 105 (not divisible by 11)
    [ 9] : 105 + 13 = 118 (not divisible by 11)
    [10] : 118 + 13 = 131 (not divisible by 11)
    [11] : 131 + 13 = 144 (not divisible by 11)
    [12] : 144 + 13 = 157 (not divisible by 11)
    [13] : 157 + 13 = 170 (not divisible by 11)
    [14] : 170 + 13 = 183 (not divisible by 11)
    [15] : 183 + 13 = 196 (not divisible by 11)
    [16] : 196 + 13 = 209 (divisible by 11)

Now let's say we don't want the value going over 100, how could we adjust this in such a way that it will still result in the right answer?  Well the answer is in the way we test for divisibility.

If a value is divisible by 11, it means the modulo of 11 is 0.  So why not only store the modulo.  This makes the above:

    [ 0] : ( 1 + 13) % 11 =  3 (not divisible by 11)
    [ 1] : ( 3 + 13) % 11 =  5 (not divisible by 11)
    [ 2] : ( 5 + 13) % 11 =  7 (not divisible by 11)
    [ 3] : ( 7 + 13) % 11 =  9 (not divisible by 11)
    [ 4] : ( 9 + 13) % 11 =  0 (divisible by 11)
    [ 5] : ( 0 + 13) % 11 =  2 (not divisible by 11)
    [ 7] : ( 2 + 13) % 11 =  4 (not divisible by 11)
    [ 8] : ( 4 + 13) % 11 =  6 (not divisible by 11)
    [ 9] : ( 6 + 13) % 11 =  8 (not divisible by 11)
    [10] : ( 8 + 13) % 11 = 10 (not divisible by 11)
    [11] : (10 + 13) % 11 =  1 (not divisible by 11)
    [12] : ( 1 + 13) % 11 =  3 (not divisible by 11)
    [13] : ( 3 + 13) % 11 =  5 (not divisible by 11)
    [14] : ( 5 + 13) % 11 =  7 (not divisible by 11)
    [15] : ( 7 + 13) % 11 =  9 (not divisible by 11)
    [16] : ( 9 + 13) % 11 =  0 (divisible by 11)

But now imagine we have two Monkeys to process, and let's assume the divisor test for the second Monkey is 16, and that it multiplies by 2 each time.  Let's assume whilst the value isn't divisible, the Current Monkey will keep the item, otherwise it will throw it to the other monkey

    [ 0] :   1 + 13 =  14 (not divisible by 11, stays with Monkey 1)
    [ 1] :  14 + 13 =  27 (not divisible by 11, stays with Monkey 1)
    [ 2] :  27 + 13 =  40 (not divisible by 11, stays with Monkey 1)
    [ 3] :  40 + 13 =  53 (not divisible by 11, stays with Monkey 1)
    [ 4] :  53 + 13 =  66 (divisible by 11, passed to Monkey 2)
    [ 5] :  66 *  2 = 132 (not divisible by 16, stays with Monkey 2)
    [ 6] : 132 *  2 = 264 (not divisible by 16, stays with Monkey 2)
    [ 7] : 264 *  2 = 528 (divisible by 16, passed to Monkey 1)

Now as we can see the divisor changes, so a modulo of 11 will undoubtely break things.  But let's consider multiples.  Let's go back to just dealing with 1 monkey, but this time use 22 as our modulo (2 * 11)

    [ 0] : ( 1 + 13) % (2 * 11) = 14 (not divisible by 11)
    [ 1] : (14 + 13) % (2 * 11) =  5 (not divisible by 11)
    [ 2] : ( 5 + 13) % (2 * 11) = 18 (not divisible by 11)
    [ 3] : (18 + 13) % (2 * 11) =  9 (not divisible by 11)
    [ 4] : ( 9 + 13) % (2 * 11) =  0 (divisible by 11)
    [ 5] : ( 0 + 13) % (2 * 11) = 13 (not divisible by 11)
    [ 7] : (13 + 13) % (2 * 11) =  4 (not divisible by 11)
    [ 8] : ( 4 + 13) % (2 * 11) = 17 (not divisible by 11)
    [ 9] : (17 + 13) % (2 * 11) =  8 (not divisible by 11)
    [10] : ( 8 + 13) % (2 * 11) = 21 (not divisible by 11)
    [11] : (21 + 13) % (2 * 11) = 12 (not divisible by 11)
    [12] : (12 + 13) % (2 * 11) =  3 (not divisible by 11)
    [13] : ( 3 + 13) % (2 * 11) = 16 (not divisible by 11)
    [14] : (16 + 13) % (2 * 11) =  7 (not divisible by 11)
    [15] : ( 7 + 13) % (2 * 11) = 20 (not divisible by 11)
    [16] : (20 + 13) % (2 * 11) = 11 (divisible by 11)

Notice the pattern of divisible by 11 checks remains consistent.  In-fact *any* non-zero multiple of 11 would work.  So now we just need to find a Modulo that works for Monkey 1 (divisible by 11) and Monkey 2 (divisible by 16).  And since multiples of 11 work for Monkey 1, and multiples of 16 work for Monkey 2, we just need to find out where these converge, and the answer is the LCM (Least/Lowest Common Multiple), being 11 * 16 or 176.

Let's go back to the 2 Monkey test to see how it works using 176 as a modulo.

    [ 0] : (  1 + 13) % (11 * 16) =  14 (not divisible by 11, stays with Monkey 1)
    [ 1] : ( 14 + 13) % (11 * 16) =  27 (not divisible by 11, stays with Monkey 1)
    [ 2] : ( 27 + 13) % (11 * 16) =  40 (not divisible by 11, stays with Monkey 1)
    [ 3] : ( 40 + 13) % (11 * 16) =  53 (not divisible by 11, stays with Monkey 1)
    [ 4] : ( 53 + 13) % (11 * 16) =  66 (divisible by 11, passed to Monkey 2)
    [ 5] : ( 66 *  2) % (11 * 16) = 132 (not divisible by 16, stays with Monkey 2)
    [ 6] : (132 *  2) % (11 * 16) =  88 (not divisible by 16, stays with Monkey 2)
    [ 7] : ( 88 *  2) % (11 * 16) =   0 (divisible by 16, passed to Monkey 1)

We get exactly the same pattern.  So to calculate the LCM for all Monkeys, it's as follows:

    LCM = 1
    FOR each Monkey
      LCM = LCM * Divisor Test for Current Monkey
    END

Meaning to bring everything together...

    Calculate the LCM for all Monkeys
    
    Reset the Activity for All Monkeys

    FOR 10,000 Rounds...

      FOR each Monkey

        Increment the Activity for the Current Monkey by the number of Items the Monkey has to process

        FOR each Item the Monkey is holding

          Perform the Current Operation on the Item the Current Monkey is Holding and store the New Value

          Apply the LCM Modulo to the New Value

          IF the New Value is exactly divisible by the Test Value

            Add the New Value to the True Monkey's list of Items

          ELSE

            Add the New Value to the False Monkey's list of Items

        NEXT

        Remove all Items from the Current Monkey

      NEXT

    NEXT

    Sort the Monkey Activity List and multiply the two largest values together

All done!  Phew.

**Python vs C++ vs C**

**Python**

Follows the solution as-is.

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

Mostly the same, this does try and optimise the operations by abusing the fact the operation applied never uses 0, so it uses 0 as a quick way of meaning "the current value of the item".  Overall C really benefits from tighter control on arrays etc. being a 100x magnitude improvement.