# AoC
**Advent of Code Solutions**

The purpose of this repo is to provide a tutorial of sorts for each and every puzzle.  Initially I'll just be supplying Python, C and C++ solutions but more may be added in time.

The core idea is to not only show at least one way of solving each given problem, but also the trade-offs of each given language in doing this.

**Note:** This isn't going to try and evangelise one language over another.  More to show each languages strengths and weaknesses, as well as some idea of feature/performance trade-offs.  If you want the TL/DR version, it will generally be:
- Python : A very feature rich language, with some very powerful collection types that helps reduce just how much code is needed, but at the expense of speed.
- C++    : A feature rich low level language, especially when paired with STL, which can often maintain near parity with Python in terms of features available out of the box.
- C      : A very low level language that doesn't have out of the box parity with Python/C++ especially when it comes to dynamic arrays / hash maps, instead requiring either custom code or third party code.  This does however come with a much lower/more obvious memory footprint, and typically, when written correctly, should produce the quickest code.  But it does often come at the expense of requiring much more user written code to achieve feature parity...

Also as a trigger warning, I *despise* Python's indentation scoping rule, so get used to seeing "#end" all over the place to make me at least think it bookends scope.

It's going to take a while to fully populate this, as whilst I have solved all (as of writing) 350 stars, I do want to take some time on each to re-factor.  Also, the majority of the code is in Python, so some effort is going to be needed (post cleanup) to port to C and C++.

One thing to be wary of, the solutions I've uploaded work for my particular environment with my inputs.  This may not be true for you.  I'll be interested to see if there's any issues along the lines of:
- My input works for you, but yours does not, which implies my solution is incomplete.
- My input doesn't work for you, which implies my solution is not portable.
