# Day 8

https://adventofcode.com/2015/day/8

This introduces:
- More advanced string handling

This one can be almost as trivial as the MD5 puzzle with the right default libraries, in that escaping/unescaping is a relatively standard operation.  However that goes against the spirit of the task so I've gone with a verbose solution across the board.

This puzzle also bucks the typical trend of AoC with Part 2 being arguably simpler than Part 1, in the Part 2 just requires appropriate insertions of the \ character, whereas Part 1 requires not only stripping of these, but also decoding the hex notation \xFF format.

**Part 1**

Nothing overly special, iterate over the string until a \ is found, skip it and either, and either:
- Decode the Hex Value in the event that the next character is an x.
- Just print the next character as-is otherwise.

The only minor convoulation is removing the inner/outer quoute.

**Part 2**

This is arguably much simpler, and time a \ or a " is encountered, insert a \ character before.  Then place quotes around the entire string.

**Python vs C++ vs C**

For once Python actually had something of a disadvantage here.  The default Python iterators aren't really sympathetic to skipping multiple cycles, meaning a while loop was the easiest way.  I could have declared an iterator, but even then to skip two cycles requires: next(next(iter)), which just feels icky.

C++ follows the Python approach pretty closely, but shows off the more versatile loops, in that I can skip multiple cycles without any convolutions.

C wasn't disadvantaged for once, aside from the usual convolutions with regards to memory allocation / buffer tokenization, the only other slight quirk was that the output buffers from encoded/decoded data had to be predicted, with some noddy testing to see if it needs resizing.  The throughput could be improved slightly if an aggressively large buffer was allocated up front, since this would prevent multiple re-allocations.

One slight, but legal bodge is C isn't overly sympathetic to appending chars, as most string operations expect to deal with strings.  The bodge to append a char (i.e. a single character without a NUL terminator) was to use strncat with a num field of 1, which neatly means the NUL terminator will never be needed for the string being appended.  Otherwise it'd either be buffering the data into a NUL terminated string first or some hot garbage like:

kString[strlen(kString)    ] = '\0';
kString[strlen(kString) - 1] = C;

Blech...

**Times**

    Language : Time      : Percentage of Best Time
    =========:===========:=========================
    Python   : 14.1ms    : 960.30%
    C++      : 1.47ms    : 100.00%
    C        : 1.62ms    : 110.71%
