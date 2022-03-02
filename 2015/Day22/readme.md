# Day 22

https://adventofcode.com/2015/day/22

On the face of it, it may be tempting to assume this is the same as Day 21, which is only partially true.  This is Day 21 *IF* Day 21 allowed the Weapon/Armor/Rings to be changed every turn.  As a result, this has far more permutations compared to Day 21.  The other tricky aspect is that this now has multi-turn actions, so those need to be kept track of.

**Part 1**

Unlike Day 21, which was a relatively simple set of nested FOR loops, this is really more a recursive issue, and can be solved with a relatively simple algorithm.

    RECURSIVE FUNCTION:
    
    FOR Each Spell
    
      PLAYER TURN:
    
        Handle any Spell Cooldowns

        Cast the current Spell if possible, if not, skip this spell and move to the next one
      
      BOSS TURN:
      
        Handle any Spell Cooldowns
        
        Boss Attacks the Player

      RECURSE to play the next round...

    NEXT

After each turn (Player and Boss), check whether the Player/Boss is still alive.  If the Player has won, compare the mana spend so far to the current lowest spend.  Keep going until we've found the lowest cost of winning.

There's some obvious early exits, for example, if we've already spent more Mana than the current lowest spend, treat the game as being over.

Whilst this solution is good enough, it is again fodder for something more akin to a BFS solution, whereby we could simply queue actions, and associated total mana spend, and use that to sort what spells should be processed first.  One inherent weakness of Depth First Searches (DFS) is that they can often infinitely recurse if not properly constrained.  For example, let's add a new spell, and call it CHEATER, this spell costs 0 Mana and recovers 50 HP each time.  Meaning a Boss's attack will never kill the player, and the player will never run out of mana.  If that is the first spell processed each time, we'll end up doing the following:

    RECURSIVE Function:
    
    FOR Each Spell (first is CHEATER)
    
      PLAYER TURN:

        No Cooldowns as we've not cast anything yet

        Cast the current Spell (CHEATER), which recovers our health to max
        
    BOSS TURN:

        No Cooldowns as CHEATER is an instant heal
        
        BOSS Attacks the Player

        RECURSE......
    
        FOR Each Spell (first is CHEATER)

          PLAYER TURN:

            No Cooldowns as CHEATER is an instant heal

            Cast the current Spell (CHEATER), which recovers our health to max

        BOSS TURN:

            No Cooldowns as CHEATER is an instant heal

            BOSS Attacks the Player

            RECURSE......
            
            etc.

There are mitigation steps to help there, but one thing this challenge will expose overall is that whilst DFS is great for permutation checking, there are times when it just isn't viable.

The solution here is borderline viable... I may revisit this depending upon how the low level solutions perform.

**Part 2**

This is Part 1, but before the Player Turn, subtract 1 from the Players Health and check whether the Player has died as a result.

**Python vs C++ vs C**

**Python**

In some ways dictionaries help in so far as we can store unique data to each spell, and even use it as a means of distinguishing whether a spell is an instant action, or an action over time, however this does come with a cost in terms of throughput, as each iteration does alot of key/array testing.

**C++**

Pretty disastrous compared to the Python solution.  This ends up almost being twice as slow.  Part of this is invariably down to the sorting issue I touched upon in the Part 1 solution.  Because the spells in both this and Python just iterate through a hash map, the order in which the spells are processed invariably ends up working to Pythons benefit and C++'s detriment (as there's no reason at all they would both iterate over a hash map in the same order).

Since the C solution was absurdly fast with the same DFS approach, I've left this as-is for prosperity.

**C**

No matter how many times I see it, it's incredible just how much overhead some seemingly innocuous abstractions can add.  Whereas the Python/C++ solutions are close to being unbearably slow, the C solution is near instantaneous (18x faster than Python, 36x faster than C++).  Part of this will invariably be that it "accidently" takes a better first route through the recursion, but also the removal of all abstractions with regards to arrays/indexing.  Whereas the C++/Python solutions have to do repeated hashing in order to perform cooldown checks / spell lookups, this goes with a simpler numerical index via an enum.

Again, this can all be fed back into the Python/C++ solutions, but I think in a way it's more interesting to show just how much overhead is added with these kinds of abstractions.
