# Day 12: Leonardo's Monorail

https://adventofcode.com/2016/day/12

Another nice Bytecode puzzle.  There's really two ways of solving this that I'll get into.  One is the approach of trying to squeeze as much out of your interpreter as possible, the other is native translation.  I'm going todo the former, but I'll explain the latter for my input.

**Part 1**

Let's get native translation out of the way.  This is effectively converting the input source code into native code, and even optimising as you go.  So let's pick apart my input:

    cpy 1 a
    cpy 1 b
    cpy 26 d
    jnz c 2
    jnz 1 5
    cpy 7 c
    inc d
    dec c
    jnz c -2
    cpy a c
    inc a
    dec b
    jnz b -2
    cpy c b
    dec d
    jnz d -6
    cpy 16 c
    cpy 12 d
    inc a
    dec d
    jnz d -2
    dec c
    jnz c -5

This essentially performs a small amount of unique code if c is 1, then eventually merges as follows:

    cpy 1 a
    cpy 1 b
    cpy 26 d
    jnz c 2
    jnz 1 5
      cpy 7 c
      inc d
      dec c
      jnz c -2
    cpy a c
    inc a
    dec b
    jnz b -2
    cpy c b
    dec d
    jnz d -6
    cpy 16 c
    cpy 12 d
    inc a
    dec d
    jnz d -2
    dec c
    jnz c -5

We can re-write this as (note: do/while is basically a loop we always process at least once):

    a = 1
    b = 1
    d = 26

    if c != 0

      c = 7

      do
        d = d + 1
        c = c - 1
      while c != 0
    #end
    
    do

      c = a
      
      do
        a = a + 1
        b = b - 1
      while b != 0
      
      b = c
      d = d - 1
      
    while d != 0

    c = 16
    do

      d = 12

      do
        a = a + 1
        d = d - 1
      while d != 0

      c = c - 1

    while c != 0

Now we can see alot of the loops are run a fixed number of times such as :

    c = 7

    do
      d = d + 1
      c = c - 1
    while c != 0

We know when the loop ends, d will be 7 bigger, and c will be 0, so let's substitute these.

    a = 1
    b = 1
    d = 26

    if c != 0
      d = d + 7
      c = 0
    #end
    
    do

      c = a
      
      do
        a = a + 1
        b = b - 1
      while b != 0
      
      b = c
      d = d - 1
      
    while d != 0

    c = 16
    do

      a = a + 12
      d = 0

      c = c - 1

    while c != 0

The remaining loop parts are either loops that aren't constant, or outer loops but we can manage those too:

    a = 1
    b = 1
    d = 26

    if c != 0
      d = d + 7
      c = 0
    #end
    
    do

      c = a
      a = a + b
      # b = 0 isn't necessary due to the next line
      b = c
      d = d - 1
      
    while d != 0

    c = 16
    a = a + (c * 12)
    d = 0
    c = 0

So we have one loop left, and if this isn't obvious now, this buffers a, adds b to a, sets b to the buffered a and repeats.  Or in other words a is added to it's previous value... smells suspciously Mathsy to me!

https://en.wikipedia.org/wiki/Fibonacci_number

So that's probably good enough for the native solution.  Problem is, this is translated from *my* input.  What I don't know is my input will differ from yours.  I have my suspcions that the initial value of d (26 or 26 + 7) would vary, and that what we set c to on the final loop will change (but the Fibonacci sequence itself will be unchanged), but without a larger data set size, we can't be overly confident this will scale to a general solution.... so what's the alternative.

The answer, throw all the hand analysis in the garbage, and try to optimise as best as possible the byte code interpretter with caching...

..... TO BE CONTINUED



**Part 2**


**Python vs C++ vs C**

**Python**


**C++**


**C**
