# Day 12

https://adventofcode.com/2015/day/12

This introduces:
- Language/Library Selection

So this is one where it's really a case of the have's and the have nots much like Day 4.  Either you're using a language which readily has a simple JSON parser built into the default library set, or you don't, meaning you either have to find one, or make your own.

It's actually kind of telling that in subsequent years, the puzzles never really tended to repeat this sort of dependency.  Personally I don't see anything particuarly wrong with this sort of puzzle, in that if you're truly interested in competing then you'll need to have selected a suitable language in the first place that helps remove all the "busy work", and if not, it's a good way to get exposed to the realities of using a selected language.

The actual puzzle once you get the data parsed is a relatively simple recursive puzzle, which goes something like:

    Recursive Function:
  
    FOR each value in the current JSON Node of Interest
    
      IF the value is an INTEGER, increase the TOTAL by 1
      
      IF the value is an ARRAY, CALL the Recursive Function with the Array as the JSON Node of Interest
      
      IF the value is an OBJECT, CALL the Recursive Function with the Object as the JSON Node of Interest

    NEXT

    RETURN the TOTAL

So as long as you have a JSON Parser and the ability to interrogate the type of each element of the JSON data, the problem is as trivial as listed above.  If you don't, then the problem becomes alot more complex...

**Part 1**

Simple call to the Recursive Function, prints the total.

**Part 2**

As per Part 1, except there's now a filter on the OBJECT path to ignore that node and all child nodes if one of the values in the OBJECT is "red".

**Python vs C++ vs C**

**Python**

Definately one of those cases where the moderness and feature rich libraries comes into play.  JSON parsing is readily available, being able to dynamically readily interrogate the type is readily available, meaning the problem is easily solved with a minimal amount of code.  The only minor complexity is iterating Arrays and Objects is ever so slightly different.

Note: For the purposes of this puzzle/JSON, in Python, Arrays are referred to as Lists, and Objects are referred to as Dictionaries.

**C++**

Whilst libraries are readily downloadable, I wanted to show that it's not always amazingly complex to roll your own.  Absolutely would not recommend using mine as a basis for any real JSON parser though.  Following the basic spec here:

https://www.json.org/json-en.html

I really only have to deal with four types:
-   Objects
-   Arrays
-   Strings
-   Numbers (ints)

Plus there's no escape sequences, inline spaces or anything overly complex/annoying in the input data.  It's very much an incomplete parser, but it's "good enough" to process the puzzle.  So really the puzzle itself is solved in the exact same method as the Python method.  I've cheated a bit with the objects type in that I force all keys to be strings, but that's more laziness that complexity.

**C**

Much like alot of the previous days... take all the work we've established before and basically scrap it.  Instead, we'll solve the data in parse phase itself.  One strategy often used in C style parses is actually to use a callback function to resolve how the parsed data is used/represented in memory (normally defined as a callback function the user defines.  A good example of this would be the XML parser expat.

https://libexpat.github.io

Basically the parser itself doesn't attempt to store the heirarchy etc., it just calls back with enough data/structural information extracted from the source data to allow something else to create a structure, but again as per C++, I'm not trying to write a JSON parser, I'm trying to write something "good enough" to solve this puzzle.  If it weren't for the filter, the callback strategy would have made sense, as it separates data/hierarchy processing from the core parser, but in the end because of the complexity this would introduce (in that I'd effectively need to be able to rollback to a given depth), it made more sense just to do all the work in the parser itself for the sake of the puzzle.

So really all this does is two things:

1.  Any time it encounters an integer, increment a global counter.
2.  Any time it finds an object with a value of red, notify the parent instance of the parser so that it can rollback the value.

So it's basically the same recursive function, except now parsing/processing is combined.  This also has the added benefit of an absurdly low footprint compared to all the other solutions, as it's really just the buffer for the data, the buffer for scratch data, and how ever many recursive calls were made to the Parse function which can be inferred from the max depth of the input data.

One thing to note is that unlike the other solutions, when "red" is encountered, the filter doesn't prevent further recursive functions being called, this is because those are still necessary since we actually still need to parse all of the data since skipping to the closing "}" is non-trivial, since you need to consider nesting, embedded "}" in strings and so on.  Simpler to allow the parser to keep going, but then discard the deltas for the purposes of this puzzle, plus in reality, it's unlikely a true parser would ever need the feature of discarding nodes at the parsing phase.

**Times**

    Language : Time      : Percentage of Best Time
    =========:===========:=========================
    Python   : 18.76ms   : 879.51%
    C++      : 5.44ms    : 255.02%
    C        : 2.13ms    : 100.00%
