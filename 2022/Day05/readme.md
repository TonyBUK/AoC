# Day 5: Supply Stacks

https://adventofcode.com/2022/day/5

This is a stacking puzzle where the essence of the puzzle is being able to move items from one array to another.  There are several key areas to this:

1. Making sure the initial states of the crates are correct.
2. Making sure the moves are parsed correctly.
3. Makig sure the crates are moved correctly, including not duplicating crates or removing too many.


**Part 1**

Most of this task will be common with Part 2, as the only delta is step #3, being the ordering of the crates.  The first step in the puzzle is reading the crates, which requires identifying where the blank line is in the file.

 Assuming you've used whichever language you're using to perform that, let's look at procesing the initial crate data.

        [D]    
    [N] [C]    
    [Z] [M] [P]
     1   2   3 

Now the easiest way to store this will be as follows:

    ARRAY[1] = Z, N
    ARRAY[2] = M, C, D
    ARRAY[3] = P

This means when storing the data, we're going to want to process the data in reverse order.  i.e.

    [Z] [M] [P]
    [N] [C]    
        [D]    

The reason is that otherwise, any time you want to add or remove anything from the array, you would have to shift all the other data.  i.e.

    ARRAY[1] = Z, N

is actually:

    Array    : Offset : Data
    =========:========:=====
    ARRAY[1] : 0      : Z
    ARRAY[1] : 1      : N

If we want to do delete N, we don't have to touch Z, but if we want to delete Z, we have to move N into the position Z was stored in.  This is essentially push/pop vs. insert/delete.  The former is far quicker.

So the core algorithm will be:

  FOR Rows in Reverse Order

    FOR each column

      IF the text occupying the current column is not a Space

        Append the text for the current column to the Crate Storage for that column

      END

    NEXT

  NEXT

You might be tempted to clean up the string and use some sort of inbuilt function like split, but consider the row:

        [D]    
     1   2   3 

That occupies column 2, but if you did some sort of split/tokenize operation, because there's nothing in columns 1 or 3, you'd invariably assign it to column 1 by mistake.

So with that in mind, hopefully you have an array with the right data.  It's actually good practice to have some sort of debug function to show you the data, preferably in a similar format to the original, such as:

    FOR each row

      FOR each column

        IF the size of the Crate for the current column is not smaller than the row

          Print the contents of the Current Column/Row

        ELSE

          Print a Space

        END

      NEXT

This would print out the crate contents, and let you compare it to your input data.

If this proves too tricky, given the stacks are actually quite a small part of the input, there's no shame in even hard coding it.  Just be aware this will somewhat limit how flexible your solution is.

Moving on, the next part of the task is to process a move command.  Most of this will be common with Part 2.

    FOR each Move

      Extract from the Move Command the: Number of Crates to Move, the Source Column, the Target Column

      FOR Current Crate Row = 0 .. (Number of Crates to Move - 1)

        Add to the Target Column the (Top Crate - Current Crate Row) from the Source Column

      NEXT

      Delete the Number of Crates to Move from the Source Column

    END

You can delete as you go, but doing it this way keeps more in common with Part 2.

The final step is to print the top crate from each column as your answer.  Some languages let you access the last element using negative indexes, such as Python, which would use -1 for the last element, -2 for the next and so on.  Otherwise, you can take the length of the array, subtract 1 (as most languages use 0 based indexing for the first element), and read that element.


**Part 2**

This is literally Part 1 with the following ammendment:

    FOR each Move

      Extract from the Move Command the: Number of Crates to Move, the Source Column, the Target Column

      FOR Current Crate Row = (Number of Crates to Move - 1) .. 0

        Add to the Target Column the (Top Crate - Current Crate Row) from the Source Column

      NEXT

      Delete the Number of Crates to Move from the Source Column

    END

Or alternatively:

    FOR each Move

      Extract from the Move Command the: Number of Crates to Move, the Source Column, the Target Column

      FOR Current Crate Row = 0 .. (Number of Crates to Move - 1)

        Add to the Target Column the (Top Crate - ((Number of Crates to Move - 1) - Current Crate Row) from the Source Column

      NEXT

      Delete the Number of Crates to Move from the Source Column

    END

The only difference in Part 1 and Part 2 is one processes the crates in the *opposite* order to the other.


**Python vs C++ vs C**

**Python**

With Python we can move the right number of crates using Array Slices.  This is also how we'll reduce the array sizes, and Python helpfully lets you use the addition operator to append one array to another.  As such, the only real difference between the parts is whether the crates are reversed or not before appending.

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

This really implements the algorithm as cited.  Because C doesn't support dynamic arrays, we have to pre-calculate what the biggest size might be, and also keep count as to how many crates are in each array.