# Day 6

https://adventofcode.com/2016/day/6

Conceptually this is similar to Day 3 (Part 2), where we're being asked to do some sort of processing on columns instead of rows.

**Part 1**

There's a few ways of going about this, some might read the data in rows and transpose the data itself to gain access to various search/count functions.  I've gone with leaving the data untouched and merely gathering metrics about the data as I go.

Basic algorithm is:

    FOR each Line
    
      FOR each Letter in the Line
      
        Increment a Counter for how many times this Letter has appeared in this column
        
      END
      
    END

    Initialise the Decrypted String to Blank

    FOR each Column
    
      Initialise the Used Letter to blank, and the maximum encountered frequency to 0

      FOR each unique letter that appears in the Column
      
        IF the frequency of the current unique letter is higher than the maximum encountered frequency
        
          Set the maximum encountered frequency to the frequency of the current unique letter
          
          Set the Used Letter to the current unique letter
          
        END
        
      END
      
      Append the Used Letter to the Decrypted String
      
    END

**Part 2**

This is really just a more different part 1.

Everything is the same, exept for the fact we now want the mimumum encountered frequency, meaning the latter part of the algorithm is now...

    Initialise the Decrypted String to Blank

    FOR each Column
    
      Initialise the Used Letter to blank, and the minimum encountered frequency to infinite

      FOR each unique letter that appears in the Column
      
        IF the frequency of the current unique letter is lower than the minimum encountered frequency
        
          Set the minimum encountered frequency to the frequency of the current unique letter
          
          Set the Used Letter to the current unique letter
          
        END
        
      END
      
      Append the Used Letter to the Decrypted String
      
    END

And of course, it's perfectly possible to merge two solutions by looking for the minimum/maximum frequencies at the same time.

**Python vs C++ vs C**

**Python**

Once again, I could have solved this with the collections module, but the algorithm is simple/quick enough that being verbose isn't the worst thing.  The usage of infinite is basically a trick to allow the first minimum comparison to always pass.  You could just use an impossibly large value, even one derived from the data set (for example, number of rows + 1), but infinite is a cheesy way of achieving the same thing with a constant.  The alternative is typically some sort of check like:

    IF v < min OR first
    
      first = False
      min   = v
      
    END

**C++**

**C**

