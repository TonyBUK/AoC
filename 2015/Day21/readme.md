# Day 21

https://adventofcode.com/2015/day/21

This one is about the ability to manage data / handle permutations.  Arguably a fairly simple puzzle considering how far along in calendar this lands.

**Part 1**

The only real complication here is the permutations being:

Weapons : Always exactly 1 weapon equipped, so a simple FOR loop here.
Armor   : 0 or 1 items of armor equipped, so not quite as simple as Weapons as we need to also handle not wearing Armor.
Rings   : 0, 1 or 2 rings equipped, but only 1 of each kind of ring at the same time.

So time to cheat...

1.  Add a single dummy item to Armor List to signify not wearing Armor.  All stats/costs will be 0.
2.  Add two dummy items to the Ring List to signify not wearing Rings.  All stats/costs will be 0.

With this in mind, we now have a condition for a relatively simple set of FOR loops.

    FOR each Weapon
    
      FOR each Armor (including the dummy one)
      
        FOR each Pair of rings (including the dummy ones)
        
          Calculate the Player Stats from the Loadout

          Calculate the cost of the Loadout

          Fight the Boss

          IF we've won, compare the loadout cost to the current cheapest winning loadout

        NEXT

      NEXT

    NEXT

We can even pre-calculate the Ring Pairs, especially since we have the dummy entries, with a pair of FOR loops.

    FOR each Ring for the First Ring
    
      FOR each Ring *after* the First Ring

        Add the current Two Rings to the list of viable Pairs

      NEXT

    NEXT

**Part 2**

This is a carbon copy of Part 1, except instead of checking how little we've spent when we win, we're now interested in how much we've spent when we lose.  In-fact, given the fights themselves never change, we can check both in one go.


    FOR each Weapon
    
      FOR each Armor (including the dummy one)
      
        FOR each Pair of rings (including the dummy ones)
        
          Calculate the Player Stats from the Loadout

          Calculate the cost of the Loadout

          Fight the Boss

          IF we've won, compare the loadout cost to the current cheapest winning loadout

          Otherwise..., compare the loadout cost to the current most expensive losing loadout

        NEXT

      NEXT

    NEXT

**Python vs C++ vs C**

**Python**

Pretty much the algorithm as described.  Using a dictionary is hardly the most efficient way of doing it, but the task itself is expressible in milliseconds, so not exactly a big deal.

**C++**

**C**

