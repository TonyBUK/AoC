# Day 1

https://adventofcode.com/2015/day/1

The actual algorithm is relatively trivial.  We need to keep track of how many "(" are present versus ")".

**Part 1**

This can be solved by taking the total number of "(", and subtracting the total number of ")".

Some languages will literally have a count feature, where you can identify the substring/character you're interested in (i.e. "(" and ")" and return the number of times they occur, failing that, there's always the good old fashion iteration method.

    FOR each character
    
      IF the character is "("
        Add 1 to the number of "(" characters
      ELSE IF the character is ")"
        Add 1 to the number of ")" characters
      ELSE
        Oh dear......
      END

    NEXT

**Part 2**

This is a variation of Part 1.  If you solved Part 1 by adding 1 to a counter each time you saw a "(", and subtracting 1 each time you saw a ")", then this part is basically solved, as you just need to figure out how many "(" and ")" you saw in total when the counter gets to -1.  This is actually where the iteration method shown in Part 1 would actually work better...

    FOR each character
    
      IF the character is "("
        Add 1 to the number of "(" characters
      ELSE IF the character is ")"
        Add 1 to the number of ")" characters
      ELSE
        Oh dear......
      END

      IF the number of "(" characters MINUS the number of ")" characters is -1
        RETURN the number of "(" characters PLUS the number of ")" characters
      END

    NEXT

**Python vs C++ vs C**

The solutions themselves are nearly identical.  The main difference is that Python/C++ solve part 1 by subtracting the number of ")" from the number of "(", the C solution, to save on all the boiler plate code needed to buffer the whole file, solves Parts 1/2 in the same way, by constantly adding/subtracting to a single counter as each instance of "(" and ")" is found respectively, whereas for C++, it's far more trivial to read the file in a similar fashion to Python.

**Times**

    Language : Time      : Percentage of Best Time
    =========:===========:=========================
    Python   : 13.46ms   : 236.65%
    C++      : 5.688ms   : 100.00%
    C        : 7.153ms   : 125.76%
