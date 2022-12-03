# Day 3: Rucksack Reorganization

https://adventofcode.com/2022/day/3

This puzzle provides a series of items, which are designated by letters (upper and lower case), providing 52 possible items in total.  The task is essentially finding duplicates.

**Part 1**

For Part 1, we're looking for duplicates in the same backpack.  The backpack has two compartments, and this is represented by providing a single line with a series of characters, where the number of characters is always even.  The first half of the characters belongs to the first compartment, the second half belongs to the second compartment.

i.e.

        vJrwpWtwJgWrhcsFMMfFFhFp

can be split as

        vJrwpWtwJgWr
        hcsFMMfFFhFp

And now need to find any/all letters in the first set of letters that have a duplicate in the second.  In this instance, it's the lower case p.

Note: You might notice the second compartment has multiple duplicates within it, such as h, F, M, but these are not present in the first compartment, so we're not worried about them.

This process needs to be repeated for each backpack.  Then all that remains is to add up all the letters.  This is done by treating a..z as 1..26, and A..Z as 27..52.

Note: For the purposes of this puzzle, it appears as though each backpack will always have one, and only one duplicate.

This means to solve this, we just need to do the following:

        Set the Total Count to 0

        FOR each line in the file

                Read the first half of the line and store as compartment 1

                Read the second half of the line and store as compartment 2

                Find all letters in compartment 1 that exist in compartment 2

                Convert the matching letter(s) to their numerical values

                Add the numerical values to the Total Count

        #END

And the manual algorithm to check for matching items is:

        FOR each letter in compartment 1

                FOR each letter in compartment 2

                        IF the compartment 1 letter matches the compartment 2 letter

                                Add the letter to the list of duplicates

                        NEXT

                NEXT

        NEXT

But there are other ways that are more tied to the features of the language you're using.

**Part 2**

This isn't *all* that different to Part 1.  The main difference is that instead of taking a single line, splitting it in two, and looking for duplicates, we read three lines at a time, and looking for duplicates that exist in all three.

Note: Again we can assume only one.

The code would be something like:

        Total Count = 0

        WHILE all lines haven't been read

                Read a line and store as backpack 1
                Read a line and store as backpack 2
                Read a line and store as backpack 3
        
                FOR each character in backpack 1

                        FOR each character in backpack 2

                                IF backpack 1's character = backpack 2's character

                                        FOR each character in backpack 3
                                        
                                                IF backpack's 1 character = backpack 3's character

                                                        Add the letter to the list of duplicates

                                                END
                                        
                                        NEXT

                                END

                        NEXT

                NEXT

                Convert the matching letter(s) to their numerical values

                Add the numerical values to the Total Count
        
        WEND

*Phew*... 


**Python**

So Python trivialises this with a set type.  A set type is like a list, except it's unordered, and all values have to be unique.  But what makes it perfect is that it supports combining sets together in different ways that ends up being perfect for this task.  For instance Part 1... instead of all that nonsense to check for duplicates, all we need to do is:

        Store Compartment 1 as a set

        Store Compartment 2 as a set

        Duplicates = Compartment 1 & Compartment 2

That's it!  By AND'ing the two sets together, it only keeps values that are common to both.  And this means Part 2 becomes:

        Read a line and store as backpack 1 set
        Read a line and store as backpack 2 set
        Read a line and store as backpack 3 set

        duplicatres = backpack 1 & backpack 2 & backpack 3

That's it!

The Python version also leverages the zip function, which is an *extremely* powerful way of iterating simultaneously on multiple things at a time.  It also expands array slicing to add steps, specifically:

        kBadges = [(kSet1 & kSet2 & kSet3).pop() for kSet1,kSet2,kSet3 in zip(kItemsAsSets[::3], kItemsAsSets[1::3], kItemsAsSets[2::3])]

Which is equivalent to:

        for kSet1, kSet2, kSet 3 in zip(kItemsAsSets[0::3], kItemsAsSets[1::3], kItemsAsSets[2::3])

                kBadges.append(kSet1 & kSet2 & kSet3)

        #end

But what about the zip function itself?  Well, it's got 3 inputs, meaning it's going to iterate over three things at the same time, but where it gets interesting is the array slicing, specifically, [0::3], [1::3], [2::3].  What this is essentially doing is saying: starting at element 0, 1 and 2 respectively, iterate to the end of the list, and only iterate every 3 items.  The more manual way of doing this would be:

        for index in range(0, len(kItemsAsSets), 3) :
                kSet1 = kItemsAsSets[index + 0]
                kSet2 = kItemsAsSets[index + 1]
                kSet3 = kItemsAsSets[index + 2]
                kBadges.append(kSet1 & kSet2 & kSet3)
        #end

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

This works based on the suggested algorithm in the main body, as we'd have to write all the code to handle sets if we wanted that functionality...