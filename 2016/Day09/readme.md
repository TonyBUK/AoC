# Day 9

https://adventofcode.com/2016/day/9

This problem is a scaleability issue, whereby the puzzle can be somewhat trivially solved by actually performing the expansion, but the expectation is to find another way, as all they want to know is the decompressed length, not the actual contents of the decompressed text.

**Part 1**

So the naive approach is something like:

    Decompressed Text = ""
    
    WHILE the Compressed String is being Processed
    
      IF the Currext Part of the Compressed String is a Decompression Sequence (Length x Frequency)

        Compute the Length and Frequency
        
        Copy the next X letters as determined by the Length part of the Decompression Sequence and store as the Expansion Text
        
        REPEAT Frequency Times
        
          Append the Expansion Text to the Decompressed String
        
        END
        
    ELSE
    
      Append the Current Part of the Compressed String to the Decompressed String
      
    END

The faster approach is:

    Decompressed Length = 0
    
    WHILE the Compressed String is being Processed
    
      IF the Currext Part of the Compressed String is a Decompression Sequence (Length x Frequency)

        Compute the Length and Frequency
        
        Add (Length * Frequency) to Decompressed Length
        
    ELSE
    
      Add the length of the Current Part of the Compressed String to the Decompressed Length
      
    END

**Part 2**

So this is subtle but it essentially now turns this into a repeated expansion.  If we consider their first example:

    X(8x2)(3x3)ABCY
    
Part 1 expands this as:

    X(3x3)ABC(3x3)ABCY

This is because if an expansion contains an expansion, that is ignored in Part 1, as we only expand text that *isn't* part of an existing expansion.

Part 2 expands this as:

    XABCABCABCABCABCABCY

This is basically taking the expansion from Part 1, and repeating it until no expansions remain.  So there's a few ways of doing this if we go the naive route.

Method 1: Repeat until everything has been expanded

    Decompressed String = Compressed String
    
    WHILE the Decompressed String contains any expansion seqeuences
    
      Perform the Naive Method from Part 1 to Decompress the String
      
    END

I mean it will work and is simple, but you're going to be waiting a long time for this to solve the puzzle.

On my M1, this is about 7 1/2 minutes.


Method 2: Recurse until everything has been expanded (Naive)

Let's go back to the naive method from Part 1, and make a couple of tweaks.

    RECURSIVE Decompression

      Decompressed Text = ""

      WHILE the Compressed String is being Processed

        IF the Currext Part of the Compressed String is a Decompression Sequence (Length x Frequency)

          Compute the Length and Frequency

          Copy the next X letters as determined by the Length part of the Decompression Sequence and Store as the Expansion Sequence
          
          RECURSE on the Expansion Sequence and store the result as the Expansion Text

          REPEAT Frequency Times

            Append the Expansion Text to the Decompressed String

          END

      ELSE

        Append the Current Part of the Compressed String to the Decompressed String

      END

    END

This is an order of magnitude faster, and is the blueprint for the quicker solution.  1.6s on my M1.


Method 2: Recurse until everything has been expanded (Lengths Only)

    RECURSIVE Decompression

      Decompressed Length = 0

      WHILE the Compressed String is being Processed

        IF the Currext Part of the Compressed String is a Decompression Sequence (Length x Frequency)

          Compute the Length and Frequency

          Copy the next X letters as determined by the Length part of the Decompression Sequence and Store as the Expansion Sequence
          
          RECURSE on the Expansion Sequence and Store the returned length as Expanded Length

          Add (Expanded Length * Frequency) to Decompressed Length

      ELSE

        Add the length of the Current Part of the Compressed String to the Decompressed Length

      END

    END

And an order of magnitude once again, now expressible in ms on my M1.


**Python vs C++ vs C**

**Python**

I think my only complaint really is that Python loops are alot more restrictive than say C/C++.  In C/C++, I can define a loop but manipulator the iterator within the loop, whereas in Python, if I want more control, I have to go with a WHILE loop and manually handle the index.  Otherwise, pretty easy to implement, multiple returns helps as well.  Especially since I implemented both the slow/fast versions of this algorithm.


**C++**

To be honest the optimisation for time doesn't lend itself too well to avoiding repetition when creating this as a class.  There's ways around this, albeit it'd probably end up involving templates / function pointers.  Additionally, the whole JIT methodology for decompressing/length calculation breaks my whole const ethos for accessor only functions, and I'd prefer not making multiple member functions mutable as that'd defeat what const functions tries to achieve.

That said, as ever, it does lead to a very clean top level function compared to other solutions.


**C**
