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

Let's indent all the loops:

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

So that's probably good enough for the native solution.  Problem is, this is translated from *my* input.  What I don't know is my input will differ from yours.  I have my suspicions that the initial value of d (26 or 26 + 7) would vary, and that what we set c to on the final loop will change (but the Fibonacci sequence itself will be unchanged), but without a larger data set size, we can't be overly confident this will scale to a general solution.... so what's the alternative.

The answer, throw all the hand analysis in the garbage, and try to optimise as best as possible the byte code interpretter with... patching, but first, let's discuss caching, and why I'm *not* taking that route.

So what is a caching interpreter?  In a nutshell, it's an interpreter that, once it processes byte code actively looks to see if there's any benefit in memorising the results of a block of code, and, if executed again, used any cached data it finds instead of processing the byte code as encountered.  In order to achieve this, we need to describe the execution of a block of code in such a way that:

1. Executing the cached block always returns the same results as the code it's now bypassing.
2. The act of caching/replaying isn't slower than just executing the byte code as is.

The problem with this really is that for the puzzle, the only loop that matters:

    do

      c = a
      
      do <------- This one right here!
        a = a + 1
        b = b - 1
      while b != 0
      
      b = c
      d = d - 1
      
    while d != 0

... is actually quite tricky to cache.   If we first look at a simpler loop:

    if c != 0

      c = 7

      do
        d = d + 1
        c = c - 1
      while c != 0
    #end

For this one, we can infer that we always set the following when we reach the line c = 7:

1. Set c to 0
2. Set d to d+7
3. Set the address to one opcode past the while loop.

Whereas for the other loop, this would require alot more processing to handle since the loop size can't just be naively ignored as it's not static, but instead will vary based on the (as yet) unknown value of b.  This is solveable, but in effect the more code we add to handle this, the slower the whole thing gets.  Infact, remember the easy loop I mentioned, problem is it's pointless to cache because it's only performed once.

Additionally, the presence of this inner loop would make caching the outer loop even trickier since we'd need to handle register b very carefully since it's key to the inner loop caching...

A much easier route would be code patching.  Look for obvious sequences of byte code, and replace them with a "better" series of instructions, and at this point by better I mean those that I've made up.

There's three sets of instructions that we want to replace, which are:

    inc d
    dec c
    jnz c -2

and

    inc a
    dec b
    jnz b -2

and

    inc a
    dec d
    jnz d -2

So it's a really simply pattern match, find any jnz {X} -2, make sure the loop index is dec'd, and that we're incrementing some other register, and then all we need to do is replace these with:

    add c d
    cpy 0 c
    nop

and

    add b a
    cpy 0 b
    nop

and

    add d a
    cpy 0 d
    nop

So we've two more instructions to add:

1. add src tgt
    - Adds the literal/register src value to the target register
3. nop
    - Does nothing, moves on to the next address.

The reason for nop is so that all remaining jnz's that I've not patched are correct.  I could have handled nop other ways, such as:

    cpy a a
    
But I like the explicitness of the NOP operator.  And there we have it, a simple pattern match that changes 9 opcodes, and we increase our execution time by 10x whilst remaining correct for the general case.


**Part 2**

All handled with the Part 1 mods!


**Python vs C++ vs C**

**Python**

Python's loose typing makes register/literal detection nice and simple.  I do wish Python's isnumeric would negative values, but it's trivial enough to work around.


**C++**

I'll take over-engineered for 1000 Alex.  In all fairness, this solution does a good job of separating out the optimisations from the original code, making it trivial to switch between the two, and it manages to do this without showing any evidence that it's impacted speed in any meaningful way, but let's face it, we could have achieved the same in far less code if we'd just done it procedurally rather than going OO.

The original version of this I wrote way back (that I basically scrapped) was originally C++ rather than Python and took 6 seconds to solve Part 2, I've not profiled why it was so much slower, however a quick glance of the deltas are:

1. My old version used string compares for the opcode, rather than enum's (meaning I couldn't implement a switch statement which would hopefully become a jump table).
2. My old version dynamically tested the operands to see if they were registers or not (string checks) when executing *every* opcode, not just on parsing the data on startup.
3. My old version didn't read the brief on there being 4 registers 'a' .. 'd', and instead just created them (if needed) every time it reached an operand that wasn't a number, again, all whilst processing the bytecode, not just on an initial parse.

So hopefully that shows it's not just a case of using C/C++ nets faster code compared to Python, in-fact Python without the code patching *still* ran faster than my old C++ implementation!


**C**

And back to simple, basically follows the Python solution, but the usual tricks, such as a wasteful register array just so I can directly index on the char, I've gone unsigned to avoid weirdness between compilers treating char as signed vs unsigned, albeit 'a' .. 'd' are all in the positive range for both anyway.

So here's a fun fact, the patching isn't strictly too important for C++/C.  On my system, both very fast solve times without patching the code, both I would consider fast enough to be good enough to post without to be honest (C did 64ms, C++ did 215ms).
