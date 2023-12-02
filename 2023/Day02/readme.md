# Day 2: Cube Conundrum

https://adventofcode.com/2023/day/2

This is an easier challenge compared to Day 1, as the complexity isn't in the task, it's in the input data processing, so let's focus on that.

**Part 1**

The data will be provided in this format:

    Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
    Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
    Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
    Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
    Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green

If we focus on one line:

    Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green

The key characters are:

    Colon     - ":"
    Semicolon - ";"
    Space     - " "

So what we need to focus on is splitting up the data using this, so let's start by splitting the Cubes from the Game, by using the Colon Separator.  This gives us:

    Game 1
     3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green

Notice there's a leading space in front of the first 3, that's expected, and we'll need to deal with that later.  So let's process the easier of the two, the Game Id.  This is now text, space and anumber, so let's split this up using the Space Separator.

    Game
    1

We now have the Game Id isolated, remember though, this will be ASCII text, so we'll need to convert it to an Integer using the features of your language.

Next up, the harder part, the Cubes.  We can take that text and split by the Semicolon separator.

     3 blue, 4 red
     1 red, 2 green, 6 blue
     2 green

What we now have is the three sub-games.  So let's focus on a single sub-game and apply the Comma Separator.

     3 blue
     4 red

Now remember that leading space, it's still there!  So we either need to clean the string first by removing leading/trailing spaces, or remember when splitting that the first element will be blank.

So if we split by the final seperator, Space, we get:

    3
    blue

Now we have our colour, and the quantity.  So to bring it all together, we should hopefully end up with a structure looking like:

    GAMES[1] = [
        ["red" : 4, "green" : 0, "blue" 3],
        ["red" : 1, "green" : 2, "blue" 6],
        ["red" : 0, "green" : 2, "blue" 0]
    ]

So from here we need to solve part 1, where we're told that the maximum number of cubes per game is 12 red, 13 green and 14 blue.  So all we have to do is go through each sub-game in each game, and see whether the number of red/green/blue exceed these.

So to bring it all together:

    Initialise the Game Total to 0

    FOR each Game

        Initialise the current game as Possible

        FOR each Sub-game

            IF the number of Red/Green/Blue cubes exceeds 12/13/14 respectively

                Flag the current game as Impossible

            END

        END

        IF the current game is *still* Possible

            Increment the Game Total by the current Game Id

        END

    END


**Part 2**

This is using the data but in a slightly different way.  All the processing from Part One is still valid, but we now only care about the maximum number of cubes in each game.

So if we take the same data as before:

    GAMES[1] = [
        ["red" : 4, "green" : 0, "blue" 3],
        ["red" : 1, "green" : 2, "blue" 6],
        ["red" : 0, "green" : 2, "blue" 0]
    ]

What we now care about is finding the highest in each column, being:

    red:   4
    green: 2
    blue:  6

And multiplying these together.  So the actual process isn't that different to before.

    Initialise the Game Power to 0

    FOR each Game

        Initialise the Highest Red/Green/Blue Cubes to 0

        FOR each Sub-game

            IF the number of Red/Green/Blue cubes exceeds the current highest number of Red/Green/Blue Cubes

                Update the highest number of Red/Green/Blue Cubes

            END

        END

        Increment the Game Power by the result of multiplying the Highest Red/Green/Blue Cubes for the Current game together

    END



**Python vs C++ vs C**

**Python**

The Python solution now goes into some meatier dictionary comprehension.  To make it easier, I've left all three versions in the code, to show you the journey of how to get from one to the other.

Personally, I think the sweet spot is the "medium" version for speed/readibility, but the Hard versino is included for completeness.  Remember, debugging *inside* of a comprehension statement is an absolute pain.  Better to write it as a simple loop first, then refine incrememntally.


**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

This takes the shortcut my solution ignored.  At no point do we need all of the sub-games, all we ever need is the maximum values, as for Part One, if a game is possible, it means the *maximum* values for each Cube Colour were all below 12/13/14 respectively, and Part Two uses this in a more obvious direct way.

This means we never really need to store the games at all, just per game, calculate the maximum number of cubes of each colour.  If it's valid, incrememnt the Game Total by the Game Id.  And irrespective of validity, increment the Game Power by the product of the maximum number of cubes.

It is possible to go one step further, this takes something of a lazy route and reads text into a dynamic string buffer per line, it's perfectly possible to not do this and process the file one character at a time.

For example, the first space will always be followed by a number, followed by a colon.  Then after this, because all we care about is maximums, we can make some big assumptions about consistent input formatting.  Consider:

    red
    green
    blue

The starting letter is unique, so we have a consistent pattern we can follow:

    1. Find a space.
    2. Process the number until we hit the next space.
    3. Determine the colour from the first letter.
    4. Keep going until we reach the end of the line.

This means we can ignore commas and semi-colons completely.