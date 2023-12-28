# Day 1: Trebuchet?!

https://adventofcode.com/2023/day/1

This challenge is a relatively simple pattern matching one, but with a nice twist, and unfortunately, one *very* big area of ambiguity.  The job is to find the first / last digits in a seemingly random stream of text.

**Part 1**

This part can be done in a few ways, the most common is to search through the data to find anything that looks like a number, and store it separately.  So far example:

    1abc2
    pqr3stu8vwx
    a1b2c3d4e5f
    treb7uchet

From these we would extract:

    12
    38
    12345
    7

Neatly, we're only dealing with single digits, so we only have to identifies numbers from 0 - 9.  This means we just need to loop through the string with an algorithm as follows:

    Start with an empty numerical list

    FOR each letter

        IF the letter is between "0" and "9"

            Append the numerical letter to the numerical list

        END

    END

From here, we take the first, last digits, and turn it into a value between 00 and 99.

To go through the previous values we extracted, that would give us:

    12    = 12
    38    = 38
    12345 = 15
    7     = 77

The interesting one being that one of them was only had a single digit, meaning it's both the *first* and *last* value.

Now we just need to add all of these up, so to bring the whole thing together:

    Initialise the Calibration Value to 0

    FOR each line

        Start with an empty numerical list

        FOR each letter

            IF the letter is between "0" and "9"

                Append the numerical letter to the numerical list

            END

        END

        Convert to a 2 digit number based on the first and last digit

        Add this to the Calibration Value

    END


**Part 2**

Now here's where we get a bit trickier.  To explain the first part, what we have isn't just random letters and numbers, it's random letters, words and numbers, where some of those words correspond to written forms of numbers, i.e. one, two, three etc.

    two1nine
    eightwothree
    abcone2threexyz
    xtwone3four
    4nineeightseven2
    zoneight234
    7pqrstsixteen

What we need to do is treat written numbers and digits equally.

i.e. this would be:

    two1nine         = 219
    eightwothree     = 823
    abcone2threexyz  = 123
    xtwone3four      = 2134
    4nineeightseven2 = 49872
    zoneight234      = 18234
    7pqrstsixteen    = 76

Now the interesting thing is we allow overlapping text.  So the following are all equivalent:

    eightwothree = eighttwothreee

This ambuguity isn't important in the test data, but it very well will be with the real data you have to process.  Mine *unfortunately* doesn't fully exercise this.

So now rather than just looking at one letter at a time, we need to consider a few at a time, so rather than just treating this as a list of letters:

    i.e. abcd = a, b, c, d

We need to think of this as positions in a list, i.e.

    abcd
    0123

This is important because it means when we search through the string, we can do it on a subset of the data as follows:

    abcd
    bcd
    cd
    d

This is what lets us find our words and digits...

    Start with an empty numerical list

    FOR each index within our string

        IF the first letter is a number between 0 and 9

            Append the numerical letter to the numerical list

        ELSE

            IF the first letter onwards contains a written number ("one", "two" etc.)

                Append the numerical letter represented by this to the numerical list

            END

        END

    END

And we bring this all together in exactly the same way.


**Python vs C++ vs C**

**Python**

The Python solution uses:

- Lists
This is used for iterating over the string itself, as well as storing all of the expected word'y numbers.

- List Comprehension
We use this for filtering out anything that's not a number.

- FOR Loops
We use this mainly for finding the words in a string with the relevant numbers.

- IF statements
We use this to determine if the text *starts* with a written number.

- Integer/String Conversion
This allows us to treat a written value as a number.

- Array Lookups
This is used to read a specific element or elements of an array.

- Array Value Lookups
This let's us go the other way, and use a value to determine an index.  This is the thing that let's us quickly determine which written word corresponds to which number.


**C++**

This is conceptually the same as the Python version.  The main thing this uses ontop of other features is:

- Iterators
This is really a C++ way of walking through a collection of *things*, and not necessarily arrays.  This is very similar to Python but with a more verbose syntax.  More recent C++ versions tidy this up with auto loops, but as I target the lowest common denominator, it means you get the loops in all their glory.

This also follows the Python approach to an extent, but does no string modification, and simply searches a string iteratively.  This does put it ahead speed wish compared to the Python version.

**C**

This makes use of:

- Functions
Mostly to refactor the code to keep similar processing for both parts in functions that can be called multiple times.

- Arrays
Main storage for the data being analysed for the digits.  For Part one this is technically read only, but for part two is destructively modified.

Whereas the Python solution makes a new string with the words replaced with numbers, this solution does it all in-place.  For example, if this found:

    four

It would replace it with:

    4our

Fortunately numbers "one" to "nine" are all unique within the English language, i.e. one isn't contained within another, meaning that leaving the trailing text is actually harmless, and better yet, is beneficial for allowing overlapping text.

The code also just specifically keeps track of the first/last numbers found rather than storing an array of numbers, as dynamic storage isn't C's strong point.