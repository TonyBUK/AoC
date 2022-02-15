# AoC
**Advent of Code Solutions**

The purpose of this repo is to provide a tutorial of sorts for each and every puzzle.  Initially I'll just be supplying Python, C and C++ solutions but more may be added in time.

The core idea is to not only show at least one way of solving each given problem, but also the trade-offs of each given language in doing this.

**Note:** This isn't going to try and evangelise one language over another.  More to show each languages strengths and weaknesses, as well as some idea of feature/performance trade-offs.  If you want the TL/DR version, it will generally be:
- Python : A very feature rich language, with some very powerful collection types that helps reduce just how much code is needed, but at the expense of speed.
- C++    : A feature rich low level language, especially when paired with STL, which can often maintain near parity with Python in terms of features available out of the box.
- C      : A very low level language that doesn't have out of the box parity with Python/C++ especially when it comes to dynamic arrays / hash maps, instead requiring either custom code or third party code.  This does however come with a much lower/more obvious memory footprint, and typically, when written correctly, should produce the quickest code.  But it does often come at the expense of requiring much more user written code to achieve feature parity...

Also as a trigger warning, I *despise* Python's indentation scoping rule, so get used to seeing "#end" all over the place to make me at least think it bookends scope.  Also my background is more procedural programming than object orientation, so my code will reflect that (my day job is typically embedded C).  Typically I only use classes to encapsulate a thing, especially if I think it would benefit from RAII/resource ownership, but my go-to isn't make all things classes.

It's going to take a while to fully populate this, as whilst I have solved all (as of writing) 350 stars, I do want to take some time on each to re-factor.  Also, the majority of the code is in Python/Ruby/Other, so some effort is going to be needed (post cleanup) to port to C and C++.

Also not everything will be a straight port.  What I'm going for is writing the solutions for each challenge that is the most natural way (to me) of implementing a solution for the language at hand.  This will mean Python/C++ will mostly follow each other, but occasionally C will diverge.  That's not to say the C methods couldn't be used in Python/C++, more that it'd be weird to go that route.  As I said, I'm certainly not trying to bias any language, but likewise I don't want to write unrealistic implementations for the sake of feature parity/lowest common denominator coding across languages.

One thing to be wary of, the solutions I've uploaded work for my particular environment with my inputs.  This may not be true for you.  I'll be interested to see if there's any issues along the lines of:
- My input works for you, but yours does not, which implies my solution is incomplete.
- My input doesn't work for you, which implies my solution is not portable.

**General comments on Python**

The Python version targetted is in the 3.x range.  The aim isn't to constantly refresh to the latest and greatest, but acknowledgement that the fragmentation issues with Python versions utterly pales to that of C/C++ compiler versions (see below).  If anyone does find compatability issues with my Python code and some widely used variant of Python 3.X, I'd be happy to take a look to try and resolve it.  For example, how's MicroPython compatability?

**General comments on C/C++**

For better or worse, the standards of each language I'm targetting are C '89/'90 and C++ '98.  The main reason is this gives the greatest portability for anyone who wants to build the code.  My day job primarily involves writing low level C code for embedded platforms using certified compilers, this typically forces me to the oldest standard of the compiler due to the prohibitive cost of certifying compilers for various uses.  Part of the aim of these solutions is to provide portable solutions with minimal dependencies, and unfortunately that typically means some newer language features are off limits.

If a more recent version of the language provides a significantly better solution, I may provide that as well, however if it just means I can't use auto (actually painful for iterators), or prevents me from scoping variables to their actual usage, then that's something I can live with, since if someone does wish to "upgrade" the C/C++ version, it's a relatively trivial process to do so.
