# Day 10

https://adventofcode.com/2015/day/10

This introduces:
- Iterative processing
- Solution scaleability

This one processes the same data multiple times over.  This will be a running theme in later challenges, whereby the naive way of processing the data (i.e. the easy way) ends up being unsustainable as the data set grows over each iteration.  Fortunately, this puzzle isn't one of those cases...

The only wrinkle with this algorithm is that it's really dependent on no digit getting > 9 otherwise the digits need splitting.

i.e.

    1,1,1,1,1,1,1,1,1,1 => 10,1
    Whereas we'll need it to be:
    1,1,1,1,1,1,1,1,1,1 => 1,0,1

Fortunately for the input data set provided, this never occurs, albeit I've left code in place to handle this properly though it's disabled for speed.  In Python, the simplest way of handling this properly is to stringify/then split
i.e.

    10,1 => "101" => 1,0,1
    
The more mathemetically correct approach is to use log10 and modulo.

    currentPower = floor(log10(X))
    while currentPower >= 0
      digit[]      = floor(X / 10^currentPower)
      X            = X % 10^currentPower
      currentPower = currentPower - 1
    wend

**Part 1**

This is mostly a scaleability issue.  My implementation solves this by never inserting, instead, each iteration creates a new array, and data is always appended.

**Part 2**

As per part 1, and whilst it's only 10 more iterations, the output length is over 10x larger.  This is where the scaleability of the solution in Part 1 is really tested.

**Python vs C++ vs C**

**Python:**

This contains an example of a deep and a shallow copy.  Whilst the input data itself is provided by AoC as a single string, in order to allow examples, and just for completeness, my code assumes that it'll process each line of an input file.

*Deep Copy:*

    currentSeeSay = [i for i in seeSay]

This takes each value of the array seeSay, and copies each value into the new array currentSeeSay.  What this means is any modifications to currentSeeSay don't change seeSay.
Note: This only works because seeSay does not contain any embedded types that would be shallow copied, if it's a nested data source (lists of lists/dictionaries/sets) then something more intelligent is needed.  As a fallback, there's always:

    import copy
    ...
    currentSeeSay = copy.deepcopy(seeSay)

One nice thing about this is that Python forces you to do something different to duplicate large data structures, meaning that it's unlikely code throughput will be killed by an accidental deep copy.  But the flip side is that if you don't understand the difference, alot of time will be wasted understanding why data keeps getting "corrupted".

*Shallow Copy:*

    currentSeeSay = newSeeSay

This essentially makes currentSeeSay overlay newSeeSay, meaning any changes to currentSeeSay also ocur in newSeeSay and vice versa.

Most of the time shallow copies are fine, and have the benefit of being much faster compared to a deep copy, but understanding when to use each is something I'd strongly urge getting to grips with.

Note: To enable the code base to handle repetitions >= 10, swap the following instances of code (occurs twice):

    # storeSeeSay(repetitionDigit, repetitionCount, newSeeSay)
    newSeeSay.append(repetitionCount)
    newSeeSay.append(repetitionDigit)

with:

    storeSeeSay(repetitionDigit, repetitionCount, newSeeSay)
    #newSeeSay.append(repetitionCount)
    #newSeeSay.append(repetitionDigit)

**C++**

Same as Python, albeit shallow copying is really an opt-in thing, or more specifically, dependent upon class implementation etc.  In this instance, pointers have been used to ensure the copies of the arrays are always shallow.

Additionally, support for large repetitions is handled at compile time with a single define.  For the CPP version, I've left this enabled as the overhead of this is actually quite negligible.  My main grip is that the mathmatically correct solution requires floating point.  It's absolutely possible to produce an integer only variant, but it's something to keep in mind.  One thing that *VERY* rarely comes up in AoC challeneges in general is floating point.

**C**

