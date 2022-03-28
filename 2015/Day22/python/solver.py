import time
import math
import sys

def main() :

    # Read/Clean the Input File to extract the Boss Stats
    bossStats = {}
    with open("../input.txt", "r") as inputFile:
        bossStats = {v[0] : int(v[1]) for line in inputFile.readlines() for v in [line.strip().split(": ")]}
    #end

    SPELLS = {
        "Magic Missile" : {"Mana Cost" :  53, "Damage" : 4                              },
        "Drain"         : {"Mana Cost" :  73, "Damage" : 2, "Heal" :   2                },
        "Shield"        : {"Mana Cost" : 113,               "Buff" :   7, "Duration" : 6},
        "Poison"        : {"Mana Cost" : 173,               "Buff" :   3, "Duration" : 6},
        "Recharge"      : {"Mana Cost" : 229,               "Buff" : 101, "Duration" : 5}
    }

    def playGames(player, boss, SPELLS, hardMode, manaSpend = 0, minManaSpend = math.inf) :

        def checkEndGame(player, boss, manaSpend) :

            nonlocal localMinManaSpend

            if manaSpend >= localMinManaSpend :
                return True
            #end

            if (player["Hit Points"] <= 0) :
                return True
            #end

            if (boss["Hit Points"] <= 0) :
                localMinManaSpend = manaSpend
                return True
            #end

            return False

        #end

        def processCooldowns(player, SPELLS) :

            # Shield
            if player["Shield"] > 0 :
                player["Shield"] -= 1
                localPlayer["Armor"] = SPELLS["Shield"]["Buff"]
            else :
                localPlayer["Armor"] = 0
            #end

            # Poison
            if player["Poison"] > 0 :
                player["Poison"] -= 1
                localPlayer["Damage"] = SPELLS["Poison"]["Buff"]
            else :
                localPlayer["Damage"] = 0
            #end

            # Recharge
            if player["Recharge"] > 0 :
                player["Recharge"] -= 1
                player["Mana"] += SPELLS["Recharge"]["Buff"]
            #end

        #end

        localMinManaSpend = minManaSpend
        for spellName,spell in SPELLS.items() :

            localPlayer       = {K: V for K,V in player.items()}
            localBoss         = {K: V for K,V in boss.items()}
            localManaSpend    = manaSpend

            # Hard Mode
            if (True == hardMode) :
                localPlayer["Hit Points"] -= 1
            #end

            if True == checkEndGame(localPlayer, localBoss, localManaSpend) :
                continue
            #end

            # Player Turn

            # Perform all Spells involving Cooldowns
            processCooldowns(localPlayer, SPELLS)

            # Determine if the Current Move is Viable
            if spell["Mana Cost"] <= localPlayer["Mana"] :

                if "Damage" in spell.keys() :
                    localBoss["Hit Points"] -= spell["Damage"]
                #end

                if "Heal" in spell.keys() :
                    localPlayer["Hit Points"] += spell["Heal"]
                #end

                if "Duration" in spell.keys() :
                    if localPlayer[spellName] == 0 :
                        localPlayer[spellName] = spell["Duration"]
                    #end
                #end

                # Spend the Mana
                localManaSpend      += spell["Mana Cost"]
                localPlayer["Mana"] -= spell["Mana Cost"]

            else :

                continue

            #end

            # Perform any Damage Over Time Actions
            localBoss["Hit Points"] -= localPlayer["Damage"]

            if True == checkEndGame(localPlayer, localBoss, localManaSpend) :
                continue
            #end

            # Boss Turn

            # Perform all Spells involving Cooldowns
            processCooldowns(localPlayer, SPELLS)

            # Perform any Damage Over Time Actions
            localBoss["Hit Points"] -= localPlayer["Damage"]

            if True == checkEndGame(localPlayer, localBoss, localManaSpend) :
                continue
            #end

            # Perform the Boss Attack
            localPlayer["Hit Points"] -= (localBoss["Damage"] - localPlayer["Armor"])

            if True == checkEndGame(localPlayer, localBoss, localManaSpend) :
                continue
            #end

            # Recurse...
            localMinManaSpend = playGames(localPlayer, localBoss, SPELLS, hardMode, localManaSpend, localMinManaSpend)

        #end

        return localMinManaSpend

    #end

    playerState = {"Hit Points" : 50, "Mana" : 500, "Damage" : 0, "Armor" : 0, "Shield" : 0, "Poison" : 0, "Recharge" : 0}

    print(f"Part 1: {playGames(playerState, bossStats, SPELLS, False)}")
    print(f"Part 2: {playGames(playerState, bossStats, SPELLS, True)}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
