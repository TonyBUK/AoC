# Day 19: Not Enough Minerals

https://adventofcode.com/2022/day/19

Permutation day... this puzzle can be remarkably fast, or horribly slow, it's all down to how you can keep track of permutations.  Also fun fact, Part 2 is the first time I've ever cracked the top 1,000 for solutions.

**Part 1**

For Part 1, we'll go sub-optimal, as the difference between Part 1 / Part 2 isn't as big as you think it is.

So the core problem is how to keep track of the permuations.  Now I'm going to go BFS here, but DFS is probably equally applicable, but for some of the optimisations in Part 2, I personally think BFS makes things easier.

So the basic algorithm is going to be for a given state, adding all combinations of things to a queue, and eventually determining what the maximum number of Geodes we can end up with.

Something like:

    Add initial Queue State (24 minutes, no resources, 1 Ore Bot)

    WHILE Queue is not Empty

      Pop a Queue Entry from the Queue

      IF the Queue Entry indicates we have more Geodes than the Best Geode Count
        Update the Best Geode Count
      END

      IF the Queue Entry indicates we've run out of time
        SKIP the rest of this cycle
      END

      IF we have enough Obsidian AND Ore to build a Geode Bot
        Add a new Queue Entry with Obsidian/Ore Costs deducted, and a Geode Bot Added, and 1 Minute Deducted, and all the resources incremented by the current number of bots
      END

      IF we have enough Clay AND Ore to build an Obsidian Bot
        Add a new Queue Entry with Clay/Ore Costs deducted, and an Obsidian Bot Added, and 1 Minute Deducted, and all the resources incremented by the current number of bots
      END

      IF we have enough Ore to build a Clay Bot
        Add a new Queue Entry with Ore Costs deducted, and a Clay Bot Added, and 1 Minute Deducted, and all the resources incremented by the current number of bots
      END

      IF we have enough Ore to build an Ore Bot
        Add a new Queue Entry with Ore Costs deducted, and an Ore Bot Added, and 1 Minute Deducted, and all the resources incremented by the current number of bots
      END

      Add a new Queue Entry with 1 Minute Deducted, and all the resources incremented by the current number of bots

    WEND

Note: An important thing to remember for ", and all the resources incremented by the number of bots" is that this is the number of bots indicated by the queue.  Adding a bot does not impact this, and won't impact this until the *next* minute.

**Part 2**

So the problem with Part 1 is it's slow, and won't scale to Part 2, which increases the time to 32 minutes, but at least only requires we consider the first three blueprints.

But we just need to consider that sometimes, adding bots may be obviously the right or wrong move to make, and to eliminate those.  Essentially each time we can put a rule in place to prevent a queue entry added, we remove a whole branch of queues that would be created from that single entry.  With enough rules, we can take a solution from hours/minutes to ms.

Rule 1:

So let's go with the obvious first.  All we care about is Geodes.  So if you can add a Geode Bot, you can safely ignore *every* other move that cycle.

Rule 2:

Now for the bots themselves.  Because we can only ever create 1 bot per minute, there's a limit to how many bots will ever be useful.

The only reason to build an Obsidian Bot is to get enough Obsidian to build Geode Bots.  Once you're collecting enough Obsidian each minute to build 1 Geode Bot each minute, you don't need any more, as you can *never* build more than 1 Geode Bot per minute.

The same is true for Clay Bots, the only point building them is to build Obsidian Bots.

Ore Bots is more interesting, as that resource is shared by Geode/Obsidian/Clay Bots, however given we can only make *one* of those 3 each minute, it just means whichever is the *most* expensive in terms of Ore shoud be our target Ore Bot count.  Once we go over that, we don't need any more Ore Bots.

Rule 3:

So the next one is propogation delay.  Specifically, we have a chain of events.  It takes 1 minute to build a bot, and it will only increase the resources 1 minute thereafter.  Meaning best case to build the next bot in the chain is 2 minutes.

    i.e.

    Minute 1: We have enough Ore to build a Clay Bot, do that.

    Minute 2: We collect some clay.  We now have enough Ore/Clay to build an Obsidian Bot, do that...

In other words, we have a chain.  Ore => Clay => Obsidian => Geode.  And each of these links has a 2 minute propogation delay, meaning.

    1. Only build Geode Bots if there's 2 or more minutes remaining.
    2. Only build Obsidian Bots if there's 4 or more minutes remaining.
    3. Only build Clay Bots if there's 6 or more minutes remaining.
    4. Only build Ore Bots if there's 8 or more minutes remaining.

*Dodgy* Rules:

So the ones I've done so far are solid, but the rest are ones that appear to work, but I haven't conclusively proved them / thought it through.

1. Much like Rule 1, building Obsidian Bots always seems to be preferable to Ore/Clay Bots, so if this occurs, *ignore* all other moves.

2. Similar to Rule 1.  Building an Ore or Clay Bot seems preferable to doing nothing, so if either of these occur, ignore the move to just tick along minutes without building a bot.

But the last one can go a bit further, if the only reason we didn't Build an Ore or Clay Bot is due to Rule 2 (max bots reached), we can still use this as a criteria for skipping the move to tick along minutes without building a bot.

These *dodgier* rules take my execution time in Python down to absurdly fast speeds, but I can't be confident these are general case.

**Python vs C++ vs C**

**Python**

As per the write-up above, and of course, with the *dodgy* rules implemented.

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

TBD - Playing catch-up with multiple solutions now as the puzzles get harder!