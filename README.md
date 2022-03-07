# AoC (Unofficial)
**Advent of Code Solutions**

https://adventofcode.com

The purpose of this repo is to provide a personal tutorial of sorts for each and every puzzle.  Initially I'll just be supplying Python, C and C++ solutions but more may be added in time.  Hopefully this is obvious but to explicitly spell it out, this repo is in no way officially representitive of the AoC challenge, it's merely my solutions and musings on each challenge as a long term pet project.

The core idea is to not only show at least one way of solving each given problem, but also the trade-offs of each given language in doing this.

**Note:** This isn't going to try and evangelise one language over another.  More to show each languages strengths and weaknesses, as well as some idea of feature/performance trade-offs.  If you want the TL/DR version, it will generally be:
- Python : A very feature rich language, with some very powerful collection types that helps reduce just how much code is needed, but at the expense of speed.
- C++    : A feature rich low level language, especially when paired with STL, which can often maintain near parity with Python in terms of features available out of the box, but with a much lower overhead with respect to speed.
- C      : A very low level language that doesn't have out of the box parity with Python/C++ especially when it comes to dynamic arrays / hash maps, instead requiring either custom code or third party code.  This does however come with a much lower/more obvious memory footprint, and typically, when written correctly, should produce the quickest code.  But it does often come at the expense of requiring much more user written code to achieve feature parity...

This isn't meant to be a "best" solution either, merely the solution I arrived at (typically as long as the solution is measurable in milliseconds or seconds, that's good enough for me).  Typically when you see solutions, they fall into one of two camps:

- Algorithmic solutions
- Mathematical solutions

The latter tend to be faster, but are often only useful if you understand the core principles behind it.  As a simple example, consider writing a general solution to: N + (N-1) + (N-2) + ... + 5 + 4 + 3 + 2 + 1 (hint, this will actually come up!)

If you were writing this algorithmically you would go with something like:

    TOTAL = 0
    WHILE N > 0
      TOTAL = TOTAL + N
      N     = N - 1
    WEND

So you would understand that as N gets larger, the amount of time required to reach a solution also increases (in Big O notation, this would be O(N), i.e. a linear increase as the size of the input increases).

The mathematical approach would be recognising the pattern as a series, specifically a Triangle Number: https://en.wikipedia.org/wiki/Triangular_number#Formula

Where you could replace the *whole* thing with:

    TOTAL = (N * (N+1)) / 2

i.e. the value of N is no longer important, it will always take the same amount of time to solve (in Big O notation, this would be O(1), i.e. the time to process remains constant).  If it doesn't make sense initially looking at it, I'd recommend seeing how it relates to the rectangle of green/brown dots on the Wikipedia page, as this is a great visual representation of what the algorithm is actually doing and why it works for a Triangle Number.

Personally I tend to bias towards the Algorithmic solutions *unless* the Mathematical one is relatively common or if I've been forced down that route due to the Algorithmic approach being impractical.  As a bench mark for me, yes, I'll typically spot applications of triangle numbers, no, I won't typically spot applications of Chinese Remainder Theorem.

If I'm aware that there was a super maths'y solution to a puzzle that I missed, I'll link it for each day I'm aware of, but this won't be exhaustive, and will typically only be for 2020/2021 since that's when I started actively participating in AoC, whereas 2015-2019 was me solving these way after the fact, meaning I wasn't *as* interested in other peoples solutions.

If you're looking for different approaches for any given solution, I would recommend either:
- Going to: https://www.reddit.com/r/adventofcode/
- Or more likely googling: Advent of Code [Day] [Year]
  - And then visiting the Reddit thread this brings up.  This also has the benefit of providing a huge range of solutions, especially in the "overachieving" category, i.e. fastest solution, smallest solution, solution written in the most esoteric language and so on, and of course you can always ask questions!

It's going to take a while to fully populate this, as whilst I have solved all (as of writing) 350 stars, I do want to take some time on each to re-factor.  Also, the majority of the code is in Python/Ruby/Other, so some effort is going to be needed (post cleanup) to port to C and C++.

Also not everything will be a straight port.  What I'm going for is writing the solutions for each challenge that is the most natural way (to me) of implementing a solution for the language at hand.  This will mean Python/C++ will mostly follow each other, but occasionally C will diverge.  That's not to say the C methods couldn't be used in Python/C++, more that it'd be weird to go that route.  As I said, I'm certainly not trying to bias any language, but likewise I don't want to write unrealistic implementations for the sake of feature parity/lowest common denominator coding across languages.

One thing to be wary of, the solutions I've uploaded work for my particular environment with my inputs.  This may not be true for you.  I'll be interested to see if there's any issues along the lines of:
- My input works for you, but yours does not, which implies my solution is incomplete.
- My input doesn't work for you, which implies my solution is not portable.

**General comments on Python**

The Python version targetted is in the 3.x range.  The aim isn't to constantly refresh to the latest and greatest, but acknowledgement that the fragmentation issues with Python versions utterly pales to that of C/C++ compiler versions (see below).  If anyone does find compatability issues with my Python code and some widely used variant of Python 3.X, I'd be happy to take a look to try and resolve it.  For example, how's MicroPython compatability?

I'm also limiting myself to the standard libraries only.  I appreciate Numpy etc. all exist and are fairly ubiquitous, but ultimately it's typically easier to take code written using standard libraries and rewriting those to use specialised libraries than the other way around, plus there may be times when installing libraries isn't feasible for others, so I want something that, provinding you can install/run Python 3, will just work out of the box.

Also as a trigger warning, I *despise* Python's indentation scoping rule, so get used to seeing "#end" all over the place to make me at least think it bookends scope.  Also my background is more procedural programming than object orientation, so my code will reflect that (my day job is typically embedded C).  Typically I only use classes to encapsulate a thing, especially if I think it would benefit from RAII/resource ownership, but my go-to isn't make all things classes.

**General comments on C/C++**

For better or worse, the standards of each language I'm targetting are C '89/'90 and C++ '98.  The main reason is this gives the greatest portability for anyone who wants to build the code.  My day job primarily involves writing low level C code for embedded platforms using certified compilers, this typically forces me to the oldest standard of the compiler due to the prohibitive cost of certifying compilers for various uses.  Part of the aim of these solutions is to provide portable solutions with minimal dependencies, and unfortunately that typically means some newer language features are off limits.

If a more recent version of the language provides a significantly better solution, I may provide that as well, however if it just means I can't use auto (actually painful for iterators), or prevents me from scoping variables to their actual usage, then that's something I can live with, since if someone does wish to "upgrade" the C/C++ version, it's a relatively trivial process to do so.

And again, trigger warning time, I like my braces to line up horizontally, meaning, I always put my open braces on the next line, meaning you'll *never* see:

        if (am I a bad person) {
            Yes!
        }

But you will see:

        if (am I being deliberately provocative)
        {
            Who knows!
        }

Plus unless it's a hyper specific case, such as a whole bunch of return tests or something of that ilk, I always use braces, even if it's only to guard a single line of code.  I'm not a fan of potential ambiguity for the sake of terseness, but there are always exceptions...

Also typically I'll write the solution in Python first, then re-write the solution in C++ (especially as Python/C++ with STL offer very similar features in termsof basic data structures), then re-write the solution in C.  Often by the time it gets to C, much of the abstractions that make Python/C++ somewhat easier to work with for this sort of task will have been removed.  I'm acutely aware that I can go back and optimise Python/C++ accordingly, but what I'm trying to showcase is (hopefully) decent examples of how someone would write a solution in a given language, and often that means discarding repeated optimisation passes for the sake of clarity.

***General comments on inputs***

Advent of Code typically provides an input text file, but very occasionally if the input is simple, such as a single number or string, bakes the value into the Web Page itself.  For both of these types, I store this as an input text file, "input.txt".  It is perfectly valid to bake this into the source code, and even hand parse/macro the data, but personally I prefer the text file solution, especially as, on the harder tasks, my work flow is typically:

1. Save the examples to a text file.
2. Work on the core algorithms until they solve the examples in a way that matches the Advent of Code instructions.
3. Switch the examples text file to the true input, and repeat.

Essentially it's easier to debug if you already know the answer, plus most of the time, this works for getting a "good enough" solution, albeit that's not always the case, especially if the examples don't expose the true nature of the puzzle at hand.

Very occasionally, some puzzles have common data that needs embedding.  Mostly just starting values, iteration counts etc., but occasionally it's a bit more involved, such as the Weapon Stats from: https://adventofcode.com/2015/day/21

In this scenario, because the data *never* changes across different users, I normally bake this directly into the source code.

Incidently, the examples text file is also why alot of the time, say on single line inputs, I'll still loop assuming it's multiple lines, as my initial work is on the examples, I genuinely am running a bunch of different inputs in one go.  It effectively provides a nice easy way to verify a change I've done hasn't broken things before diving into the true input.

***General Comments On Solutions***

Obviously I've no idea who's going to read any of this, but my assumption is that if you end up reading any of the documentation associated with the earlier puzzles, you're probably still finding your feet with respect to algorithms / code etc. in general, so I tend to pitch those accordingly.  If you're stuck on one of the later ones, you probably are more interested in the core algorithm itself for the solution, and not so much on language features etc., so those are pitched accordingly too.

What I'm particuarly interested to hear is whether the blurb with the solutions helped or hindered, so that I can refine as needed.  Whilst the code is going to be the best reference at the end of the day, I also like the idea that someone might just read the accompanying text, and figure the rest out for themselves from there.

***About Me***

I'm a pretty experienced S/W Engineer specialising in embedded C, typically written as close to bare metal as possible.  I've also worked extensively with C++/Ada, and have dabbled in x86/Z80/68K/PPC/Arm assembly, C#, Python, Java, Javascript, PHP, Pascal, Jovial, Delphi, Lua and Ruby and many more.  My main motivation for taking part in AoC was two-fold, firstly I wanted to get more experience with Python/Ruby, in that I found myself typically brute forcing C/C++ style solutions into these languages rather than taking advantage of the unique features of the languages.  As an example (for Python), my original solutions didn't contain any list/dictionary/generator comprehension whatsoever.

This also tends to scratch more of an academia style itch with regards to the problem types, plus it's not that often that I get to see such a diverse range of solutions to a given problem when comparing what I came up with to the wider community.  Sometimes it's nice to be validated when my solution aligns, and sometimes it's nice to see what I missed (or on the very very rare occasions, what everyone else missed) when it doesn't.

If I do add more languages, I'd probably want to select one that has some fundamental differences, for example, whilst I like Ruby, and in many ways prefer it to Python, I feel the solutions would be too similar to Python to warrant showcasing.  I'm probably more inclined to pick something like Rust, albeit let's get Python/C++/C out of the way first!

I do want to stress, I've no interest in any deliberately esoteric languages, so that's a hard pass on Brain-f... etc.  If I feel like I'm just fighting the language all the way, that's not even a remotely fun experience for me.

***My Development Environment***

All the code is written using Visual Code on my Laptop which is a 2012 Macbook Pro with a massive thermal throttling issue, so if I've deemed the speed of a solution acceptible on my machine, you'll be just fine...  This is why I don't have any make files etc., as Visual Code basically just does all that for me on the fly.  If there's a big demand, I can easily add cmake files, but as my builds consist of (typically) single self-contained source files, or at most one other source file if I'm pulling in some external library, it's not the trickiest thing to resolve.

I have very deliberately tried to shy away from libraries/functions I know to be OS specific, one of the reasons there's no timing code in C/C++.

Compiler wise, C/C++ is clang for me.

I am planning on upgrading my laptop at some point this year, probably to an Arm based Laptop (M1/M2 probably), so I'll of course be re-validating all of my solutions to ensure they still work.
