# Day 1: Calorie Counting

https://adventofcode.com/2022/day/1

This challenge is one that once you get over the initial hurdle of reading the input data and collating it, isn't too tricky.  The input data is provided as a follows:

    1000
    2000
    3000

    4000

    5000
    6000

    7000
    8000
    9000

    10000

The idea is that we want to group all of the data inbetween the blank lines together, such that we have:

Elf 1: 1000, 2000, 3000
Elf 2: 4000
Elf 3: 5000, 6000
Elf 4: 7000, 8000, 9000
Elf 5: 10000

The basic implementation of this is going to be something like:

        Create a List of Elves, with only one Elf

        WHILE there's more to read from the file...

            Read Line from File

            If it's not blank, add the number to the current Elfs list

            Otherwise, create the next Elf

        WEND

Now we can go one step further, because both parts of the puzzle only care about the sum of all the numbers associated with each Elf, i.e.

Elf 1: 1000 + 2000 + 3000 = 6000
Elf 2: 4000               = 4000
Elf 3: 5000 + 6000        = 11000
Elf 4: 7000 + 8000 + 9000 = 24000
Elf 5: 10000              = 10000

Once we've done that, we can get to solving each part.

**Part 1**

This asks which Elf is carrying the most calories.  To solve this, we basically need to do one of two things...  Either manually compare each item, and find the biggest, or sort them, and pick the item at the appropriate end of the list.

To do this via comparison would be something like:

        Most Calories = 0 <- This should just force the first Elf comparison to always trigger updating the Most Calories

        FOR Each Elf...

            IF the current Elf's total calorie count is bigger than the current Most Calories
            THEN

                Most Calories = Current Elf's total Calorie Count

            END

        NEXT

By the end of this, the Most Calories should match the highest calorie count.

The alternative is to sort these (most languages have built in sort functions), whereby if we use the example data:

    Elf Calorie Count = [6000, 4000, 11000, 24000, 10000]

Assuming we're sorting highest to lowest, we would get:

    Elf Calorie Count = [24000, 11000, 10000, 6000, 4000]

So we know that the first elf now always has the most calories.

**Part 2**

Rather than finding just the first, we now need the top three.  Now we could do the comparison loop, and store the three highest, but as you might notice, the sorting version did the hard work for us, in that rather instead of just using the first Elfs

**Python vs C++ vs C**

**Python**

In terms of Python staples, we're using:

- Lists : These are also known as arrays, and are essentially a list of things that we can access in a predictable order.

So in terms of how I use this...

- Lists are used to store all the individual meals each Elf is holding, making it a list of lists.

Plus this makes use of:
- List comprehension
  - Which allows an action to be used on anything iterable (such as another list) to generate a new list
    - For example, converting our input file into something we can use

Python neatly trivialises this task, in that the split operation does all the hard work for blank line detection etc.  To go through the main reading loop and process it on the example data, we have:

    inputFile.read().split("\n\n")

Note: \n is an artefact from C, and just means New Line.  If you want to read more: https://en.wikipedia.org/wiki/Escape_sequences_in_C

This will transform:

    1000
    2000
    3000

    4000

    5000
    6000

    7000
    8000
    9000

    10000

into:

    [1000\n2000\n3000], [4000], [5000\n6000], [7000\n8000\n9000], [10000]

And the full line:

    for line in inputFile.read().split("\n\n") :

Will return one of these 5 items in the list at a time into a variable called line.  i.e. the internal part of the FOR loop will execute 5 times:

    Iteration 1: line = [1000\n2000\n3000]
    Iteration 2: line = [4000]
    Iteration 3: line = [5000\n6000]
    Iteration 4: line = [7000\n8000\n9000]
    Iteration 5: line = [10000]

But now we need to handle those pesky remaining \n's, which is where line.split("\n") comes into play.

    for calorie in line.split("\n")

Which will act on the contents of line.  So if we just use the instance of: line = [1000\n2000\n3000]
We get...

    Iteration 1: calorie = 1000
    Iteration 2: calorie = 2000
    Iteration 3: calorie = 3000

So to bring it all together...

    elves = []

    for line in inputFile.read().split("\n\n") :

        elves.append([])

        for calorie in line.split("\n") :

            elves[-1].append(int(calorie))

        #end

    #end

The final part of this puzzle is the list handling.

    elves = [] <- This is creating a blank list

    elves.append([]) <- This is creating a new elf, but one that is holding nothing.

    elves[-1].append(int(calorie)) <- This is adding the calorie value to the last elf we created.

Note: You might notice that I have int(calorie) rather than just calorie.  This is because:

    Iteration 1: calorie = 1000
    Iteration 2: calorie = 2000
    Iteration 3: calorie = 3000

is *actually*...

    Iteration 1: calorie = "1000"
    Iteration 2: calorie = "2000"
    Iteration 3: calorie = "3000"

Making it a string, not a number.  The int() function converts the numeric string into an actual number.

This can all be expressed as a one liner using list comprehension:

    elves = [[int(calorie) for calorie in line.split("\n")] for line in inputFile.read().split("\n\n")]

What you might notice is it contains pretty much identical text to the FOR loop above, but *without* any of the .append statements.

So now we should have:

    elves = [[1000, 2000, 3000], [4000], [5000, 6000], [7000, 8000, 9000], [10000]]

Next job is adding them all together...

    meals = []

    for calories in elves :
        meals.append(sum(calories))
    #end

    meals = sorted(meals, reverse=True)

So now we should have:

    meals = [24000, 11000, 10000, 6000, 4000]

This introduces the sum function, which returns the total of a list (well, of anything iterable), and the sorted function is what puts everything in order.  The reverse statement is so that we go *highest* to *lowest*.

Part 1 just needs the first item in the list, which is element 0.  i.e. meals[0]
Part 2 needs the sum of the top 3.  We can get this by doing an array slice.  i.e. meals[:3] (or meals[0:3] which is equivalent), which would return [24000, 11000, 10000].  We can then add all these up, meaning the answer is: sum(meals[:3])

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

This does away most of the array handling or anything of that nature.  Instead we can simplify this algorithm to basically:

    Top 3 Calories = [0, 0, 0]

    Current Total Meal Calories = 0

    FOR each line in the file

      IF the line isn't blank
      THEN

        Add the Calories to the Current Total Meal Calories

      ELSE

        IF the Current Total Meal Calories > The 3rd Lowest of the Top 3 Calories
        THEN

          Set the 3rd Lowest of the Top 3 Calories to the Current Total Meal Calories

          Sort the Top 3 Calories

        END

        Current Total Meal Calories = 0

      END

    NEXT
  
Of course as we're dealing with C, we do now also need to handle alot of the string conversions manually.  There are default functions to do this, however I wrote this to emphasise a low memory footprint with no dynamic memory allocations, hence it essentially converts numbers as follows:

        Number = 0
        FOR each Digit
            Number = (Number * 10) + int(Digit)
        NEXT

What's actually interesting is throughput wise, this is at parity with Python, if not a little slower.  This is due to the constant re-sorting, whereas Python only does this once at the end.  This *could* work that way, but we'd need to either hard code or *guess* the size at the beginning.  This is somewhere that C++ would provide a best of both worlds solution.