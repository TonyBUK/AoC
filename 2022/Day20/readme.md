# Day 20: Grove Positioning System

https://adventofcode.com/2022/day/20

So this one reminded me alot of Crab Cup: https://adventofcode.com/2020/day/23.  Where the puzzle itself is deceptively simple, but lives and dies by insertion/removal time of data sets.

**Part 1**

So the crux of this puzzle is starting from a large set of data of non-unique values (except for 0, which appears once), which can be positive or negative, we have to move each value (based on the original order) the number of places specified by the value.  Positive moves right, negative moves left.

So the idea is if we take their example, I'll interject with what would actually be done to an array:

    Initial arrangement:
    1, 2, -3, 3, -2, 0, 4

    1 moves between 2 and -3:
    Actions: Position 0 removed from Array
             All Positions shifted Left 1
             1 Inserted into Position 1
             Positions 2 onwards shifted Right 1
    Result : 2, 1, -3, 3, -2, 0, 4

    2 moves between -3 and 3:
    Actions: Position 0 removed from Array
             All Positions shifted Left 1
             2 Inserted into Position 2
             Positions 3 onwards shifted Right 1
    Result : 1, -3, 2, 3, -2, 0, 4

And so on.  What we end up with is quite a few memory shifts to accomodate this.  And on a sufficiently large data set (5000 items for the puzzle input), this ends up killing performance if done naively.

There is also the matter of keeping track of the original order, as we need to not only know where it got moves to, but where it originally came from.  And here's where the values being non-unique kicks in, as we can't just search for the values either.

So the reason I said this reminded me of Crab Cups is because that had the same issue, and a strong solution to this is Linked Lists.

If you've ever used a tree structure of any kind, you've used Linked Lists before.  The purpose of them is that each element *describes* where related elements exist.  In tree's it would be child nodes, maybe even the parent node, but for our purposes, each item in the Linked List just needs to keep track of who's to the Left, and who's to the Right.

This neatly also solves the circular buffer issue.

So if we descibe the example again as a linked list:

    Initial arrangement:
    1, 2, -3, 3, -2, 0, 4

    Element 0:
      Left Index : 6
      Right Index: 1
      Value      : 1

    Element 1:
      Left Index : 0
      Right Index: 2
      Value      : 2

    Element 2:
      Left Index : 1
      Right Index: 3
      Value      : -3

    Element 3:
      Left Index : 2
      Right Index: 4
      Value      : 3

    Element 4:
      Left Index : 3
      Right Index: 5
      Value      : -2

    Element 5:
      Left Index : 4
      Right Index: 6
      Value      : 0

    Element 6:
      Left Index : 5
      Right Index: 0
      Value      : 4

This may seem like an oddly verbose way to store them, but now consider what a removal/insertion looks like.

To remove an item, we just have to change the linked list values of it's neighbours.  So say we wanted to remove Element 0, we'd just end up having to change Elements 1 and 6 as follows:

    Element 0: (Removed)
      Left Index : 6
      Right Index: 1
      Value      : 1

    Element 1: (Right Neighbour)
      Left Index : 0 => 6
      Right Index: 2
      Value      : 2

    Element 6: (Left Neighbour)
      Left Index : 5
      Right Index: 0 => 1
      Value      : 4

In other words...

The Left Neighbour (Element 6) had it's Right Index set to the Right Index of the item we're removing (Element 0).

The Right Neighbour (Element 1) had it's Left Index set to the Left Index of the item we're removing (Element 0).

i.e. Element 1 is now the Right Neighbour of Element 6, and Element 6 is now the Left Neighbour of Element 1.

So 2 writes, and no memory shifting.

Similarly insertion.

    Element 1:
      Left Index : 6
      Right Index: 2 => 0
      Value      : 2

    Element 0:
      Left Index : 6 => 1
      Right Index: 1 => 2
      Value      : 1

    Element 2:
      Left Index : 1 => 0
      Right Index: 3
      Value      : -3

Element 0 had it's Left/Right Indexes set to the new Neighbours.

Element 1 had it's Right Index set to Element 0.
Element 2 had it's Left Index set to Element 0.

So insertion was 4 writes.

Making Insertion/Deletion 6 writes, no shifts, irrespective of the data size we're dealing with.

So why not use Linked Lists all the time?  Traversal.  Arrays have one massive advantage, say I want element 50, I could just do:

    Element 50 = Array[50]

But for a Linked List?

    Element 50 = Start Node
    FOR i = 0..50
      Element 50 = Element 50->Right Node
    NEXT

There's no quick way to get to a specific node entry, you have to traverse them one by one.

However, this trade-off, on a sufficiently large data set, is massively offset by the insertion/removal benefit.

One other neat aspect, well, the puzzle also asked us to process the data in the original order.  Well, we never touched the indexing of the Linked List, just the neighbours, so that part is trivial.

This makes the pseudo code:

    FOR each item in the sequence

      Get the Value

      IF Value != 0

        Iterate through the Neighbours in the Linked List based on the Value (+ve is right neighbours, -ve is left neighbours)

        Remove the current item in the sequence from its existing neighbours

        Set the new neighbour left/right indexes to point to the current element

        Set the current elements left/right indexes to point to the new neighbours

      NEXT

    NEXT

Now we can optimise this a bit on the Iteration through the Neighbours step.  If the Value exceeds the size of the sequence, it means we're looping multiple times, so we can apply the modulo of the sequence length..... minus one.  This is because when moving the item, we need to first *remove* ourselves.

Secondly, moving right 4999 locations in a 5000 element array is the same as moving left once.  So if the value is over half the size of the sequence, set it to the other direction by the remainder.

After this it's relatively trivial.  Starting at any neighbour you wish, find 0, then travel right 1000 times, grab the value, right 1000 more times, grab the value and right 1000 more times grabbing the last value.

You now have the Grove Co-ordinates.

**Part 2**

Here's where all that effort in Part 1 pays off.  We have a linked list that's quick to mix, but remains in the original order.  So all we need to do is scale the values by the decryption key, and run the whole thing 10 times.

Everything else is identical.

**Python vs C++ vs C**

**Python**

As per the write-up above.  Unfortunately, array lookups in Python are relatively expensive (range checking etc.), so the cost of traversal isn't great.

Now Python does seem to support a type that will do alot of this for you.

https://docs.python.org/3/library/collections.html#collections.deque

I've not personally used it, but it's worth investigating.

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

This is *absolutely* tailored made for a language like C.  Due to the utter lack of boundary checks etc., the traversal cost is orders of magnitudes cheaper than Python, causing the exact same algorithm to run in around 1/10th of the time.