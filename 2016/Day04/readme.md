# Day 4: Security Through Obscurity

https://adventofcode.com/2016/day/4

A relatively simple one that really requires three key aspects:

1. File parsing of a data structure.
2. Gather metrics about the data structure.
3. Performing a repeated operation on the data structure.

**Part 1**

The input data format is essentially:

ABC-DEF-GHI-YYY[ZZZZZ]

Where YYY is an integer representing the Sector ID, and ZZZZZ is a 5 character checksum which is the five most frequent characters in the input data structure ABC-DEF-GHI (not including dashes).

So for part 1, we're validating the checksum, and using the sum of all the valid Sector ID's to prove we've done so.  So really this is a matter of, for each unique character in the encrypted data, determining how many times each occurs, and picking the 5 most frequent (or in the case of a tie, going with the highest alphabetically).

One easy way to achieve this is with a hash map...

    Initialise an empty hash map to store unique characters, and frequency of occurence

    FOR each encrypted character
    
      IF does not the contains the encrypted character
        Add a new hash map entry using the encrypted character as the index, initialised to 1
      ELSE
        Increment the hash map entry using the encrypted character as the index
      END

    END

What we'll end up with here is a hash map, indexed by each unique character, with a value of the number of times it occured in the encrypted string.  Something like:

    kStringMetrics["a"] = 5
    kStringMetrics["c"] = 2
    kStringMetrics["d"] = 4
    kStringMetrics["g"] = 2
    kStringMetrics["h"] = 5
    kStringMetrics["x"] = 1

But this isn't really something we can use... yet.  What we need is to swap the data/index around.

    Initialise an empty hash map to store unique frequencies, and the characters which fulfill those frequencies
    
    FOR each entry in the previous hash map
    
      IF the value of the current entry (character frequency) is not in the new hash map
        Add a new hash map entry using the frequency as the index, initialised to the current entry index (character)
      ELSE
        Append the current entry index (character) to the new hash map, and sort them alphabetically
      END
      
    END

What we should now end up with is:

    kStringMetricsByFrequency[1] = "x"
    kStringMetricsByFrequency[2] = "cg"
    kStringMetricsByFrequency[4] = "d"
    kStringMetricsByFrequency[5] = "ah"

And this is something we can now use.  We just keep appending string entries from highest to lowest, and use the first 5.  i.e.

     =====
    "ahdcgx"
     =====

If that checksum matches what we've extracted from the input string, it's valid.

**Part 2**

As mentioned in the puzzle, the encryption method is: https://en.wikipedia.org/wiki/Caesar_cipher, probably one of the first ways of encoding data we learn when we're younger.

What we've been asked to perform is to shift each letter by the Sector ID, which tends to be in the thousands.

i.e. if the input character is "A" and we have to rotate it 2 times, it will transform to "C".  Similarly if we have a "Z" and need to shift it 2 times, it will transform to "B".

So we could perform :

    FOR Sector ID times :
    
      Rotate each character once
      
    END

But there's a shortcut.  Instead of thinking of the alphabet as letters, think of them as numbers 0 - 25.  i.e.

    A = 0
    B = 1
    ...
    Z = 25
    
To perform the previous actions of rotating A and Z, we can just add the number of times we need to rotate it.

    C = A + 2 : 2 = 0 + 2
    B = Z + 2 : 1 = 25 + 2..... uh oh...

... and apply a modulo.  So to shift N times, it's:

    Decrypted = (Encrypted + SectorID) % 26

So now how do we know which one is the North Pole....... here's where the puzzle falls a bit flat to me.  What you've been told is:

    What is the sector ID of the room where North Pole objects are stored?

The problem is to know this, you'd need to know that the word: "northpole" is stored within the matching object.  So it's hinted upon, but not explicitly defined, meaning unless you know this, the algorithm is going to be:

    FOR each Input
    
      IF the Checksum is Valid
      
        Decrypt the String
        
        Print the Decrypted String
        
      END
      
    END
    
    Use fleshy human eyeballs over printout to find something that jumps out as being the right answer
    
    Re-write code to search for the string "northpole"

**Python vs C++ vs C**

**Python**

Python does have a collections package which contains functionality to help perform part 1 (analysing an input data set and returning frequency metrics), however it's such a trivial operation, I think it's more useful to show this verbosely.

This again uses list comprehension for the decryption routine, equivalent code has been left in as comments.  The only other part that might need explaining is why there's a string containing the alphabet present.

Rather than performing some ASCII conversion, which would be something like:

    nEncrypted = ord(C) - ord('a')
    nDecrypted = (nEncrypted + nSectorId) % 26 # or ord('z') - ord('a') + 1
    nEncoded   = chr(nDecrypted + ord('a'))

We instead just use an array a-z, and use a simple search/lookup to go between a index / value.  Making the equivalent code:

    ALPHABET   = "abcdefghijklmnopqrstuvwxyz"
    nEncrypted = ALPHABET.index(C)
    nDecrypted = (nEncrypted + nSectorId) % 26 # or len(ALPHABET)
    nEncoded   = ALPHABET[nDecrypted]

**C++**

My god, it's full of bloat...  basically this is the Python solution but distributed across way more files than it should be, and arguably, I held back too!  One could argue that the encryption class should be derived from some form of base class.  It does at least leave the primary code item, solver.cpp, as a very clean solution, but certainly unless I knew both parts of the puzzle up front (which I did), I'd never code the solution this way, primarily as part 2 more often than not invalidates the part 1 solutions scaleability/extendibility.

That said, the real indicator as to whether the decomposition was worth it will be whether any of it can be re-used down the line.  In this case I suspect not, but in upcoming years, this sort of approach will definately pay off.

**C**

Basically a trimmed down C++ solution without hash maps.  This also acts destructively to the encrypted data, but for the purposes of the puzzle, this is basically fine.  Also you may notice that this actually does perform the ASCII conversion that was derided in the Python solution, primarily because in C (and C++ as well), char's are just integers, so we can easily go to/from ASCII encoding very quickly.
