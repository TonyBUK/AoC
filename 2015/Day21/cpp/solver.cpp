#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cinttypes>
#include <assert.h>

std::vector<std::string> split(const std::string& s, const std::string& seperator)
{
    std::vector<std::string> output;
    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );
        output.push_back(substring);
        pos     += seperator.length();
        prev_pos = pos;
    }
    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word
    return output;
}

#define MAX_SIZE_T static_cast<std::size_t>(static_cast<uint64_t>(-1))

struct coreStatsType
{
    int64_t         nDamage;
    int64_t         nArmor;
};

struct playerType
{
    coreStatsType   kCoreStats;
    int64_t         nHealth;
};

struct itemType
{
    coreStatsType   kCoreStats;
    int64_t         nCost;
};

itemType initialiseItem(const int64_t nCost, const int64_t nDamage, const int64_t nArmor)
{
    const itemType kItem = {{nDamage, nArmor}, nCost};
    return kItem;
}

bool playGame(const playerType& kPlayerStats, const playerType& kBossStats)
{
    int64_t nPlayerHealth = kPlayerStats.nHealth;
    int64_t nBossHealth   = kBossStats.nHealth;

    while (true)
    {
        // Player Turn
        if (kPlayerStats.kCoreStats.nDamage > kBossStats.kCoreStats.nArmor)
        {
            nBossHealth -= kPlayerStats.kCoreStats.nDamage - kBossStats.kCoreStats.nArmor;
            if (nBossHealth <= 0) return true;
        }

        // Boss Turn
        if (kBossStats.kCoreStats.nDamage > kPlayerStats.kCoreStats.nArmor)
        {
            nPlayerHealth -= kBossStats.kCoreStats.nDamage - kPlayerStats.kCoreStats.nArmor;
            if (nPlayerHealth <= 0) return false;
        }
    }
}

void playGamePermutations(const std::vector<itemType>& kWeapons,
                          const std::vector<itemType>& kArmor,
                          const std::vector<itemType>& kRings,
                          const playerType&            kBossStats,
                          const int64_t                nPlayerHealth,
                          std::size_t&                 nLeastGold,
                          std::size_t&                 nMostGold)
{
    nLeastGold = MAX_SIZE_T;
    nMostGold  = 0;

    // Precalculate the Ring Permutations
    std::vector< std::vector<itemType> > kRingPermutations;
    for (std::size_t i = 0; i < kRings.size(); ++i)
    {
        for (std::size_t j = i + 1; j < kRings.size(); ++j)
        {
            std::vector<itemType> kRingPermutation;
            kRingPermutation.push_back(kRings.at(i));
            kRingPermutation.push_back(kRings.at(j));
            kRingPermutations.push_back(kRingPermutation);
        }
    }

    // For each possible Load Out
    for (std::vector<itemType>::const_iterator itWeapon = kWeapons.cbegin(); itWeapon != kWeapons.cend(); ++itWeapon)
    {
        const itemType& kWeapon = *itWeapon;
        for (std::vector<itemType>::const_iterator itArmor = kArmor.cbegin(); itArmor != kArmor.cend(); ++itArmor)
        {
            const itemType& kArmor = *itArmor;
            for (std::vector< std::vector<itemType> >::const_iterator itRings = kRingPermutations.cbegin(); itRings != kRingPermutations.cend(); ++itRings)
            {
                const std::vector<itemType>& kRingPermutation = *itRings;
                const itemType&              kRing1           = kRingPermutation.at(0);
                const itemType&              kRing2           = kRingPermutation.at(1);

                const playerType             kPlayerStats     = {{kWeapon.kCoreStats.nDamage + kArmor.kCoreStats.nDamage + kRing1.kCoreStats.nDamage + kRing2.kCoreStats.nDamage,
                                                                  kWeapon.kCoreStats.nArmor  + kArmor.kCoreStats.nArmor  + kRing1.kCoreStats.nArmor  + kRing2.kCoreStats.nArmor},
                                                                 nPlayerHealth};
                const std::size_t            nCost            = static_cast<std::size_t>
                                                                (kWeapon.nCost               + kArmor.nCost              + kRing1.nCost              + kRing2.nCost);

                // Play the Game with the current Loadout
                // If we win, we're interested in the least gold used,
                // otherwise we're interested in the most
                if (playGame(kPlayerStats, kBossStats))
                {
                    if (nCost < nLeastGold) nLeastGold = nCost;
                }
                else
                {
                    if (nCost > nMostGold) nMostGold = nCost;
                }
            }
        }
    }
/*
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
*/
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        playerType              kBoss;
        std::vector<itemType>   kWeapons;
        std::vector<itemType>   kArmor;
        std::vector<itemType>   kRings;

        // Define the Weapons
        kWeapons.push_back(initialiseItem(  8, 4, 0)); // Dagger
        kWeapons.push_back(initialiseItem( 10, 5, 0)); // Shortsword
        kWeapons.push_back(initialiseItem( 25, 6, 0)); // Warhammer
        kWeapons.push_back(initialiseItem( 40, 7, 0)); // Longsword
        kWeapons.push_back(initialiseItem( 74, 8, 0)); // Greataxe

        // Define the Armor
        kArmor.push_back  (initialiseItem(  0, 0, 0)); // None
        kArmor.push_back  (initialiseItem( 13, 0, 1)); // Leather
        kArmor.push_back  (initialiseItem( 31, 0, 2)); // Chainmail
        kArmor.push_back  (initialiseItem( 53, 0, 3)); // Splintmail
        kArmor.push_back  (initialiseItem( 75, 0, 4)); // Bandedmail
        kArmor.push_back  (initialiseItem(102, 0, 5)); // Platemail

        // Define the Rings
        kRings.push_back  (initialiseItem(  0, 0, 0)); // None
        kRings.push_back  (initialiseItem(  0, 0, 0)); // None
        kRings.push_back  (initialiseItem( 25, 1, 0)); // Damage +1
        kRings.push_back  (initialiseItem( 50, 2, 0)); // Damage +2
        kRings.push_back  (initialiseItem(100, 3, 0)); // Damage +3
        kRings.push_back  (initialiseItem( 20, 0, 1)); // Defense +1
        kRings.push_back  (initialiseItem( 40, 0, 2)); // Defense +2
        kRings.push_back  (initialiseItem( 80, 0, 3)); // Defense +3

        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);

            const std::vector<std::string> kSplit = split(kLine, ": ");
            assert(kSplit.size() == 2);
            if ("Hit Points" == kSplit.at(0))
            {
                kBoss.nHealth = std::stold(kSplit.at(1));
            }
            else if ("Damage" == kSplit.at(0))
            {
                kBoss.kCoreStats.nDamage = std::stold(kSplit.at(1));
            }
            else if ("Armor" == kSplit.at(0))
            {
                kBoss.kCoreStats.nArmor = std::stold(kSplit.at(1));
            }
            else
            {
                assert(false);
            }
        }

        // Solve the Puzzle
        std::size_t nLeastGold;
        std::size_t nMostGold;
        playGamePermutations(kWeapons, kArmor, kRings, kBoss, 100, nLeastGold, nMostGold);
        std::cout << "Part 1: " << nLeastGold << std::endl;
        std::cout << "Part 2: " << nMostGold  << std::endl;
   }

    return 0;
}
