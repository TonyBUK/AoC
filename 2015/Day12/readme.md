# Day 12

https://adventofcode.com/2015/day/12

This introduces:
- Language/Library Selection

So this is one where it's really a case of the have's and the have nots much like Day 4.  Either you're using a language which readily has a simple JSON parser built into the default library set, or you don't, meaning you either have to find one, or make your own.

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

