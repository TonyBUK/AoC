# Day 6: Tuning Trouble

https://adventofcode.com/2022/day/6

This puzzle is essentially a slicing puzzle, where we're interested in finding, from any given X sized subset from a larger data set, the first time that subset contains no duplicates.


**Part 1**

So there's a few ways to do this.  The easiest is soemthing along the lines of:

    FOR each position in the larger data set

      Get a slice from the current position of size X

      Determine if all of the items in the slice are unique

      IF so, puzzle solved.

    END

The trick is *how* to check the values are unique.  The easiest way is a two dimensional check as follows.

    FOR each position in the slice

      FOR the next position slice onward

        IF the value in the current position of the slice is the same as the next position(s) in the slice

          The slice isn't unique

        END
      
      NEXT
    
    NEXT

This means if we have a slice of size 4, we need to compare:

  Element 0 to Element 1
  Element 0 to Element 2
  Element 0 to Element 3
  Element 1 to Element 2
  Element 1 to Element 3
  Element 2 to Element 3

That is essentially ever possible combination of elements, and you might note it's essentially the triangle number of the (size-1).  The problem with this is the rate at which the triangle number expands as the number of items in a slice increases.  It's not a scaleable solution... it's good enough for the Day 6 puzzle, but we can take it further.

The next method is tallies.  The problem with the slice test is that it discards the results of any prior checks, and when you're just moving your position by 1, what that really means between any two given slices is that only 2 changes have been made.  The oldest element is discarded, and we have a new element.  It's really only those that we want to re-test.

The tally system works on the following principle.

Store a data set, index'd by the letter used in the input message, which will store a counter against each one.  The idea with this is we want to store how many times a character has been found, with the solution being when we have all 1's for the number of items in the slice of interest.

It goes something like:

    Set the entire tally count to 0 for each letter

    Set the current slice to a blank string

    FOR the first few items in the input data set up to the size of the slice of interest

      Increment the Tally Counter for the current Letter

      Add the item to the current string

    NEXT

    FOR each subsequent position in the input data set

      Decrement the tally count for the first letter in the string

      Remove the first letter in the string

      Increment the tally count for the new letter (current position in the input data set)

      Add the string to the input data set

      FOR each letter in the string

        IF the Tally Count isn't 1

          We've yet to solve the puzzle, skip the remainder of the loop

        END

        We've solved the puzzle!!!

      NEXT

    NEXT

This is an improvement, in that it we're now only checking the number of letters in the slice each time.  But we can go further...

The problem with that method is most of the Tally Count hasn't changed, we only really want to test the item being removed (first letter), and the item being added (new last letter).

And of these items, there's only two transitions of interest...

For an item we're removing from the string:

- When the value changes from 1 to 0, we are losing one of our unique letters, so we want to subtract 1 from the number of unique letters.
- When the value changes from 2 to 1, we're gaining a unique letter, so we want to add 1 to the number of unique letters.
- Anything else won't change the number of unique letters.

For an item we're adding to the string:

- When the value changes from 1 to 2, we are losing one of our unique letters, so we want to subtract 1 from the number of unique letters.
- When the value changes from 0 to 1, we're gaining a unique letter, so we want to add 1 to the number of unique letters.
- Anything else won't change the number of unique letters.


So to write it as pseudocode...

    Set the entire tally count to 0 for each letter

    Set the Number of Unique Letters to 0

    Set the current slice to a blank string

    FOR the first few items in the input data set up to the size of the slice of interest

      Increment the Tally Counter for the current Letter

      IF the current Tally Counter for the current letter is 1
        Increment the Number of Unique Letters
      ELSE IF the current Tally Counter for the current letter is 2
        Decrement the Number of Unique Letters
      END

      Add the item to the current string

    NEXT

    FOR each subsequent position in the input data set

      Decrement the tally count for the first letter in the string

      IF the current Tally Counter for the first letter is 1
        Increment the Number of Unique Letters
      ELSE IF the current Tally Counter for the first letter is 0
        Decrement the Number of Unique Letters
      END

      Remove the first letter in the string

      Increment the tally count for the new letter (current position in the input data set)

      IF the current Tally Counter for the new letter is 1
        Increment the Number of Unique Letters
      ELSE IF the current Tally Counter for the new letter is 2
        Decrement the Number of Unique Letters
      END

      Add the string to the input data set

      IF the Number of Unique Letters is the Size of the String
        We've solved the puzzle!!!
      END

    NEXT


**Part 2**

This is Part 1, but looking for a longer string.  You can also (and this may depend on your input), make an assumption that this should occur after the start of packet.  The puzzle doesn't explicitly state this, but it is true of my input data, it may be true for yours.


**Python vs C++ vs C**

**Python**

This performs the first solution, being to test all characters against each other, but does it using the inbuilt set type.  A nice property of set is that it removes any duplicate items from a data set, so this entire puzzle is just reading a slice from the input data, converting it to a set, and testing the length.  It's still effectively testing everything against everything, but delegating the hard work to the Python language itself.

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

This implements the last solution, being to keep a count of unique items in a tally.  It's 8x faster than the Python solution, and with good reason!