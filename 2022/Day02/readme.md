# Day 2: Rock Paper Scissors

https://adventofcode.com/2022/day/2

Firstly for anyone who doesn't know, this is a simple playground game where you and an opponent make one of three hand shapes:

- Rock (Closed Fist)
- Paper (Open Palm)
- Scissors (Two fingers making a scissors shape)

The rules are simple:

- If both players pick the same one, it's a draw otherwise...
- Rock beats Scissors
- Paper beats Rock
- Scissors beats Paper

Or to put it another way, there's 9 possible outcomes of any given game:

        Player 1 : Player 2 : Winner
        =========:==========:=========
        Rock     : Rock     : Draw
        Rock     : Paper    : Player 2
        Rock     : Scissors : Player 1
        Paper    : Rock     : Player 1
        Paper    : Paper    : Draw
        Paper    : Scissors : Player 2
        Scissors : Rock     : Player 2
        Scissors : Paper    : Player 1
        Scissors : Scissors : Draw

With the Elf version of the game, each player is given a score after a game is played.

- If a player wins, they get 6 points.
- If a player loses, they get 3 points.
- Otherwise, no points.

In addition, they also get extra points based on what they picked regardless of the game outcome.
- Picking Rock is worth 1 Point
- Picking Paper is worth 2 Points
- Picking Scissors is worth 3 Points

The puzzle provides two values.  The first is the move the Elf will play, A being Rock, B being Paper, and C being Scissors, then we have the second column, which will have either X, Y or Z.  What this means depends on which part of the puzzle you're on...

**Part 1**

For Part 1, X/Y/Z are the moves you have to make.  X being Rock, Y being Paper, Z being Scissors.

So the challenge here is based on knowing what move both players made, and which move you specifically made, calculating the score.

There's really three ways of doing this

*Algorithmically*

With this method, the pseudo code is something like:

        Total Score = 0
        
        FOR Each Round

            Score = 0

            Translate the Elfs Letter A/B/C to Rock/Paper/Scissors

            Translate our Letter X/Y/Z to Rock/Paper/Scissors

            If we Won Add 6 to the Score
            If we Drew Add 3 to the Score

            Add the value associated with our move to the Score

            Total Score = Total Score + Score

        NEXT

There's quite a few ways to calculate whether we won or not but ultimately from the previous truth table, we only need three states:

        Player 1 : Player 2 : Winner
        =========:==========:=========
        Rock     : Paper    : Player 2
        Paper    : Scissors : Player 2
        Scissors : Rock     : Player 2

And hopefully drawing is obvious, it's when we both make the same move, i.e.

        Player 1 : Player 2 : Winner
        =========:==========:=========
        Rock     : Rock     : Draw
        Paper    : Paper    : Draw
        Scissors : Scissors : Draw

So we could do something like:

        IF (Elfs Move = Rock     AND Our Move = Paper) OR
           (Elfs Move = Paper    AND Our Move = Scissors) OR
           (Elfs Move = Scissors AND Our Move = Rock)
            We Won
        ELSE IF Elfs Move = Our Move
            We Drew
        ELSE
            We Lost
        END


*Lookup*

So algorithmic is fine and all, but there's only 9 outcomes, couldn't we just pre-calculate them?  Why yes we can:

        Player 1     : Player 2     : Winner   : Score
        =============:==============:==========:==========
        Rock (A)     : Rock (X)     : Draw     : 3 + 1 = 4
        Rock (A)     : Paper (Y)    : Player 2 : 6 + 2 = 8
        Rock (A)     : Scissors (Z) : Player 1 : 0 + 3 = 3
        Paper (B)    : Rock (X)     : Player 1 : 0 + 1 = 1
        Paper (B)    : Paper (Y)    : Draw     : 3 + 2 = 5
        Paper (B)    : Scissors (Z) : Player 2 : 6 + 3 = 9
        Scissors (C) : Rock (X)     : Player 2 : 6 + 1 = 7
        Scissors (C) : Paper (Y)    : Player 1 : 0 + 2 = 2
        Scissors (C) : Scissors (Z) : Draw     : 3 + 3 = 6

So we could instead have:

        Outcomes["A", "X"] = 4
        Outcomes["A", "Y"] = 8
        Outcomes["A", "Z"] = 3
        Outcomes["B", "X"] = 1   
        Outcomes["B", "Y"] = 5
        Outcomes["B", "Z"] = 9
        Outcomes["C", "X"] = 7
        Outcomes["C", "Y"] = 2
        Outcomes["C", "Z"] = 6

        Total Score = 0
        
        FOR Each Round

            Score       = Outcomes[Elfs Letter, Our Letter]
            Total Score = Total Score + Score

        NEXT

*Mathematically*

This one is hinted at when they give Rock/Paper/Scissors values (1/2/3 respectively).  Imagine this as a circular list:

        Scissors BEATS Paper BEATS Rock BEATS Scissors BEATS Paper BEATS Rock
        3        BEATS 2     BEATS 1    BEATS 3        BEATS 2     BEATS 1

Notice how aside from Rock BEATS SCISSORS, you can pretty generally describe this as:

        Winning = (Our Move - Elf Move) == 1

Now consider Losing Moves:

        Rock LOSES TO Paper LOSES TO Scissors LOSES TO Rock LOSES TO Paper LOSES TO Scissors
        1    LOSES TO 2     LOSES TO 3        LOSES TO 1    LOSES TO 2     LOSES TO 3

Notice how aside from Scissors LOSES TO Rock, you could pretty generally describe this as:

        Losing = (Our Move - Elf Move) == -1

And of course the draw state would be:

        Draw = (Our Move - Elf Move) == 0

So the two exceptions are:

        - Rock (1) BEATS Scissors (3)    = (Our Move - Elf Move) = -2
        - Scissors (3) LOSES TO Rock (1) = (Our Move - Elf Move) = 2

Consider:

        Game State = Our Move - Elf Move
        IF Game State < -1
            Game State = Game State + 3
        ELSE IF Game State > 1
            Game State = Game State - 3
        END

This would make all losing states -1, all draws 0, and all winning states 1.  Why do we want this?  Well, the Game Scores are 0, 3 and 6 for a Win/Lose/Draw.  Let's do a little re-factoring.

        0 = 0 * 3
        3 = 1 * 3
        6 = 2 * 3

Or...

        0 = (-1 + 1) * 3
        3 = ( 0 + 1) * 3
        6 = ( 1 + 1) * 3

Yup, we can ue the relationship between the moves to calculate the score.  But I did say this solution was mathematical.  Let's lose those IF statements with modulo.  We have 3 outcomes (-1, 0 and 1), so we want to use modulo 3, which will return 0, 1 or 2, so let's add 1 initially so that the outcomes are 0, 1 or 2 from the initial subtraction, then add 3 to handle that nasty -2.

Giving us:

        Player 1     : Player 2     : Winner   : Outcome
        =============:==============:==========:==========
        Rock (1)     : Rock (1)     : Draw     : ((1 - 1 + 4) % 3) - 1 =  0
        Rock (1)     : Paper (2)    : Player 2 : ((2 - 1 + 4) % 3) - 1 =  1
        Rock (1)     : Scissors (3) : Player 1 : ((3 - 1 + 4) % 3) - 1 = -1
        Paper (2)    : Rock (1)     : Player 1 : ((1 - 2 + 4) % 3) - 1 = -1
        Paper (2)    : Paper (2)    : Draw     : ((2 - 2 + 4) % 3) - 1 =  0
        Paper (2)    : Scissors (3) : Player 2 : ((3 - 2 + 4) % 3) - 1 =  1
        Scissors (3) : Rock (1)     : Player 2 : ((1 - 3 + 4) % 3) - 1 =  1
        Scissors (3) : Paper (2)    : Player 1 : ((2 - 3 + 4) % 3) - 1 = -1
        Scissors (3) : Scissors (3) : Draw     : ((3 - 3 + 4) % 3) - 1 =  0

But of course the eagled eyed of you will have spotted we've subtracted 1, but aren't we just going to add that back on for the * 3 to get the score.  Why yes.

        Player 1     : Player 2     : Winner   : Score
        =============:==============:==========:==========
        Rock (1)     : Rock (1)     : Draw     : ((1 - 1 + 4) % 3) * 3 = 3
        Rock (1)     : Paper (2)    : Player 2 : ((2 - 1 + 4) % 3) * 3 = 6
        Rock (1)     : Scissors (3) : Player 1 : ((3 - 1 + 4) % 3) * 3 = 0
        Paper (2)    : Rock (1)     : Player 1 : ((1 - 2 + 4) % 3) * 3 = 0
        Paper (2)    : Paper (2)    : Draw     : ((2 - 2 + 4) % 3) * 3 = 3
        Paper (2)    : Scissors (3) : Player 2 : ((3 - 2 + 4) % 3) * 3 = 6
        Scissors (3) : Rock (1)     : Player 2 : ((1 - 3 + 4) % 3) * 3 = 6
        Scissors (3) : Paper (2)    : Player 1 : ((2 - 3 + 4) % 3) * 3 = 0
        Scissors (3) : Scissors (3) : Draw     : ((3 - 3 + 4) % 3) * 3 = 3

So the algorithm is:

        Total Score = 0
        
        FOR Each Round

            Score = 0

            Translate the Elfs Letter A/B/C to Rock/Paper/Scissors

            Translate our Letter X/Y/Z to Rock/Paper/Scissors

            Total Score = Total Score + (((Our Move - Elfs Move + 4) % 3) * 3) + Our Move

        NEXT

Note: This *only* works because of the numeric distance, if you changed the values assigned to Rock/Paper/Scissors, it would potentially break the algorithm above.

**Part 2**

For Part 2, X/Y/Z are the moves you have to make.  X being Rock, Y being Paper, Z being Scissors.

This time we just have to adjust our truth table a bit...

        Player 1     : Winner       : Player 2
        =============:==============:=========
        Rock (A)     : Player 1 (X) : Scissors
        Rock (A)     : Player 2 (Y) : Paper
        Rock (A)     : Draw (Z)     : Rock
        Paper (B)    : Player 1 (X) : Rock
        Paper (B)    : Player 2 (Y) : Scissors
        Paper (B)    : Draw (Z)     : Paper
        Scissors (C) : Player 1 (X) : Paper
        Scissors (C) : Player 2 (Y) : Rock
        Scissors (C) : Draw  (Z)    : Scissors

Again the same three techniques apply.

*Algorithmically*

With this method, the pseudo code is something like:

        Total Score = 0
        
        FOR Each Round

            Score = 0

            Translate the Elfs Letter A/B/C to Rock/Paper/Scissors

            Translate our Letter X/Y/Z to Lose/Draw/Win

            If we Won Add the Winning Move to the Score
            If we Drew Add the Drawing Move to the Score
            If we Lost Add the Losing Move to the Score

            Add the Lose/Draw/Win Value to the Score

            Total Score = Total Score + Score

        NEXT

And using the truth table, the logic for picking the right move is similar to Part 1.


*Lookup*

As per Part 1:

        Player 1     : Winner       : Player 2     : Score
        =============:==============:==============:==========
        Rock (A)     : Player 1 (X) : Scissors (3) : 0 + 3 = 3
        Rock (A)     : Draw (Y)     : Rock (1)     : 3 + 1 = 4
        Rock (A)     : Player 2 (Z) : Paper (2)    : 6 + 2 = 8
        Paper (B)    : Player 1 (X) : Rock (1)     : 0 + 1 = 1
        Paper (B)    : Draw (Y)     : Paper (2)    : 3 + 2 = 5
        Paper (B)    : Player 2 (Z) : Scissors (3) : 6 + 3 = 9
        Scissors (C) : Player 1 (X) : Paper (2)    : 0 + 2 = 2
        Scissors (C) : Draw  (Y)    : Scissors (3) : 3 + 3 = 6
        Scissors (C) : Player 2 (Z) : Rock (1)     : 6 + 1 = 7

Giving:

        Outcomes["A", "X"] = 3
        Outcomes["A", "Y"] = 4
        Outcomes["A", "Z"] = 8
        Outcomes["B", "X"] = 1   
        Outcomes["B", "Y"] = 5
        Outcomes["B", "Z"] = 9
        Outcomes["C", "X"] = 2
        Outcomes["C", "Y"] = 6
        Outcomes["C", "Z"] = 7

        Total Score = 0
        
        FOR Each Round

            Score       = Outcomes[Elfs Letter, Our Letter]
            Total Score = Total Score + Score

        NEXT

*Mathematically*

This is trying to now go the other way from Part 1.  Before we used Our Move vs the Elf Move, and looked for a -1 / 0 / 1 Delta to determine Win/Lose/Draw.  Now we just have to use the -1 / 0 / 1 delta to determine the move, but instead of subtracting, we now add to reverse the operation

        Rock (1)     +  1 (Win)  = 2 (Paper)
        Rock (1)     +  0 (Draw) = 1 (Rock)
        Rock (1)     + -1 (Lose) = 0 (Invalid, should be 3 (Scissors))
        Paper (2)    +  1 (Win)  = 3 (Scissors)
        Paper (2)    +  0 (Draw) = 2 (Paper)
        Paper (2)    + -1 (Lose) = 1 (Rock)
        Scissors (3) +  1 (Win)  = 4 (Invalid, should be 1 (Rock))
        Scissors (3) +  0 (Draw) = 3 (Scissors)
        Scissors (3) + -2 (Lose) = 2 (Paper)

So again, two edge cases, and again we can add/subtract 3 to resolve, meaning modulo to the rescue.  But of course, currently, our data is in the range 1 .. 3, but the result will be 0 .. 2.  The outputs we'd get using: (Elf Move + Game State) % 3 would be:

        (Rock (1)     +  1 (Win))  % 3 = 2 (Paper)
        (Rock (1)     +  0 (Draw)) % 3 = 1 (Rock)
        (Rock (1)     + -1 (Lose)) % 3 = 0 (Invalid, should be 3 (Scissors))
        (Paper (2)    +  1 (Win))  % 3 = 0 (Invalid, should be 3 (Scissors))
        (Paper (2)    +  0 (Draw)) % 3 = 2 (Paper)
        (Paper (2)    + -1 (Lose)) % 3 = 1 (Rock)
        (Scissors (3) +  1 (Win))  % 3 = 1 (Rock)
        (Scissors (3) +  0 (Draw)) % 3 = 0 (Invalid, should be 3 (Scissors))
        (Scissors (3) + -2 (Lose)) % 3 = 2 (Paper)

This fixed rock, but globally broke Scissors, and we'd still an if statement to fix that.  What would simplify this is if Rock was 0, Paper was 1 and Scissors was 2, and we can do that by adding 2 prior to the modulo to rotate the values around.  Then add 1 post modulo to get them into their proper states.

        ((Rock (1)     +  1 (Win)  + 2) % 3) + 1 = 2 (Paper)
        ((Rock (1)     +  0 (Draw) + 2) % 3) + 1 = 1 (Rock)
        ((Rock (1)     + -1 (Lose) + 2) % 3) + 1 = 3 (Scissors)
        ((Paper (2)    +  1 (Win)  + 2) % 3) + 1 = 3 (Scissors)
        ((Paper (2)    +  0 (Draw) + 2) % 3) + 1 = 2 (Paper)
        ((Paper (2)    + -1 (Lose) + 2) % 3) + 1 = 1 (Rock)
        ((Scissors (3) +  1 (Win)  + 2) % 3) + 1 = 1 (Rock)
        ((Scissors (3) +  0 (Draw) + 2) % 3) + 1 = 3 (Scissors)
        ((Scissors (3) + -2 (Lose) + 2) % 3) + 1 = 2 (Paper)

But of course we actually want Lose/Draw/Win to be 0, 1, 2 for the multiplication to work, so this actually becomes:

        ((Rock (1)     +  1 (Win)  + 1) % 3) + 1 = 2 (Paper)
        ((Rock (1)     +  0 (Draw) + 1) % 3) + 1 = 1 (Rock)
        ((Rock (1)     + -1 (Lose) + 1) % 3) + 1 = 3 (Scissors)
        ((Paper (2)    +  1 (Win)  + 1) % 3) + 1 = 3 (Scissors)
        ((Paper (2)    +  0 (Draw) + 1) % 3) + 1 = 2 (Paper)
        ((Paper (2)    + -1 (Lose) + 1) % 3) + 1 = 1 (Rock)
        ((Scissors (3) +  1 (Win)  + 1) % 3) + 1 = 1 (Rock)
        ((Scissors (3) +  0 (Draw) + 1) % 3) + 1 = 3 (Scissors)
        ((Scissors (3) + -2 (Lose) + 1) % 3) + 1 = 2 (Paper)

So the algorithm is:

        Total Score = 0
        
        FOR Each Round

            Score = 0

            Translate the Elfs Letter A/B/C to Rock/Paper/Scissors

            Translate our Letter X/Y/Z to Lose/Draw/Win

            Total Score = Total Score + (((Our Move + Elfs Move + 1) % 3) + 1) + (Lose/Draw/Win * 3)

        NEXT

**Python vs C++ vs C**

**Python**

The Python solution implements the Algorithm/Lookup solutions.

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

This does the algorithmic one, the only delta is that I translate X/Y/Z to 1, 2, 3, not 0, 1, 2 to prioritise Part 1.