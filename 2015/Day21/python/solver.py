import time
import math
import sys

def main() :

    # Read/Clean the Input File to extract the Boss Stats
    bossStats = {}
    with open("../input.txt", "r") as inputFile:
        bossStats = {v[0] : int(v[1]) for line in inputFile.readlines() for v in [line.strip().split(": ")]}
    #end

    WEAPONS = [
    #"Dagger"     :
                    {"Cost" :   8, "Damage" : 4, "Armor" : 0},
    #"Shortsword" :
                    {"Cost" :  10, "Damage" : 5, "Armor" : 0},
    #"Warhammer"  :
                    {"Cost" :  25, "Damage" : 6, "Armor" : 0},
    #"Longsword"  :
                    {"Cost" :  40, "Damage" : 7, "Armor" : 0},
    #"Greataxe"   :
                    {"Cost" :  74, "Damage" : 8, "Armor" : 0}
    ]

    # Note: To simplify things, a "None" armor set is added
    ARMOR = [
    # None        :
                    {"Cost" :   0, "Damage" : 0, "Armor" : 0},
    #"Leather"    :
                    {"Cost" :  13, "Damage" : 0, "Armor" : 1},
    #"Chainmail"  :
                    {"Cost" :  31, "Damage" : 0, "Armor" : 2},
    #"Splintmail" :
                    {"Cost" :  53, "Damage" : 0, "Armor" : 3},
    #"Bandedmail" :
                    {"Cost" :  75, "Damage" : 0, "Armor" : 4},
    #"Platemail"  :
                    {"Cost" : 102, "Damage" : 0, "Armor" : 5}
    ]

    # Note: To simplify things, two "None" ring sets are added
    #       to allow 0, 1 or 2 rings to be held.  By making
    #       two rings 0 cost but 0 buffs, we effectively
    #       achieve this with 2 rings always held, only two
    #       are in the category of "Emporers New Rings"
    RINGS = [
    # None        :
                    {"Cost" :   0, "Damage" : 0, "Armor" : 0},
    # None        :
                    {"Cost" :   0, "Damage" : 0, "Armor" : 0},
    #"Damage +1"  :
                    {"Cost" :  25, "Damage" : 1, "Armor" : 0},
    #"Damage +2"  :
                    {"Cost" :  50, "Damage" : 2, "Armor" : 0},
    #"Damage +3"  :
                    {"Cost" : 100, "Damage" : 3, "Armor" : 0},
    #"Defense +1" :
                    {"Cost" :  20, "Damage" : 0, "Armor" : 1},
    #"Defense +2" :
                    {"Cost" :  40, "Damage" : 0, "Armor" : 2},
    #"Defense +3" :
                    {"Cost" :  80, "Damage" : 0, "Armor" : 3}
    ]

    def playGame(PLAYER_STATS, BOSS_STATS, playerHealth) :

        bossHealth = BOSS_STATS["Hit Points"]

        while True :

            # Player Turn
            if PLAYER_STATS["Damage"] > BOSS_STATS["Armor"] :
                bossHealth -= PLAYER_STATS["Damage"] - BOSS_STATS["Armor"]
                if bossHealth <= 0 : return True
            #end

            # Boss Turn
            if BOSS_STATS["Damage"] > PLAYER_STATS["Armor"] :
                playerHealth -= BOSS_STATS["Damage"] - PLAYER_STATS["Armor"]
                if playerHealth <= 0 : return False
            #end

        #end

    #end

    def playGamePermutations(WEAPONS, ARMOR, RINGS, BOSS_STATS, playerHealth) :

        leastGold = math.inf
        mostGold  = -1

        # Precalculate the Ring Permutations
        ringPermutations = []
        for i in range(len(RINGS) + 1) :
            for j in range(i + 1, len(RINGS)) :
                ringPermutations.append(tuple([i,j]))
            #end
        #end

        # For each possible Load Out
        for weapon in WEAPONS :
            for armor in ARMOR :
                for ringPermutation in ringPermutations :

                    # Determine the Basic Stats for the Loadout
                    playerStats = {
                        "Armor"  : weapon["Armor"] +
                                   armor["Armor"] +
                                   RINGS[ringPermutation[0]]["Armor"] +
                                   RINGS[ringPermutation[1]]["Armor"],
                        "Damage" : weapon["Damage"] +
                                   armor["Damage"] +
                                   RINGS[ringPermutation[0]]["Damage"] +
                                   RINGS[ringPermutation[1]]["Damage"],
                    }

                    # Calculate Cost of the Loadout
                    cost = (weapon["Cost"] +
                            armor["Cost"] +
                            RINGS[ringPermutation[0]]["Cost"] +
                            RINGS[ringPermutation[1]]["Cost"])
                
                    # Play the Game with the current Loadout
                    # If we win, we're interested in the least gold used,
                    # otherwise we're interested in the most
                    if playGame(playerStats, BOSS_STATS, playerHealth) :
                        if cost < leastGold : leastGold = cost
                    else :
                        if cost > mostGold  : mostGold  = cost
                    #end
                #end
            #end
        #end

        return leastGold, mostGold

    #end

    # Determine Least/Most Gold for each Load Out
    leastGold, mostGold = playGamePermutations(WEAPONS, ARMOR, RINGS, bossStats, 100)
    print(f"Part 1: {leastGold}")
    print(f"Part 2: {mostGold}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
