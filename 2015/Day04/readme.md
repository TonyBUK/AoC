# Day 4

https://adventofcode.com/2015/day/4

This introduces:
- Google!
- External libraries.
- Portability.

In some ways, this is almost the easiest puzzle, but also the most brute force-y.

**Part 1**

Using a given string as a starting point, append to that string a number from 1 onwards, and keep going until the first 5 characters (nibbles) of the MD5 filehash are all 0's.

**Part 2**

As per Part 1, but keep going until the first 6 characters (nibbles) of the MD5 filehash are all 0's.

**Python vs C++ vs C**

Aside from figuring out how to combine a text string with a number, the main thing of concern is how do you calculate an MD5 filehash?  Now if you know your standards bodies / algorithms, you might think to google: "md5 rfc" which would get you:

https://datatracker.ietf.org/doc/html/rfc1321

Where RFC is just "Request For Comments", where people post a submission to the Internet Engineering Task Force, and submit to a peer review.  Embedded within is a reference implementation of the MD5 algorithm, that you may just find elsewhere.

More likely though, you're going to google "md5 python" or "md5 c++" or "md5 c", in which case you'd probably end up on one of the following:
- https://stackoverflow.com/questions/5297448/how-to-get-md5-sum-of-a-string-using-python (Python)
- http://www.zedwood.com/article/cpp-md5-function (C++)
- https://people.csail.mit.edu/rivest/Md5.c (C)

We can see from Python, 2 lines of code get you up and running.  Same with C++, C is significantly more.  It's also worth noting that the C/C++ code is actually the RFC code, one with a C++ wrapper and one in pure C.  But here's where we get to porting issues... The code-as is doesn't work on my setup.  The core issue is the line:

typedef unsigned long int UINT4;

This is meant to create a 4 byte unsigned type, but on my setup, Intel I7 running OSX Catalina using clang, this actually creates an 8 byte unsigned type.  This is broken on both the C/C++ version on my setup and is actually one of the core issues you'll invariably run into when downloading source code from the web, as some of it will, even if unknowingly, be reliant upon some sort of platform/processor specific architecture, such as data sizes, endianness etc.  The fix was relatively simple, include <stdint.h> and use uint32_t.  Whereby someone else has gone through the effort of creating size appropriate types for a given platform/architecture.

Otherwise for the most part they're very similar.  Python/C++ both return strings, which simplifies the comparison but results in slower execution type since 5/6 characters corresponds to 5/6 byte compares.  The C implementation returns a byte array, meaning it's a 2.5 / 3 byte comparison since I'm comparing nibbles.  There's no reason the C++/Python versions couldn't also do the same thing, but it's something to keep in mind when it comes to performance.

It's also noteworthy that C/C++ could have also used to OpenSSL library, which was available to me, but I went this route instead since it's not guarenteed this will be available to you in whatever environment you're using, whereas the source code solution does at least make reduce the dependencies.

**Times**

  Language : Time      : Percentage of Best Time
  =========:===========:=========================
  Python   : 6.7265s   : 100.00%
  C++      : 14.2467s  : 211.80%
  C        : 8.9093s   : 132.45%
