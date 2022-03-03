# Day 23

https://adventofcode.com/2015/day/23

This goes alot easier than some of the later Day 23 puzzles, and is basically a staple, being the Virtual Machine.  I'd actually argue the Day 7 puzzle is trickier because it has dependencies that need resolving, whereas this is about a simple as a black box puzzle gets.

Later years do subvert this puzzle, by providing Byte Code that will take an absurdly long time to complete, requiring more aggressive approaches but that's not the case here, so no need to overcomplicate things.

**Part 1**

The core of any VM is effectively a Fetch => Decode => Execute cycle.  Making the main loop:

    WHILE Program Counter is valid for the Byte Code
    
      FETCH the Opcode at the Program Counter
      
      DECODE the Opcode at the Program Counter
      
      EXECUTE the Opcode at the Program Counter
      - Update the Address
      - Update (if required) the Registers
      
    END

Keeping in mind that the EXECUTE phase will also manipulate the Program Counter.  Most Opcodes will just set it to the next opcode, whereas some will set it to values commanded by the Byte Code.

For those interested, the Byte Code sequence can be expressed as:

    IF Register A != 1
    
      Initialise Register A to a starting value

    ELSE
    
      Initialise Register A to a more different starting value

    END

    WHILE Register A != 1

      Increment Register B
      
      IF Register A is Even
      
        Halve Register A

      ELSE

        Register A = (Register A * 3) + 1

      END

    END

Basically the WHILE loop can only complete when the act of multipling by 3, then adding 1, sets Register A to a power of 2 (i.e. a value that can be repeatedly halved until it has a final value of 1).

Or for anyone who spends any time googling the 3N + 1 problem, this is the Collatz Conjecture: https://en.wikipedia.org/wiki/Collatz_conjecture

That's really the nice thing about the puzzle, the fact it is performing a famous (in Mathemetical circles) algorithm isn't necessary to solve it.  But if you're willing to look deeper into the puzzle, it's possible to tease out things like this.

So because for Part 1 we initialise Register A to 0, we use the first set of starting data, which for my data set is 9663, which takes 184 iterations through the while loop before A = 1, therefore B = 184.

**Part 2**

So because for Part 2 we initialise Register A to 1, we use the more different set of starting data, which for my data set is 77671, which takes 231 iterations through the while loop before A = 1, therefore B = 231.

**Python vs C++ vs C**

**Python**

Very trivial implementation in Python, as this task is perfectly suited to the language.

**C++**

Oooh boy, did this ever go off into the deep end.  I figured rather than doing a straight port, I would demonstrate why I typically don't OO all the things.  This is effectively the Python solution, but alot more... classy.  Mocking aside, VM's are actually one of the stronger use cases for encapsulation into classes, especially if we need more than one VM running concurrently (the alternative is to store a bunch of meta-data about each VM's state externally which can get annoying).

In this case however, overkill, but I figured it'd at least be worth demonstrating that C++ is hardly a terse language, the only reason that it's mostly matched Python is the sheer amount of code STL hides (Strings/Vectors/Hash Maps), this is a small taste of that.

That aside, the basic class heirarchy is:

        Virtual Machine Class
        |
        |
        --> Operands Class
        |       |
        |       | (read only)
        |       v
        --> Register Class

The Virtual Machine stores the Byte Code as a list of Opcodes and associated Operands.  It also stores the Registers.  Where it gets a bit murky is that the Operands Class *also* has read only access to the Registers, in order to allow their Get method to return the value of the Register.

The cleaner approach from a hierarchy perspective would be that the Get method only works on literals, and to make the Virtual Machine do the heavy lifting to interrogate the Registers Class directly if the Operands Class declares itself as a Register, but to me that's just trading one layer of convolution for another...

**C**
