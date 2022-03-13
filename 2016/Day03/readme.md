# Day 3

https://adventofcode.com/2016/day/3

Time for something a bit different... there's really two aspects to this puzzle.

1. Figuring out how to parse a formatted text file.
2. Determining whether any two sides of the triangle are greater than the third.

**Part 1**

For the reading of the file, there's a few ways of doing it, but the simplest (to me) is:

    FOR each line of text
    
      Keep replacing duplicate spaces with single spaces until all we have is three numbers with a single space dividing them
      
        i.e. instead of:    X   Y   Z
             we now have:   X Y Z

      Convert each of the textual numbers into integers as chances are, the language you're using will interpret "123" differently to 123.

      Store the current set of 3 numbers

    NEXT

In terms of how to work out if any two sides are larger than the remaining side, we could take the triangle co-ordinates X,Y,Z and compare the following:

    X+Y>Z
    X+Z>Y
    Y+X>Z
    Y+Z>X
    Z+X>Y
    Z+Y>X

And of course because operands for "+" are associative https://en.wikipedia.org/wiki/Operator_associativity

We can condense this to:

    X+Y>Z (equivalent to Y+X>Z)
    X+Z>Y (equivalent to Z+X>Y)
    Y+Z>X (equivalent to Z+Y>X)

But even so, we can condense this further.  The *best* chance we have of proving an invalid triangle is to take the *smallest* pair of values, and compare it to the remaining value.  It's impossible for this to pass the triangle test, but for other permutations to fail.

Meaning we can do:

    Smallest     Next Smallest      Largest
    MIN(X,Y,Z) + MID(X,Y,Z)     >   MAX(X,Y,Z)

**Part 2**

Here's where all your due dillegence sorting the data is used against you.  So far you've converted:

    X1   Y1   Z1
    X2   Y2   Z2
    X3   Y3   Z3
    X4   Y4   Z4
    X5   Y5   Z5
    X6   Y6   Z6

into:

    Triangle 1: [X=X1, Y=Y1, Z=Z1]
    Triangle 2: [X=X2, Y=Y2, Z=Z2]
    Triangle 3: [X=X3, Y=Y3, Z=Z3]
    Triangle 4: [X=X4, Y=Y4, Z=Z4]
    Triangle 5: [X=X5, Y=Y5, Z=Z5]
    Triangle 6: [X=X6, Y=Y6, Z=Z6]

But now we find out it's actually columns, meaning the same data set would actually be:

    Triangle 1: [X=X1, Y=X2, Z=X3]
    Triangle 2: [X=Y1, Y=Y2, Z=Y3]
    Triangle 3: [X=Z1, Y=Z2, Z=Z3]
    Triangle 4: [X=X4, Y=X5, Z=X6]
    Triangle 5: [X=Y4, Y=Y5, Z=Y6]
    Triangle 6: [X=Z4, Y=Z5, Z=Z6]

The only way this is legal is if the original row count of data is a multiple of 3.  So you could re-parse all the data, now splitting into columns, but the simpler method would be to just take our existing triangles and re-arrange the data.

    New Triangle 1: [Triangle 1 [X], Triangle 2 [X], Triangle 3 [X]]
    New Triangle 2: [Triangle 1 [Y], Triangle 2 [Y], Triangle 3 [Y]]
    New Triangle 3: [Triangle 1 [Z], Triangle 2 [Z], Triangle 3 [Z]]
    New Triangle 4: [Triangle 4 [X], Triangle 5 [X], Triangle 6 [X]]
    New Triangle 5: [Triangle 4 [Y], Triangle 5 [Y], Triangle 6 [Y]]
    New Triangle 6: [Triangle 4 [Z], Triangle 5 [Z], Triangle 6 [Z]]

Once we have our new set of triangles, the rest of the solution is identical.

**Python vs C++ vs C**

**Python**

In all honesty, the most complex line here is probably the list comprehension line to read in the file.

    triangles = [[int(x) for x in " ".join(line.strip().split()).split(" ")] for line in inputFile.readlines()]

This is equivalent to:

    for line in inputFile.readlines() :
        cleanedLine = line.strip()
        while "  " in cleanedLine :
            cleanedLine = cleanedLine.replace("  ", " ")
        #end
        triangle = []
        for X in cleanedLine.split(" ") :
            triangle.append(int(X))
        #end
        triangles.append(triangle)
    #end

There's no shame in not using the list comprehension method, especially if you're more familiar with other languages, but like most things, it's best to embrace the features of a language if you can.

The rest of the solution basically matches Part 1/2 above pretty closely.

**C++**

Mostly follows the Python.  Encapsulated the triangle handling in it's own class because... C++.  You may notice some weirdness with typedef'ing an array, that's mostly because C/C++ are a bit... fiddly, when it comes to returning fixed sized arrays.  Typedef'ing or wrapping in a struct is the typical workaround here.  Personally, I disliking hiding arrays behind typedefs, as this often adds ambiguity, but it's a necessary evil here.

**C**
