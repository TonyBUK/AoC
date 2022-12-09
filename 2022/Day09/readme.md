# Day 9: Rope Bridge

https://adventofcode.com/2022/day/9

Kind of a interesting issue for this one.  The puzzle itself is so *close* to implementing Snake with respect to tail movement, but uses slightly tweaked rules.

**Part 1**

Part 1 is actually deceptively trivial.  You'll either get lucky and solve it in a way that's viable for Part 2, or you'll get unlucky, think of a cool short-cut, and your solution here won't work.

Firstly the common part... the input is actually nice and simple.  A series of lines, divided by a space, with a direction, and a move count.  This can be trivialised using vectors.

The directions will be:

    U : UP    : [ 0,  1]
    R : RIGHT : [ 1,  0]
    D : DOWN  : [ 0, -1]
    L : LEFT  : [-1,  0]

Now you may notice compared to other puzzles, I've used [X, Y] instead of [Y, X] and UP is +ve, whereas I normally pick negative.  The main reason is that we're not working with a grid for this puzzle.  The sign conventions, or event the axis order is unimportant, all that is required is:

1. U/D only move 1 axis, and it can't be the same as L/R.
2. U/D have the same magnitude, but different sign.
3. L/R only move 1 axis, and it can't be the same as U/D.
4. L/R have the same magnitude, but different sign.

If you'd prefer to stick to [Y,X] with Up being -ve, feel free.  It absolutely has no impact on the puzzle itself.

Once you have a move list, the basic algorithm is:

    Set the Head to [0, 0]
    Set the Tail to [0, 0]

    FOR each Move

      Determine which Vector to use for the current Move

      FOR Number of Times to Run the Move

        Move the Head by the Vector

        ...

      NEXT

    NEXT

All that remains is the tail.  So there's two aspect, the common, and the important.

Common:

So the rule is that as long as the tail is either on the same position as the head, or on one of the 9 adjacent squares from the head, it does not need to move.

This is basically:

    IF ABS(Head X Axis - Tail X Axis) <= 1 AND
       ABS(Head Y Axis - Tail Y Axis) <= 1

      No Need to Move the Tail

    END

As we're adjacent as long as the X/Y positions of the Tail are +/- from the X/Y positions of the Head, the ABS check just simplifies this check (Otherwise we'd need to test >= -1 and <= 1).

Important:

Here's the real meat of the task.  What happens when the tail is not adjacent.

The rules are:

One Axis not Touching:

    .....    .....    .....
    .TH.. -> .T.H. -> ..TH.
    .....    .....    .....

    ...    ...    ...
    .T.    .T.    ...
    .H. -> ... -> .T.
    ...    .H.    .H.
    ...    ...    ...

Two Axis not Touching:

    .....    .....    .....
    .....    ..H..    ..H..
    ..H.. -> ..... -> ..T..
    .T...    .T...    .....
    .....    .....    .....

    .....    .....    .....
    .....    .....    .....
    ..H.. -> ...H. -> ..TH.
    .T...    .T...    .....
    .....    .....    .....

Now there is a horrible solution that I may or may not have come up with... the horrible one is that in all of these scenarios, the tail actually ended up at the Heads *old* position.  That works, but will break in Part 2 for reasons I'll explain later.

Now the implication is that we need to do something different whether it's only one or two axis that are different between the Head/Tail.  In reality it's:

    Difference = (Head - Tail)
    Vector     = Difference Limited to +/-1 in each Axis

So let's go through the examples:

    .....    .....    .....
    .TH.. -> .T.H. -> ..TH.
    .....    .....    .....

    Head       = [3, 1]
    Tail       = [1, 1]
    Difference = [2, 0]
    Vector     = [1, 0] (Difference Limited to +/-1)

    New Tail   = Tail + Vector = [2, 1]

    ...    ...    ...
    .T.    .T.    ...
    .H. -> ... -> .T.
    ...    .H.    .H.
    ...    ...    ...

    Head       = [1, 3]
    Tail       = [1, 1]
    Difference = [0, 2]
    Vector     = [0, 1] (Difference Limited to +/-1)

    New Tail   = Tail + Vector = [1, 2]

    .....    .....    .....
    .....    ..H..    ..H..
    ..H.. -> ..... -> ..T..
    .T...    .T...    .....
    .....    .....    .....

    Head       = [2, 1]
    Tail       = [1, 3]
    Difference = [1, -2]
    Vector     = [1, -1] (Difference Limited to +/-1)

    New Tail   = Tail + Vector = [2, 2]

    .....    .....    .....
    .....    .....    .....
    ..H.. -> ...H. -> ..TH.
    .T...    .T...    .....
    .....    .....    .....

    Head       = [3, 2]
    Tail       = [1, 3]
    Difference = [2, -1]
    Vector     = [1, -1] (Difference Limited to +/-1)

    New Tail   = Tail + Vector = [2, 2]

All good!

So bringing it all together to finally get all the unique positions the tail reached...

    Set the Head to [0, 0]
    Set the Tail to [0, 0]

    Reset the Visited Positions

    FOR each Move

      Determine which Vector to use for the current Move

      FOR Number of Times to Run the Move

        Move the Head by the Vector

        Calculate the Difference between the Head and the Tail

        Set the Tail Vector to the Difference, Limited to +/-

        Add the Tail Vector to the Tail

        Add the Tail to the Visited Positions if it's not already there

      NEXT

    NEXT

    Answer = Length of Visited Positions

**Part 2**

By doing Part 1 properly this is actually just an extention of that.  Rather than having 1 knot, we have 9 following the Head, or better yet, we have 10 knots total.  So what we have is 9 Head/Tail problems.

So we just need to repeat Part 1 but with:

    Head = Knot 0
    Tail = Knot 1

    Head = Knot 1
    Tail = Knot 2

    Head = Knot 2
    Tail = Knot 3

    Head = Knot 3
    Tail = Knot 4

    Head = Knot 4
    Tail = Knot 5

    Head = Knot 5
    Tail = Knot 6

    Head = Knot 6
    Tail = Knot 7

    Head = Knot 7
    Tail = Knot 8

    Head = Knot 8
    Tail = Knot 9

Where the first Head is moved by the movement command, but all subsequent heads are moved based on the Part 1 Tail Vector algorithm.

The solution is:

    Set the All the Knots to [0, 0]

    Reset the Visited Positions

    FOR each Move

      Determine which Vector to use for the current Move

      FOR Number of Times to Run the Move

        Move the First Knot by the Vector

        FOR each the Second Knot Onwards

          Calculate the Difference between the Current Knot and the Previous Knot

          Set the Tail Vector to the Difference, Limited to +/-

          Add the Tail Vector to the Current Knot

        NEXT

        Add the Last Knot to the Visited Positions if it's not already there

      NEXT

    NEXT

    Answer = Length of Visited Positions

**Python vs C++ vs C**

**Python**

Uses sets for the Visited Positions, which are forces only unique values to be present.

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

So another C bodge.  Now I could use hashing or something along those lines, however, instead, let's pre-calculate the bounding rectangle that can contain where the Snake could move.  To do this, we just need to worry about the Head, as Tails can only move *towards* the Head, they can't move away, meaning they can't overshoot.

Once we know this, assuming the range of positions isn't absurd, we could just store a RAM buffer of possible locations, and flag each one we've reached.

But of course C arrays are 0 based, and we'll probably encounter negative locations.  No problem...

Let's look at what my puzzle returns.

    X Axis = -47 .. 144
    Y Axis = -65 .. 196
    Width  = 144 - -47 = 191
    Height = 196 - -65 = 261

Well we know the smallest value.  If we negated that add added it to the all positions we'd get:

    X Axis = (-47+47) .. (144+47)
    Y Axis = (-65_65) .. (196+65)

But why add it, we set the starting position of all knots to [0, 0].  Why not set it to these offset values, as all we care about is unique positions, not what those positions really were.  Once we do this, we have a nice trivial 0 based index check.

This way the puzzle becomes:

    Set the All the Knots to [-MINX, -MINY]

    Reset the Visited Positions

    Set the Unique Visited Position Count to 0

    FOR each Move

      Determine which Vector to use for the current Move

      FOR Number of Times to Run the Move

        Move the First Knot by the Vector

        FOR each the Second Knot Onwards

          Calculate the Difference between the Current Knot and the Previous Knot

          Set the Tail Vector to the Difference, Limited to +/-

        NEXT

        IF Visited Positions for the Last Knot is False

          Set Visited Positions Knot to True

          Increment the Unique Visited Position Count

          Add the Tail Vector to the Current Knot

        END

      NEXT

    NEXT

In-fact we can go further.  Part 2 actually subsumes Part 1.  Part 1 is just checking the second knot, which the C version implements.