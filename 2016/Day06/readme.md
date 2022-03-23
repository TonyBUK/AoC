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

Note: You could actually have the first loop also keep track of the maximum as well which would eliminate most of the second loop, but for reasons which will be more obvious in Part 2, I've not done this.

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

And had I gone with using the first loop to also determine the maximum, I'd have been somewhat scuppered for part 2, since the minimum is something you can't easily keep tabs on as you go compared to the maximum, not without some sort of sorting/constant re-evaluation of all the unqiue letter metrics.

**Python vs C++ vs C**

**Python**

Once again, I could have solved this with the collections module, but the algorithm is simple/quick enough that being verbose isn't the worst thing.  The usage of infinite is basically a trick to allow the first minimum comparison to always pass.  You could just use an impossibly large value, even one derived from the data set (for example, number of rows + 1), but infinite is a cheesy way of achieving the same thing with a constant.  The alternative is typically some sort of check like:

    IF v < min OR first
    
      first = False
      min   = v
      
    END

**C++**

Essentially the Python solution but class...ified.  I don't think this is necessarily the worst split, albeit there's zero reason this couldn't have just been a carbon copy of the Python solution.

My desire to make all things const meant I had to give a way of notifying that all the data had been added via the calculate function, otherwise I could have automatically detected this whenever Min/Max were called and had it as a private method.

I did a similar trick to the initialisation of min/max, albeit in this instance, instead of INFINITE, which really isn't a thing for integers in C/C++, I simply fed -1 into an unsigned type and let underflow take care of the rest.

i.e.

    Assume the data is 8 bits wide

     -1 == 0xFF (signed)
    255 == 0xFF (unsigned)

The most famous underflow bug is probably Gandhi in civilization, albeit in my case, it's a behaviour I actively want.

**C**

