# Day 5

https://adventofcode.com/2016/day/5

This is basically 2015 Day 4 with a very slight twist.  Personally I'm not a fan of puzzles that basically rely upon either the functionality being available out of the box, or reliant upon downloading some COTS library... suffice as to say I'm doubtful anyone went and downloaded the MD5 specification and coded their own implementation.

https://datatracker.ietf.org/doc/html/rfc1321 to prove me wrong!

**Part 1**

Basic algorithm is:

    Start the Counter at 0

    WHILE the Password isn't 8 Characters Long
    
      Append the current counter to the Door ID
      
      Calculate the MD5 of the of the Door ID/Counter
      
      IF the MD5 begins with "00000" (positions 0 .. 4)
      
        Add the next character in the MD5 hash to the Password (position 5)
        
      END
      
      Increment the Counter
      
    END

The problem with the puzzle here is this really forces you down a brute force solution, in that there's no trivial way of coming up with a list of counter values that would generate the right MD5 preamble, so get ready to sit patiently whilst the password generates.

**Part 2**

This is really just a more different part 1.

    Start the Counter at 0

    Initialise the Password to "        "
                                01234567

    WHILE the Password still contains spaces
    
      Append the current counter to the Door ID
      
      Calculate the MD5 of the of the Door ID/Counter
      
      IF the MD5 begins with "00000" (positions 0 .. 4)
      
        IF the next character (position 5) in the MD5 hash is a number between 0 and 7
        
          IF the password at this position is still a space
          
            Add the next character in the MD5 hash to the Password (position 6)
          
          END
        
        END

        Add the next character in the MD5 hash to the Password
        
      END
      
      Increment the Counter
      
    END

As you probably spotted, the "optimisation" that you can get is merging Parts 1 and 2 together such that the MD5's are only processed once, since this is going to be the expensive part of the calculation.  This saves around 30% of the total time on my input data.

Of course this time is all then squandered on the stretch goal aspect, being to make the decryption cinematic.  I went with the cheezy rapidly changing random values progressively becoming the password.  This does completely negate any time saved with the optimisation pass!

Interestingly, in some of the later years, there are definately puzzles where I would recommend at least having the ability to squander time on outputting what's happening, but I'll address those as they come up.

**Python vs C++ vs C**

**Python**

Not that much to say as everything I said in 2015's Day 4 solution is equally applicable here.  Python is quick to get up and running, however this does incur a performance penalty for how it's implemented that C/C++ can bypass, mostly being unnecessary to/from with stringification of the MD5 itself.

**C++**


**C**

