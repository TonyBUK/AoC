# Day 16

https://adventofcode.com/2015/day/16

There's not really a "trick" to this puzzle, it's really a simple matter of pattern matching one list against another.

The basic algorithm being:

    FOR each Sue...
      FOR each of Current Sue's Compounds...
        IF the Compound doesn't match the Wrapping Paper
          Move onto the next Sue
        END
      NEXT
      IF all of the Current Sue's Compounds matched... we've found our Sue
    END

Note: The Wrapping Paper may have compounds the Current Sue doesn't, this is allowed since the puzzle specifically states you can't remember all the details about each Sue, it's entirely down to whether *all* the compounds the Current Sue has match the Wrapping Paper.

**Part 1**

Uses the basic algorithm as shown...

**Part 2**

This now uses a list of compounds we want to compare using greater than or less than rules, so the basic algorithm is unchanged, it's just how we check whether the compound matches or not changes.

**Python vs C++ vs C**

**Python**

It's getting to the point where I think List/Dictionary comprehension might be making the code illegible.  The basic aim is to turn:

Sue 1: goldfish: 6, trees: 9, akitas: 0
Sue 2: goldfish: 7, trees: 1, akitas: 0

into:

{1: {"goldfish" : 6, "trees" : 9, "akitas" 0},
 2: {"goldfish" : 7, "trees" : 1, "akitas" 0}}

There's no shame in just using a normal for loop to achieve the same thing.  I've always found comprehension to be a bit counter intuitive to be honest, for example:

        auntSues = {k: v
                        for line in inputFile.readlines()
                            for k,v in extractSue(line.replace(":", "").replace(",", "").replace("\n", "").split(" ")).items()}

This works because extractSue returns a dictionary {Sue : Compounds}, and I use .items() to unravel it, but curiously, if I return: Sue, Compounds (i.e. two return values), that fails, as it seems to iterate over each item separately.

        auntSues = {k: v
                        for line in inputFile.readlines()
                            for k,v in extractSue(line.replace(":", "").replace(",", "").replace("\n", "").split(" "))}

Sometimes it's better to favour readibility, especially since this step has essentially no real bearing in the exeution time of the program.

**C++**

**C**
