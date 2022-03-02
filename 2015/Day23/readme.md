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

So because for Part 1 we initialise Register A to 0, we use the first set of starting data, which for my data set is 9663, which takes 184 iterations through the while loop before A = 1, therefore B = 184.

**Part 2**

So because for Part 2 we initialise Register A to 1, we use the more different set of starting data, which for my data set is 77671, which takes 231 iterations through the while loop before A = 1, therefore B = 231.

**Python vs C++ vs C**

**Python**

Very trivial implementation in Python, as this task is perfectly suited to the language.

**C++**

**C**
